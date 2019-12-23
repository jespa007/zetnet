#include "zetnet.h"


#define  MAX_BYTES_TO_SEND	512


#define  TIME_TO_RECONNECT	3000	//  ms

#define  MAX_TRIES_BAD_MESSAGE	3
//#define  TIME_POLLIGN		1000
#define  TIME_WAIT_ACK		3000	//  ms


//#define  MAX_CLIENTS					MAX_SOCKETS-1  // -1 because socket server is included.

#define DEFAULT_TIMEOUT_SECONDS 0
#define SOCKET_CLIENT_NOT_AVAILABLE -1


void  * TcpServer_Update(void * varg);

const char * SERVER_FULL     = "FULL";

//  receive  a  buffer  from  a  TCP  socket  with  error  checking
//  this  function  handles  the  memory,  so  it  can't  use  any  []  arrays
//  returns  0  on  any  errors,  or  a  valid  char*  on  success
int  TcpServer_ReceiveBytes(SOCKET  sock,  uint8_t  *buf)
{
//#define MAXLEN 1024
	int result;
	//char msg[MAXLEN];

	result = recv(sock,(char *)buf,MAX_LENGTH_MESSAGE,0);

	if(result <= 0) {
		// TCP Connection is broken. (because of error or closure)
		return 0;
	}
	else {
		if(result < (MAX_LENGTH_MESSAGE-1)){
			buf[result] = 0;

		}
		else {
			fprintf(stderr,"Max message reached %i<%i!\n",result,MAX_LENGTH_MESSAGE);
			return 0;
		}
	}


	return  result;
}

//  send  a  CString  buffer  over  a  TCP  socket  with  error  checking
//  returns  0  on  any  errors,  length  sent  on  success
int  TcpServer_SendBytes(SOCKET  sock,  uint8_t  *buf,  uint32_t  len) {
	uint32_t  result=0;

	if(!len) {
		fprintf(stderr,"0 bytes to send or buffer is NULL!\n");
		return 0;
	}

	//  send  the  buffer,  with  the  NULL  as  well
	result=send(sock,(const char *)buf,len,0);

	if(result<len) {
		fprintf(stderr,"TCP_putMsg (%i<%i)\n",result,len);
		return(0);
	}
	return(result);
}


void TcpServer_SetTimeout(TcpServer * tcp_server,int seconds){
	tcp_server->timeout = (struct timeval){
		.tv_sec=seconds
		,.tv_usec=0};   // sleep for ten minutes!
}

