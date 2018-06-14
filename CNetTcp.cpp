#include "HttpServer.h"


const char * SERVER_FULL     = "FULL";

//  receive  a  buffer  from  a  TCP  socket  with  error  checking
//  this  function  handles  the  memory,  so  it  can't  use  any  []  arrays
//  returns  0  on  any  errors,  or  a  valid  char*  on  success
int  CNetTcp::getMsg(SOCKET  sock,  uint8_t  *buf)
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
int  CNetTcp::putMsg(SOCKET  sock,  uint8_t  *buf,  uint32_t  len) {
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


SOCKET CNetTcp::socketAccept(){

	SOCKET newsockfd = INVALID_SOCKET;
#ifdef _WIN32
	newsockfd = accept(sockfd, NULL, NULL);
	    if (newsockfd == INVALID_SOCKET) {
	        fprintf(stderr,"accept failed with error: %d\n", WSAGetLastError());
	        //socketClose(newsockfd);
	        //WSACleanup();
	       // return INVALID_SOCKET;
	    }
#else
	socklen_t clilen=sizeof(cli_addr);
	newsockfd = accept(sockfd,
	                 (struct sockaddr *) &cli_addr,
	                 &clilen);
	 if (newsockfd < 0) {
		  fprintf(stderr,"ERROR on accept\n");
		  return INVALID_SOCKET;
	 }
#endif

	 return newsockfd;

}

int CNetTcp::socketReady(SOCKET sock){

	unsigned long int count;
	ioctl(sock, FIONREAD, &count);

	return count;

}

//--------------------------------------------------------------------
//  PUBLIC
CNetTcp::CNetTcp()
{
	src_port=0;
	dst_port=0;
	ipaddr=0;
	host=NULL;//"127.0.0.1";
	RequestToDisConnect=false;
	timeout=1000;

	initialized  =  false;
	Want_reconnection = false;


	memset(clientSocket,0,sizeof(clientSocket));

	for(int i=0; i < MAX_CLIENTS; i++){
		clientSocket[i].idxClient=i;
		clientSocket[i].header_sent=false;
		freeSocket[i]=i;
	}

	n_freeSockets=MAX_CLIENTS;

	message=NULL;

	connected  =  false;
	RequestToConnect  =  false;
	configured = false;

	thread = NULL;
	end_loop_mdb=false;

	sockfd=-1;

	portno=-1;
}

void CNetTcp::mainLoop(CNetTcp *tcp){
	tcp->update();
}
//---------------------------------------------------------------------------------------------------------------------------
bool  CNetTcp::setup(  int _portno, const char *name_server)  //  Reads  configuration  of  machine  &  init  sdl_net...
{
	// kill thread if is active...
	host = name_server;

	unLoad();

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = _portno;

#ifdef _WIN32
		// Initialize Winsock
		struct addrinfo *result = NULL;
	    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	    if (iResult != 0) {
	        fprintf(stderr,"WSAStartup failed with error: %d\n", iResult);
	        return false;
	    }

	    serv_addr.ai_family = AF_INET;
	    serv_addr.ai_socktype = SOCK_STREAM;
	    serv_addr.ai_protocol = IPPROTO_TCP;
	    serv_addr.ai_flags = AI_PASSIVE;

	    // Resolve the server address and port
	   iResult = getaddrinfo(NULL, (const char *)CIO_Utils::intToString(portno).c_str(), &serv_addr, &result);
	   if ( iResult != 0 ) {
		   fprintf(stderr,"getaddrinfo failed with error: %d\n", iResult);
		   WSACleanup();
		   return false;
	   }

	   // Create a SOCKET for connecting to server
	   sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	      if (sockfd == INVALID_SOCKET) {
	          fprintf(stderr,"socket failed with error: %ld\n", WSAGetLastError());
	          freeaddrinfo(result);
	          WSACleanup();
	          return false;
	      }

	      // Setup the TCP listening socket
	       iResult = bind( sockfd, result->ai_addr, (int)result->ai_addrlen);
	       if (iResult == SOCKET_ERROR) {
	           fprintf(stderr,"bind failed with error: %d\n", WSAGetLastError());
	           freeaddrinfo(result);
	           closesocket(sockfd);
	           WSACleanup();
	           return false;
	       }

	      freeaddrinfo(result);


	 iResult = listen(sockfd, MAX_SOCKETS);
	    if (iResult == SOCKET_ERROR) {
           fprintf(stderr,"listen failed with error: %d\n", WSAGetLastError());
           closesocket(sockfd);
           WSACleanup();

	        return false;
	    }



#else // GNU

		 serv_addr.sin_family = AF_INET;
		 serv_addr.sin_addr.s_addr = INADDR_ANY;
		 serv_addr.sin_port = htons(_portno);

		 sockfd = socket(AF_INET, SOCK_STREAM, 0);
		 if (sockfd < 0){
			fprintf(stderr,"ERROR opening socket");
			return false;
		 }

		if (bind(sockfd, (struct sockaddr *) &serv_addr,
					  sizeof(serv_addr)) < 0){
					  fprintf(stderr,"ERROR on binding");
					  return false;
		}

		listen(sockfd,10); // block until new connection is established...
#endif

	timeout  =  5000;

	printf("Setup server  %s (%d.%d.%d.%d:%i)\n",
			host,
			ipaddr>>24,
			(ipaddr>>16)&0xff,
			(ipaddr>>8)&0xff,
			ipaddr&0xff,
			portno);


	thread = new std::thread(mainLoop,this);//mainLoop(this));

	return true;
}
//---------------------------------------------------------------
bool  CNetTcp::isConnected() {
	return  connected;
}
//--------------------------------------------------------------------
void CNetTcp::resume(){
	connect();
}
void CNetTcp::pause(){
	disconnect();
}

void CNetTcp::closeSocket(SOCKET sock){

	if(sock != INVALID_SOCKET){

#ifdef _WIN32
    // shutdown the connection since we're done

		if(sockfd != sock){ // is not my self (it could be, i.e server)...
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
tClientSocket * CNetTcp::getFreeSlot(){

	tClientSocket *cs=NULL;

	if(n_freeSockets > 0){

		if(freeSocket[n_freeSockets-1] != SOCKET_CLIENT_NOT_AVAILABLE){
			cs = &clientSocket[freeSocket[n_freeSockets-1]];
			freeSocket[n_freeSockets-1]=SOCKET_CLIENT_NOT_AVAILABLE;
			n_freeSockets--;
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

bool CNetTcp::freeSlot(tClientSocket *clientSock){

	if(n_freeSockets < MAX_CLIENTS){

		if(clientSock->socket != 0){

			freeSocket[n_freeSockets]=clientSock->idxClient;

			//socketDel(clientSock->socket);
			closeSocket(clientSock->socket);
			clientSock->socket = 0;
			clientSock->header_sent = false;


			n_freeSockets++;

			return true;
		}
		else{
			fprintf(stderr,"Cannot free because -SOCKET IS NOT AVAILABLE-\n");
		}
	}
	else{
		fprintf(stderr,"Cannot free because -MAX_CLIENTS REACHED-\n");
	}
	return false;
}

//---------------------------------------------------------------------------------------------------------------
bool CNetTcp::gestServerBase()
{
	int  numready=0;





		//socketAdd(c->socket);

	return true;
}

void CNetTcp::gestServer()
{
	//gestServerBase();
	tClientSocket *c = getFreeSlot();



	if(c==NULL){ // no space left ... reject client ...
		fprintf(stderr,"*** Maximum client count reached - rejecting client connection ***\n");
	}else{
	// accept socket client (you're ready to get / send messages from/to this socket)...
		c->socket =socketAccept();

#if __DEBUG__
	printf("Adding new client %i\n",c->socket);
#endif
	}


	for (int clientNumber = 0; clientNumber < MAX_CLIENTS; clientNumber++)  {
		// If the socket is ready (i.e. it has data we can read)... (SDLNet_SocketReady returns non-zero if there is activity on the socket, and zero if there is no activity)
		if(clientSocket[clientNumber].socket != INVALID_SOCKET){
			int clientSocketActivity = socketReady(clientSocket[clientNumber].socket);
			printf("Just checked client number %i  and received activity status is: %i\n", clientNumber,clientSocketActivity);
			// If there is any activity on the client socket...
			if (clientSocketActivity != 0)
			{
				if(getMsg(clientSocket[clientNumber].socket,  (uint8_t  *)buffer))  //  message read ...
				{
					if(!gestMessage(clientSocket[clientNumber].socket,buffer, sizeof(buffer))){
#if __DEBUG__
						printf("gestMessage:Erasing client %i (gestMessage)\n",clientNumber);
#endif
						freeSlot(&clientSocket[clientNumber]);
					}
				}else{ // remove that socket because there's no activity...
#if __DEBUG__
					printf("gestMessage:Erasing client %i (getMessage)\n",clientNumber);
#endif
					freeSlot(&clientSocket[clientNumber]);
				}

			} // End of if client socket is active check
		}
	}// End of server socket check sockets loop
}
//------------------------------------------------------------------------------------------------
void  CNetTcp::internal_disconnect()
{
	if( connected)
	{
		connected  =  false;


			// remove all clients boot (only for TCP protocol)
			for(int i = 0; i < MAX_CLIENTS; i++){
				if(clientSocket[i].socket!=INVALID_SOCKET){
					closeSocket(clientSocket[i].socket);
					clientSocket[i].socket=INVALID_SOCKET;
					clientSocket[i].header_sent=false;
				}
			}

			for(int i=0; i < MAX_CLIENTS; i++){
				freeSocket[i]=i;
			}

			n_freeSockets=MAX_CLIENTS;

			if(sockfd != 0){
				// close and free socket server...
				closeSocket(sockfd);
			}

			sockfd = 0;
			printf("Disconnect server!\n");

	}
}
//------------------------------------------------------------------------------------------------------------------------
//char  *str  =  NULL;
void CNetTcp::unLoad()
{
	if(thread != NULL){
		end_loop_mdb=true;
		thread->join();
		internal_disconnect();

		delete thread;
		thread=NULL;

        closeSocket(sockfd);

#ifdef _WIN32
        WSACleanup();
#endif
	}
}

CNetTcp::~CNetTcp() {
	unLoad();
}
//---------------------------------------------------------------------------------------------------------v
void  CNetTcp::getMessage()
{
	if(!connected)  return;

	gestServer();
}
//--------------------------------------------------------------------
void  CNetTcp::connect()
{
	RequestToConnect  =  true;
}
//--------------------------------------------------------------------
void  CNetTcp::disconnect()
{
	RequestToDisConnect  =  true;
}
//--------------------------------------------------------------------
void  CNetTcp::Reconnection() {
	Want_reconnection = true;
}
//--------------------------------------------------------------------
void  CNetTcp::update()  //  Receive  messages,  gest  &  send...
{
	while(!end_loop_mdb)
	{
		if(RequestToConnect){
			connected=true;
			RequestToConnect=false;
		}
		else{
			getMessage();  //  For  server  update  connections  &  get  messages  from  clients...
		}

		sleep(1);
	}

	//return 0;
}

//--------------------------------------------------------------------
void  CNetTcp::PrintStatus()
{
}
