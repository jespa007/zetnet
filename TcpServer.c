#include "zetnet.h"


#define  	MAX_BYTES_TO_SEND	512

#define  	MAX_TRIES_BAD_MESSAGE	3
#define  	TIME_WAIT_ACK		3000	//  ms

#define 	DEFAULT_TIMEOUT_SECONDS 0
#define 	SOCKET_CLIENT_NOT_AVAILABLE -1


void  * TcpServer_Update(void * varg);

void   TcpServer_Connect(TcpServer *tcp_server);
void   TcpServer_Disconnect(TcpServer *tcp_server);

const char * SERVER_FULL     = "FULL";



void TcpServer_SetTimeout(TcpServer * tcp_server,int seconds){
	tcp_server->timeout = (struct timeval){
		.tv_sec=seconds
		,.tv_usec=0};   // sleep for ten minutes!
}


SOCKET TcpServer_SocketAccept(TcpServer * tcp_server){

	SOCKET newsockfd = INVALID_SOCKET;

	//clear the socket set
	FD_ZERO(&tcp_server->readfds);

	//add master socket to set
	// Always look for connection attempts
	FD_SET(tcp_server->sockfd, &tcp_server->readfds);
	SOCKET max_sd = tcp_server->sockfd;

	//add child sockets to set
	for (int i = 0 ; i < MAX_SOCKETS ; i++)
	{
		if(tcp_server->socket_client[i].socket!=INVALID_SOCKET){
			//socket descriptor
			SOCKET sd = tcp_server->socket_client[i].socket;

			//if valid socket descriptor then add to read list
			if(sd > 0){
				FD_SET( sd , &tcp_server->readfds);
			}

			//highest file descriptor number, need it for the select function
			if(sd > max_sd){
				max_sd = sd;
			}
		}
	}

	//wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
	int activity = select( max_sd + 1 , &tcp_server->readfds , NULL , NULL , &tcp_server->timeout);

	if ((activity < 0) && (errno!=EINTR))
	{
#if _WIN32
		int wsa_error=WSAGetLastError();


		switch(wsa_error){
		default:
			fprintf(stderr,"select error:unknow error (%i)\n",wsa_error);
			break;
		case WSANOTINITIALISED:
			fprintf(stderr,"select error:A successful WSAStartup call must occur before using this function.\n");
			break;
		case WSAEFAULT:
			fprintf(stderr,"select error:The Windows Sockets implementation was unable to allocate needed resources for its internal operations, or the readfds, writefds, exceptfds, or timeval parameters are not part of the user address space.\n");
			break;
		case WSAENETDOWN:
			fprintf(stderr,"select error:The network subsystem has failed.\n");
			break;
		case WSAEINVAL:
			fprintf(stderr,"select error:The time-out value is not valid, or all three descriptor parameters were null.\n");
			break;
		case WSAEINTR:
			fprintf(stderr,"select error:A blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall.\n");
			break;
		case WSAEINPROGRESS:
			fprintf(stderr,"select error:A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.\n");
			break;
		case WSAENOTSOCK:
			fprintf(stderr,"select error:One of the descriptor sets contains an entry that is not a socket or fd_set is not valid.\n");
			break;
		}
#else
		fprintf(stderr,"select error:unknow error (%i)\n",activity);
#endif

	}

	//If something happened on the socket server, then its an incoming connection
	if (FD_ISSET(tcp_server->sockfd, &tcp_server->readfds))
	{

#ifdef _WIN32
	newsockfd = accept(tcp_server->sockfd, NULL, NULL);
		if (newsockfd == INVALID_SOCKET) {
			fprintf(stderr,"\naccept failed with error: %d", WSAGetLastError());
			//socketClose(newsockfd);
			//WSACleanup();
		   // return INVALID_SOCKET;
		}
#else

	struct addrinfo cli_addr;
	socklen_t clilen=sizeof(cli_addr);
	newsockfd = accept(tcp_server->sockfd,
					 (struct sockaddr *) &cli_addr,
					 &clilen);
	 if (newsockfd < 0) {
		  fprintf(stderr,"ERROR on accept\n");
		  return INVALID_SOCKET;
	 }
#endif
	}

	 return newsockfd;

}

bool TcpServer_SocketReady(TcpServer * tcp_server,SOCKET sock){

	if (FD_ISSET( sock , &tcp_server->readfds)){
		return true;
	}

	return false;
}

