#include "zetnet.h"


#define  	ZN_MAX_BYTES_TO_SEND	512

#define  	ZN_MAX_TRIES_BAD_MESSAGE	3
#define  	ZN_TIME_WAIT_ACK		3000	//  ms

#define 	ZN_DEFAULT_TIMEOUT_SECONDS 0
#define 	ZN_SOCKET_CLIENT_NOT_AVAILABLE -1


const char * ZN_SERVER_FULL     = "FULL";

void  * ZN_TcpServer_Update(void * varg);


void ZN_TcpServer_SetTimeout(ZN_TcpServer * tcp_server,int seconds){
	tcp_server->timeout = (struct timeval){
		.tv_sec=seconds
		,.tv_usec=0};   // sleep for ten minutes!
}


SOCKET ZN_TcpServer_SocketAccept(ZN_TcpServer *tcp_server)
{
    SOCKET newsockfd = INVALID_SOCKET;
    SOCKET max_sd;
    int activity;

    if (!tcp_server || tcp_server->sockfd == INVALID_SOCKET) {
        return INVALID_SOCKET;
    }

    FD_ZERO(&tcp_server->readfds);

    FD_SET(tcp_server->sockfd, &tcp_server->readfds);
    max_sd = tcp_server->sockfd;

    for (int i = 0; i < ZN_TCP_SERVER_MAX_CLIENTS; i++) {
        SOCKET sd = tcp_server->clients[i].socket;

        if (sd != INVALID_SOCKET) {
            FD_SET(sd, &tcp_server->readfds);

            if (sd > max_sd) {
                max_sd = sd;
            }
        }
    }

    ZN_TcpServer_SetTimeout(tcp_server, 1);

#ifdef _WIN32
    activity = select(0, &tcp_server->readfds, NULL, NULL, &tcp_server->timeout);
#else
    activity = select((int)(max_sd + 1), &tcp_server->readfds, NULL, NULL, &tcp_server->timeout);
#endif

    if (activity == 0) {
        return INVALID_SOCKET;
    }

    if (activity < 0) {
#ifdef _WIN32
        int wsa_error = WSAGetLastError();

        if (wsa_error != WSAEINTR) {
            fprintf(stderr, "\nselect failed with error: %d\n", wsa_error);
        }
#else
        if (errno != EINTR) {
            fprintf(stderr, "\nselect failed with error: %d\n", errno);
        }
#endif
        return INVALID_SOCKET;
    }

    if (!FD_ISSET(tcp_server->sockfd, &tcp_server->readfds)) {
        return INVALID_SOCKET;
    }

#ifdef _WIN32
    newsockfd = accept(tcp_server->sockfd, NULL, NULL);
    if (newsockfd == INVALID_SOCKET) {
        fprintf(stderr, "\naccept failed with error: %d\n", WSAGetLastError());
        return INVALID_SOCKET;
    }
#else
    {
        struct sockaddr_storage cli_addr;
        socklen_t clilen = sizeof(cli_addr);

        newsockfd = accept(tcp_server->sockfd,
                           (struct sockaddr *)&cli_addr,
                           &clilen);

        if (newsockfd == INVALID_SOCKET) {
            if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) {
                fprintf(stderr, "\naccept failed with error: %d\n", errno);
            }
            return INVALID_SOCKET;
        }
    }
#endif

    return newsockfd;
}

bool ZN_TcpServer_SocketReady(ZN_TcpServer * tcp_server,SOCKET sock){

	if (FD_ISSET( sock , &tcp_server->readfds)){
		printf("Incoming connection detected\n");
		return true;
	}

	return false;
}

//--------------------------------------------------------------------
//  PUBLIC
ZN_TcpServer * ZN_TcpServer_New(ZN_TcpServerOnGestMessage on_gest_message)
{
	ZN_TcpServer * tcp_server = ZN_NEW(ZN_TcpServer);

	tcp_server->src_port=0;
	tcp_server->dst_port=0;

	tcp_server->on_gest_message=on_gest_message;


	tcp_server->initialized  =  false;


	for(int i=0; i < ZN_TCP_SERVER_MAX_CLIENTS; i++){
		tcp_server->clients[i].idx_client=i;
		tcp_server->clients[i].socket=INVALID_SOCKET;
		tcp_server->clients[i].streaming_header_sent=false;
		tcp_server->free_slots[i]=i;
	}

	tcp_server->n_free_slots=ZN_TCP_SERVER_MAX_CLIENTS;

	tcp_server->message=NULL;

	tcp_server->configured = false;

	tcp_server->thread = -1;
	tcp_server->end_loop_mdb=false;

	tcp_server->sockfd=INVALID_SOCKET;
	tcp_server->portno=-1;
	tcp_server->time_delay_ms=10; // 10ms delay

	tcp_server->is_streaming_server=false;



	return tcp_server;
}

//-------------------------------------------------------------------------------------
void ZN_TcpServer_SetTimeDelay(ZN_TcpServer * tcp_server,unsigned long delay){
	tcp_server->time_delay_ms = delay;
}