bool TcpServer_GestMessageDefault(TcpServer * tcp_server,SOCKET in_socket, uint8_t *buffer, uint32_t len, void *user_data){
	return false;
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
		if(tcp_server->clientSocket[i].socket!=INVALID_SOCKET){
			//socket descriptor
			SOCKET sd = tcp_server->clientSocket[i].socket;

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
			fprintf(stderr,"accept failed with error: %d\n", WSAGetLastError());
			//socketClose(newsockfd);
			//WSACleanup();
		   // return INVALID_SOCKET;
		}
#else



	socklen_t clilen=sizeof(tcp_server->cli_addr);
	newsockfd = accept(tcp_server->sockfd,
					 (struct sockaddr *) &tcp_server->cli_addr,
					 &tcp_server->clilen);
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
TcpServer * TcpServer_New()
{
	TcpServer * tcp_server = malloc(sizeof(TcpServer));
	memset(tcp_server,0,sizeof(TcpServer));
	tcp_server->src_port=0;
	tcp_server->dst_port=0;
	tcp_server->ipaddr=0;
	tcp_server->disconnect_request=false;


	tcp_server->initialized  =  false;


	for(int i=0; i < MAX_SOCKETS; i++){
		tcp_server->clientSocket[i].idxClient=i;
		tcp_server->clientSocket[i].socket=INVALID_SOCKET;
		tcp_server->clientSocket[i].header_sent=false;
		tcp_server->freeSocket[i]=i;
	}

	tcp_server->n_freeSockets=MAX_SOCKETS;

	tcp_server->message=NULL;

	tcp_server->connected  =  false;
	tcp_server->reconnection_request  =  false;
	tcp_server->configured = false;

	tcp_server->thread = -1;
	tcp_server->end_loop_mdb=false;

	tcp_server->sockfd=-1;
	tcp_server->portno=-1;
	tcp_server->time_delay_ms=10; // 10ms delay

	tcp_server->is_streaming_server=false;

	tcp_server->tcp_server_on_gest_message=(TcpServerOnGestMessage){
		.callback_function=TcpServer_GestMessageDefault
		,.user_data=NULL
	};

	return tcp_server;
}

//-------------------------------------------------------------------------------------
void TcpServer_SetTimeDelay(TcpServer * tcp_server,unsigned long delay){
	tcp_server->time_delay_ms = delay;
}

const char * TcpServer_GetErrorSockOpt(){
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
	bzero((char *) &tcp_server->serv_addr, sizeof(tcp_server->serv_addr));
	tcp_server->portno = _portno;

	TcpServer_SetTimeout(tcp_server,DEFAULT_TIMEOUT_SECONDS);

#ifdef _WIN32
		// Initialize Winsock
		struct addrinfo *result = NULL;
		int iResult = WSAStartup(MAKEWORD(2,2), &tcp_server->wsaData);
		if (iResult != 0) {
			fprintf(stderr,"WSAStartup failed with error: %d\n", iResult);
			return false;
		}

		tcp_server->serv_addr.ai_family = AF_INET;
		tcp_server->serv_addr.ai_socktype = SOCK_STREAM;
		tcp_server->serv_addr.ai_protocol = IPPROTO_TCP;
		tcp_server->serv_addr.ai_flags = AI_PASSIVE;

		// Resolve the server address and port
	   iResult = getaddrinfo(NULL, (const char *)ZNString_IntToString(_portno), &tcp_server->serv_addr, &result);
	   if ( iResult != 0 ) {
		   fprintf(stderr,"getaddrinfo failed with error: %d\n", iResult);
		   WSACleanup();
		   return false;
	   }

	   // Create a SOCKET for connecting to server
	   tcp_server->sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		  if (tcp_server->sockfd == INVALID_SOCKET) {
			  fprintf(stderr,"socket failed with error: %i\n", WSAGetLastError());
			  freeaddrinfo(result);
			  WSACleanup();
			  return false;
		  }

		  // Setup the TCP listening socket
		   iResult = bind( tcp_server->sockfd, result->ai_addr, (int)result->ai_addrlen);
		   if (iResult == SOCKET_ERROR) {
			   fprintf(stderr,"bind failed with error: %i\n", WSAGetLastError());
			   freeaddrinfo(result);
			   closesocket(tcp_server->sockfd);
			   WSACleanup();
			   return false;
		   }

		  freeaddrinfo(result);


	 iResult = listen(tcp_server->sockfd, MAX_SOCKETS);
		if (iResult == SOCKET_ERROR) {
		   fprintf(stderr,"listen failed with error: %d\n", WSAGetLastError());
		   closesocket(tcp_server->sockfd);
		   WSACleanup();

			return false;
		}



#else // GNU



		 // create socket for server...
		tcp_server->sockfd = socket(AF_INET, SOCK_STREAM, 0);
		 if (sockfd < 0){
			fprintf(stderr,"ERROR opening socket");
			return false;
		 }

		 //set server socket to allow multiple connections , this is just a good habit, it will work without this
		 if(( setsockopt(tcp_server->sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&tcp_server->opt, sizeof(tcp_server->opt))) < 0 )
		 {
			 fprintf(stderr,"setsockopt:%s",getErrorSockOpt());
			 return false;
		 }

		 // setup parameters for server binding ...
		 tcp_server->serv_addr.sin_family = AF_INET;
		 tcp_server->serv_addr.sin_addr.s_addr = INADDR_ANY;
		 tcp_server->serv_addr.sin_port = htons(_portno);

		if (bind(tcp_server->sockfd, (struct sockaddr *) &tcp_server->serv_addr,
					  sizeof(tcp_server->serv_addr)) < 0){
					  fprintf(stderr,"ERROR on binding");
					  return false;
		}

		listen(tcp_server->sockfd,5); // block until new connection is established...
#endif



	printf("Setup server  (%d.%d.%d.%d:%i)\n",
			tcp_server->ipaddr>>24,
			(tcp_server->ipaddr>>16)&0xff,
			(tcp_server->ipaddr>>8)&0xff,
			tcp_server->ipaddr&0xff,
			tcp_server->portno);


	 if(pthread_create(&tcp_server->thread,NULL,TcpServer_Update,(void *)tcp_server)!=0){//mainLoop(this));
		 fprintf(stderr,"error creating thread\n");
		 tcp_server->thread=-1;
		 return false;
	 }

	return true;
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

void TcpServer_CloseSocket(TcpServer * tcp_server,SOCKET sock){

	if(sock != INVALID_SOCKET){

#ifdef _WIN32
	// shutdown the connection since we're done

		if(tcp_server->sockfd != sock){ // is not my self (it could be, i.e server)...
			int iResult = shutdown(sock, SD_SEND);
			if (iResult == SOCKET_ERROR) {
				fprintf(stderr,"shutdown failed with error: %d\n", WSAGetLastError());
			}
		}

	closesocket(sock);
#else
	close(sock);
#endif

	}

}
//--------------------------------------------------------------------
ClientSocket * TcpServer_GetFreeSlot(TcpServer * tcp_server){

	ClientSocket *cs=NULL;

	if(tcp_server->n_freeSockets > 0){

		if(tcp_server->freeSocket[tcp_server->n_freeSockets-1] != SOCKET_CLIENT_NOT_AVAILABLE){
			cs = &tcp_server->clientSocket[tcp_server->freeSocket[tcp_server->n_freeSockets-1]];
			tcp_server->freeSocket[tcp_server->n_freeSockets-1]=SOCKET_CLIENT_NOT_AVAILABLE;
			tcp_server->n_freeSockets--;
		}else{
			fprintf(stderr,"internal error!\n");
			return NULL;
		}
	}
	else{
		fprintf(stderr,"no space left!!\n");
	}
	return cs;
}

bool TcpServer_FreeSlot(TcpServer * tcp_server,ClientSocket *clientSock){

	if(tcp_server->n_freeSockets < MAX_SOCKETS){

		if(clientSock->socket != INVALID_SOCKET){

			tcp_server->freeSocket[tcp_server->n_freeSockets]=clientSock->idxClient;

			//socketDel(clientSock->socket);
			TcpServer_CloseSocket(tcp_server,clientSock->socket);
			clientSock->socket = INVALID_SOCKET;
			clientSock->header_sent = false;


			tcp_server->n_freeSockets++;

			return true;
		}
		else{
			fprintf(stderr,"Cannot free because -SOCKET IS NOT AVAILABLE-\n");
		}
	}
	else{
		fprintf(stderr,"Cannot free because -MAX_SOCKETS REACHED-\n");
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

		ClientSocket *c = TcpServer_GetFreeSlot(tcp_server);

		if(c==NULL){ // no space left ... reject client ...
			fprintf(stderr,"*** Maximum client count reached - rejecting client connection ***\n");
			TcpServer_CloseSocket(tcp_server,new_socket);
		}else{

#if __DEBUG__
	printf("Adding new client %i\n",new_socket);
#endif

			c->socket=new_socket;
		}
	}



	for (int clientNumber = 0; clientNumber < MAX_SOCKETS; clientNumber++)  {
		// If the socket is ready (i.e. it has data we can read)... (SDLNet_SocketReady returns non-zero if there is activity on the socket, and zero if there is no activity)
		if(tcp_server->clientSocket[clientNumber].socket != INVALID_SOCKET){
			int clientSocketActivity = TcpServer_SocketReady(tcp_server,tcp_server->clientSocket[clientNumber].socket);
#if __DEBUG__
		//	printf("Just checked client number %i  and received activity status is: %i\n", clientNumber,clientSocketActivity);
#endif
			// If there is any activity on the client socket...
			if (clientSocketActivity != 0)
			{
				int ok=1;
				if(!tcp_server->is_streaming_server){ // read from client...
					ok=TcpServer_ReceiveBytes(tcp_server->clientSocket[clientNumber].socket,  (uint8_t  *)tcp_server->buffer);
				}

				if(ok) // serve to client ...
				{
					TcpServerOnGestMessage cf=tcp_server->tcp_server_on_gest_message;
					if(!cf.callback_function(tcp_server,tcp_server->clientSocket[clientNumber].socket,tcp_server->buffer, sizeof(tcp_server->buffer),cf.user_data)){
#if __DEBUG__
						printf("gestMessage:Erasing client %i (gestMessage)\n",clientNumber);
#endif
						TcpServer_FreeSlot(tcp_server,&tcp_server->clientSocket[clientNumber]);
					}
				}else{ // remove that socket because client closed the connection ...
#if __DEBUG__
					printf("gestMessage:Erasing client %i (getMessage)\n",clientNumber);
#endif
					TcpServer_FreeSlot(tcp_server,&tcp_server->clientSocket[clientNumber]);
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
			if(tcp_server->clientSocket[i].socket!=INVALID_SOCKET){
				TcpServer_CloseSocket(tcp_server,tcp_server->clientSocket[i].socket);
				tcp_server->clientSocket[i].socket=INVALID_SOCKET;
				tcp_server->clientSocket[i].header_sent=false;
			}
		}

		for(int i=0; i < MAX_SOCKETS; i++){
			tcp_server->freeSocket[i]=i;
		}

		tcp_server->n_freeSockets=MAX_SOCKETS;

		if(tcp_server->sockfd != 0){
			// close and free socket server...
			TcpServer_CloseSocket(tcp_server,tcp_server->sockfd);
		}

		tcp_server->sockfd = 0;
		printf("Disconnect server!\n");

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

void TcpServer_Delete(TcpServer * tcp_server) {
	if(tcp_server!=NULL){

		tcp_server->end_loop_mdb=true;
		pthread_join(tcp_server->thread,NULL);
		TcpServer_InternalDisconnect(tcp_server);


		TcpServer_CloseSocket(tcp_server,tcp_server->sockfd);

	#ifdef _WIN32
		WSACleanup();
	#endif

		free(tcp_server);
	}
}