//--------------------------------------------------------------------
//  PUBLIC
TcpServer * TcpServer_New(TcpServerOnGestMessage on_gest_message)
{
	TcpServer * tcp_server = ZN_MALLOC(sizeof(TcpServer));

	tcp_server->src_port=0;
	tcp_server->dst_port=0;
	tcp_server->disconnect_request=false;
	tcp_server->on_gest_message=on_gest_message;


	tcp_server->initialized  =  false;


	for(int i=0; i < MAX_SOCKETS; i++){
		tcp_server->socket_client[i].idx_client=i;
		tcp_server->socket_client[i].socket=INVALID_SOCKET;
		tcp_server->socket_client[i].streaming_header_sent=false;
		tcp_server->free_socket[i]=i;
	}

	tcp_server->n_free_sockets=MAX_SOCKETS;

	tcp_server->message=NULL;

	tcp_server->connected  =  false;
	tcp_server->reconnection_request  =  false;
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
void TcpServer_SetTimeDelay(TcpServer * tcp_server,unsigned long delay){
	tcp_server->time_delay_ms = delay;
}

const char * TcpServer_GetErrorSockOpt(void){
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
bool  TcpServer_Setup(TcpServer * tcp_server,  int _portno)  //  Reads  configuration  of  machine  &  init  sdl_net...
{

	tcp_server->end_loop_mdb=false;

	tcp_server->portno = _portno;

	TcpServer_SetTimeout(tcp_server,DEFAULT_TIMEOUT_SECONDS);

	tcp_server->sockfd=TcpUtils_NewSocketServer(_portno);

	 if(pthread_create(&tcp_server->thread,NULL,TcpServer_Update,(void *)tcp_server)!=0){//mainLoop(this));
		 fprintf(stderr,"\nerror creating thread");
		 tcp_server->thread=0;
		 return false;
	 }

	return true;
}

bool TcpServer_Start(TcpServer * tcp_server,  int _portno){
	if(TcpServer_Setup(tcp_server, _portno)){
		TcpServer_Connect(tcp_server);
		return true;
	}
	return false;
}

//---------------------------------------------------------------
bool  TcpServer_IsConnected(TcpServer * tcp_server) {
	return  tcp_server->connected;
}
//--------------------------------------------------------------------
void TcpServer_Resume(TcpServer * tcp_server){
	TcpServer_Connect(tcp_server);
}
void TcpServer_Pause(TcpServer * tcp_server){
	TcpServer_Disconnect(tcp_server);
}

//--------------------------------------------------------------------
SocketClient * TcpServer_GetFreeSlot(TcpServer * tcp_server){

	SocketClient *cs=NULL;

	if(tcp_server->n_free_sockets > 0){

		if(tcp_server->free_socket[tcp_server->n_free_sockets-1] != SOCKET_CLIENT_NOT_AVAILABLE){
			cs = &tcp_server->socket_client[tcp_server->free_socket[tcp_server->n_free_sockets-1]];
			tcp_server->free_socket[tcp_server->n_free_sockets-1]=SOCKET_CLIENT_NOT_AVAILABLE;
			tcp_server->n_free_sockets--;
		}else{
			fprintf(stderr,"\ninternal error!");
			return NULL;
		}
	}
	else{
		fprintf(stderr,"\nno space left!!");
	}
	return cs;
}

bool TcpServer_CloseSocketClient(TcpServer * tcp_server,SocketClient *socket_client){

	if(tcp_server->n_free_sockets < MAX_SOCKETS){

		if(socket_client->socket != INVALID_SOCKET){

			//socketDel(clientSock->socket);
#ifdef _WIN32
			// in wsa we have to shutdown client socket connection by default recv and send operations
			int iResult = shutdown(socket_client->socket, SD_BOTH);
			if (iResult == SOCKET_ERROR) {
				fprintf(stderr,"\nshutdown failed with error: %d", WSAGetLastError());
			}
#endif

			// close socket client...
			TcpUtils_CloseSocket(&socket_client->socket);

			socket_client->socket=INVALID_SOCKET;
			socket_client->streaming_header_sent=false;
			tcp_server->free_socket[tcp_server->n_free_sockets]=socket_client->idx_client;
			tcp_server->n_free_sockets++;


			return true;
		}
		else{
			fprintf(stderr,"\nCannot ZN_FREE because -SOCKET IS NOT AVAILABLE-");
		}
	}
	else{
		fprintf(stderr,"\nCannot ZN_FREE because -MAX_SOCKETS REACHED-");
	}
	return false;
}

//---------------------------------------------------------------------------------------------------------------
bool TcpServer_GestServerBase(TcpServer * tcp_server)
{
	//int  numready=0;

	return true;
}

void TcpServer_GestServer(TcpServer * tcp_server)
{
	//gestServerBase();

	SOCKET new_socket = INVALID_SOCKET;


	// accept socket client (you're ready to get / send messages from/to this socket)...

	if((new_socket =TcpServer_SocketAccept(tcp_server)) != INVALID_SOCKET){

		SocketClient *socket_client= TcpServer_GetFreeSlot(tcp_server);

		if(socket_client==NULL){ // no space left ... reject client ...
			fprintf(stderr,"\n*** Maximum client count reached - rejecting client connection ***");
			TcpUtils_CloseSocket(&new_socket);
		}else{

#if __DEBUG__
			printf("\nAdding new client");
#endif

			socket_client->socket=new_socket;
		}
	}

	for (int cn = 0; cn < MAX_SOCKETS; cn++)  {
		// If the socket is ready (i.e. it has data we can read)... (SDLNet_SocketReady returns non-zero if there is activity on the socket, and zero if there is no activity)
		if(tcp_server->socket_client[cn].socket != INVALID_SOCKET){
			int client_socket_activity = TcpServer_SocketReady(tcp_server,tcp_server->socket_client[cn].socket);
#if __DEBUG__
		//	printf("Just checked client number %i  and received activity status is: %i\n", clientNumber,clientSocketActivity);
#endif
			// If there is any activity on the client socket...
			if (client_socket_activity != 0)
			{
				int ok=1;
				if(!tcp_server->is_streaming_server){ // read from client...
					ok=TcpUtils_ReceiveBytes(tcp_server->socket_client[cn].socket,  (uint8_t  *)tcp_server->buffer);
				}

				if(ok) // serve to client ...
				{
					TcpServerOnGestMessage cf=tcp_server->on_gest_message;
					if(!cf.callback_function(tcp_server,&tcp_server->socket_client[cn],tcp_server->buffer, sizeof(tcp_server->buffer),cf.user_data)){
#if __DEBUG__
						printf("\ngestMessage:Erasing client %i (gestMessage)",cn);
#endif
						TcpServer_CloseSocketClient(tcp_server,&tcp_server->socket_client[cn]);
					}
				}else{ // remove that socket because client closed the connection ...
#if __DEBUG__
					printf("\ngestMessage:Erasing client %i (getMessage)",cn);
#endif
					TcpServer_CloseSocketClient(tcp_server,&tcp_server->socket_client[cn]);
				}

			} // End of if client socket is active check
		}
	}// End of server socket check sockets loop
}
//------------------------------------------------------------------------------------------------
void  TcpServer_InternalDisconnect(TcpServer * tcp_server)
{
	if( tcp_server->connected)
	{
		tcp_server->connected  =  false;

		// remove all clients boot (only for TCP protocol)
		for(int i = 0; i < MAX_SOCKETS; i++){
			if(tcp_server->socket_client[i].socket!=INVALID_SOCKET){
				TcpServer_CloseSocketClient(tcp_server,&tcp_server->socket_client[i]);
			}
		}

		if(tcp_server->sockfd != INVALID_SOCKET){
			// close and ZN_FREE socket server...
			TcpUtils_CloseSocket(&tcp_server->sockfd);
		}

		printf("\nDisconnect server");

	}
}
//---------------------------------------------------------------------------------------------------------v
void  TcpServer_GetMessage(TcpServer * tcp_server)
{
	if(!tcp_server->connected)  return;

	TcpServer_GestServer(tcp_server);
}
//--------------------------------------------------------------------
void  TcpServer_Connect(TcpServer * tcp_server)
{
	tcp_server->reconnection_request  =  true;
}
//--------------------------------------------------------------------
void  TcpServer_Disconnect(TcpServer * tcp_server)
{
	tcp_server->disconnect_request  =  true;
}
//--------------------------------------------------------------------
void  * TcpServer_Update(void * varg)  //  Receive  messages,  gest  &  send...
{
	if(varg != NULL){
		TcpServer * tcp_server=(TcpServer *)varg;
		//printf("reco %i\n",tcp_server->end_loop_mdb);
		while(!tcp_server->end_loop_mdb)
		{
			if(tcp_server->reconnection_request){

				tcp_server->connected=true;
				tcp_server->reconnection_request=false;
			}
			else{
				TcpServer_GetMessage(tcp_server);  //  For  server  update  connections  &  get  messages  from  clients...
			}

			if(tcp_server->time_delay_ms>0){
	#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
			Sleep( tcp_server->time_delay_ms );
	#else
			usleep( tcp_server->time_delay_ms * 1000 );
	#endif
			}

		}
	}

	return 0;
}


//------------------------------------------------------------------------------------------------------------------------
//char  *str  =  NULL;

void TcpServer_Stop(TcpServer * tcp_server) {

	tcp_server->end_loop_mdb=true;
	pthread_join(tcp_server->thread,NULL);
	TcpServer_InternalDisconnect(tcp_server);

	if(tcp_server->sockfd!=INVALID_SOCKET){
		TcpUtils_CloseSocket(&tcp_server->sockfd);
	}

#ifdef _WIN32
	WSACleanup();
#endif


}

void TcpServer_Delete(TcpServer * tcp_server) {
	TcpServer_Stop(tcp_server);
	ZN_FREE(tcp_server);
}