const char * ZN_TcpServer_GetErrorSockOpt(void){
	switch(errno){
	case EBADF:
		return "The argument sockfd is not a valid descriptor.";
	case EFAULT:
		return "The address pointed to by optval is not in a valid part of the process address space. For getsockopt(), this error may also be returned if optlen is not in a valid part of the process address space.";
	case EINVAL:
		return "optlen invalid in setsockopt(). In some cases this error can also occur for an invalid value in optval (e.g., for the IP_ADD_MEMBERSHIP option described in ip(7)).";
	case ENOPROTOOPT:
		return "The option is unknown at the level indicated.";
	case ENOTSOCK:
		return "The argument sockfd is a file, not a socket.";
	}

	return "unknow";
}
//---------------------------------------------------------------------------------------------------------------------------
bool ZN_TcpServer_Setup(ZN_TcpServer *tcp_server, const char *_host, int _portno)
{
    if (!tcp_server) {
        return false;
    }

    tcp_server->end_loop_mdb = false;
    tcp_server->portno = _portno;

    ZN_TcpServer_SetTimeout(tcp_server, ZN_DEFAULT_TIMEOUT_SECONDS);

    tcp_server->sockfd = ZN_TcpSocket_NewSocketServer(_host, _portno);
    if (tcp_server->sockfd == INVALID_SOCKET) {
        return false;
    }

    if (pthread_create(&tcp_server->thread, NULL, ZN_TcpServer_Update, (void *)tcp_server) != 0) {
        fprintf(stderr, "\nerror creating thread\n");
        ZN_TcpSocket_CloseSocket(&tcp_server->sockfd);
        tcp_server->thread = 0;
        tcp_server->initialized = false;
        return false;
    }

    tcp_server->initialized = true;
    return true;
}

bool ZN_TcpServer_Start(ZN_TcpServer * tcp_server, const char *_host,  int _portno){
	if(ZN_TcpServer_Setup(tcp_server,_host, _portno)){
		return true;
	}
	return false;
}

//---------------------------------------------------------------
bool  ZN_TcpServer_IsConnected(ZN_TcpServer * tcp_server) {
	return  tcp_server->sockfd != INVALID_SOCKET;
}

//--------------------------------------------------------------------
ZN_TcpServerClient * ZN_TcpServer_GetFreeSlot(ZN_TcpServer * tcp_server){

	ZN_TcpServerClient *cs=NULL;

	if(tcp_server->n_free_slots > 0){

		if(tcp_server->free_slots[tcp_server->n_free_slots-1] != ZN_SOCKET_CLIENT_NOT_AVAILABLE){
			cs = &tcp_server->clients[tcp_server->free_slots[tcp_server->n_free_slots-1]];
			tcp_server->free_slots[tcp_server->n_free_slots-1]=ZN_SOCKET_CLIENT_NOT_AVAILABLE;
			tcp_server->n_free_slots--;
		}else{
			fprintf(stderr,"\ninternal error!\n");
			return NULL;
		}
	}
	else{
		fprintf(stderr,"\nno space left!!\n");
	}
	return cs;
}

bool ZN_TcpServer_CloseClient(ZN_TcpServer * tcp_server,ZN_TcpServerClient *clients){

	if(tcp_server->n_free_slots < ZN_TCP_SERVER_MAX_CLIENTS){

		if(clients->socket != INVALID_SOCKET){

			//socketDel(clientSock->socket);
#ifdef _WIN32
			// in wsa we have to shutdown client socket connection by default recv and send operations
			int iResult = shutdown(clients->socket, SD_BOTH);
			if (iResult == SOCKET_ERROR) {
				fprintf(stderr,"\nshutdown failed with error: %d\n", WSAGetLastError());
			}
#endif

			// close socket client...
			ZN_TcpSocket_CloseSocket(&clients->socket);

			clients->socket=INVALID_SOCKET;
			clients->streaming_header_sent=false;
			tcp_server->free_slots[tcp_server->n_free_slots]=clients->idx_client;
			tcp_server->n_free_slots++;


			return true;
		}
		else{
			fprintf(stderr,"\nCannot ZN_FREE because -SOCKET IS NOT AVAILABLE-\n");
		}
	}
	else{
		fprintf(stderr,"\nCannot ZN_FREE because -ZN_TCP_SERVER_MAX_CLIENTS REACHED-\n");
	}
	return false;
}

//---------------------------------------------------------------------------------------------------------------
/*bool ZN_TcpServer_GestServerBase(ZN_TcpServer * tcp_server)
{
	//int  numready=0;

	return true;
}*/

void ZN_TcpServer_GestServer(ZN_TcpServer * tcp_server)
{
	//gestServerBase();

	SOCKET new_socket = INVALID_SOCKET;


	// accept socket client (you're ready to get / send messages from/to this socket)...

	if((new_socket =ZN_TcpServer_SocketAccept(tcp_server)) != INVALID_SOCKET){

		ZN_TcpServerClient *clients= ZN_TcpServer_GetFreeSlot(tcp_server);

		if(clients==NULL){ // no space left ... reject client ...
			fprintf(stderr,"\n*** Maximum client count reached - rejecting client connection ***\n");
			ZN_TcpSocket_CloseSocket(&new_socket);
		}else{

#if __DEBUG__
			printf("Adding new client\n");
#endif

			clients->socket=new_socket;
		}
	}

	for (int cn = 0; cn < ZN_TCP_SERVER_MAX_CLIENTS; cn++)  {
		// If the socket is ready (i.e. it has data we can read)... (SDLNet_SocketReady returns non-zero if there is activity on the socket, and zero if there is no activity)
		if(tcp_server->clients[cn].socket != INVALID_SOCKET){
			int client_socket_activity = ZN_TcpServer_SocketReady(tcp_server,tcp_server->clients[cn].socket);
#if __DEBUG__
		//	printf("Just checked client number %i  and received activity status is: %i\n", clientNumber,clientSocketActivity);
#endif
			// If there is any activity on the client socket...
			if (client_socket_activity != 0){

				ssize_t result = 0;

				if (!tcp_server->is_streaming_server) {
				    result = ZN_TcpSocket_ReceiveBytes(
				        tcp_server->clients[cn].socket,
				        (uint8_t *)tcp_server->buffer,
				        sizeof(tcp_server->buffer)
				    );
				}

				if (result > 0) {
				    ZN_TcpServerOnGestMessage cf = tcp_server->on_gest_message;

				    if (!cf.callback_function) {
				        ZN_LOG_DEBUG("gestMessage: Erasing client %i because callback is NULL", cn);
				        ZN_TcpServer_CloseClient(tcp_server, &tcp_server->clients[cn]);
				        continue;
				    }

				    if (!cf.callback_function(
				            tcp_server,
				            &tcp_server->clients[cn],
				            tcp_server->buffer,
				            (size_t)result,
				            cf.user_data
				    )) {

						ZN_LOG_DEBUG("gestMessage:Erasing client %i (gestMessage)",cn);

						ZN_TcpServer_CloseClient(tcp_server,&tcp_server->clients[cn]);
					}
				}else if(result == 0){
					// peer disconnected
					ZN_TcpServer_CloseClient(tcp_server,&tcp_server->clients[cn]);

				}else{ // check error to keep socket if socket is still valid or close the connection ...

				#ifdef _WIN32
					int err = WSAGetLastError();
					bool block_socket = (err == WSAEWOULDBLOCK);
				#else
					int err = errno;
					bool block_socket = (err == EAGAIN || err == EWOULDBLOCK);
				#endif

					if (block_socket) {
						// Do NOTHING.
						// Just no data available right now.
					} else {
						// Error -> close client
						// socket finished transfer so close socket.
						ZN_LOG_DEBUG("gestMessage:Erasing client %i (getMessage)",cn);
						ZN_TcpServer_CloseClient(tcp_server,&tcp_server->clients[cn]);
					}
				}

			} // End of if client socket is active check
		}
	}// End of server socket check sockets loop
}
//------------------------------------------------------------------------------------------------

void  ZN_TcpServer_GetMessage(ZN_TcpServer * tcp_server)
{
	if(tcp_server->sockfd != INVALID_SOCKET){
		ZN_TcpServer_GestServer(tcp_server);
	}
}
//--------------------------------------------------------------------
void  * ZN_TcpServer_Update(void * varg)  //  Receive  messages,  gest  &  send...
{


	if(varg != NULL){
		ZN_TcpServer * tcp_server=(ZN_TcpServer *)varg;
		//printf("reco %i\n",tcp_server->end_loop_mdb);
		if(tcp_server->sockfd != INVALID_SOCKET){
			while(!tcp_server->end_loop_mdb)
			{
				ZN_TcpServer_GetMessage(tcp_server);  //  For  server  update  connections  &  get  messages  from  clients...


				if(tcp_server->time_delay_ms>0){
		#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
				Sleep( tcp_server->time_delay_ms );
		#else
				ZN_System_SleepMicroSeconds( tcp_server->time_delay_ms * 1000UL );
		#endif
				}
			}
		}
	}


	return 0;
}


//------------------------------------------------------------------------------------------------------------------------
//char  *str  =  NULL;

void ZN_TcpServer_Stop(ZN_TcpServer *tcp_server)
{
    if (!tcp_server) {
        return;
    }

    if (tcp_server->sockfd == INVALID_SOCKET) {
        return;
    }

    tcp_server->end_loop_mdb = true;

    if (tcp_server->initialized) {
        pthread_join(tcp_server->thread, NULL);
        tcp_server->initialized = false;
    }

    for (int i = 0; i < ZN_TCP_SERVER_MAX_CLIENTS; i++) {
        if (tcp_server->clients[i].socket != INVALID_SOCKET) {
            ZN_TcpServer_CloseClient(tcp_server, &tcp_server->clients[i]);
        }
    }

    ZN_TcpSocket_CloseSocket(&tcp_server->sockfd);

    printf("Disconnect server\n");
}

void ZN_TcpServer_Delete(ZN_TcpServer * tcp_server) {
	ZN_TcpServer_Stop(tcp_server);
	ZN_FREE(tcp_server);
}


