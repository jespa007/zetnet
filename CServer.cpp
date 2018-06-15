#include "HttpServer.h"


const char * SERVER_FULL     = "FULL";

//  receive  a  buffer  from  a  TCP  socket  with  error  checking
//  this  function  handles  the  memory,  so  it  can't  use  any  []  arrays
//  returns  0  on  any  errors,  or  a  valid  char*  on  success
int  CServer::getMsg(SOCKET  sock,  uint8_t  *buf)
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
int  CServer::putMsg(SOCKET  sock,  uint8_t  *buf,  uint32_t  len) {
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


void CServer::setTimeout(unsigned int seconds){
	timeout = {seconds, 0};   // sleep for ten minutes!
}

SOCKET CServer::socketAccept(){

	SOCKET newsockfd = INVALID_SOCKET;

    //clear the socket set
    FD_ZERO(&readfds);

    //add master socket to set
    FD_SET(sockfd, &readfds);
    SOCKET max_sd = sockfd;

    //add child sockets to set
    for (int i = 0 ; i < MAX_SOCKETS ; i++)
    {
        //socket descriptor
        SOCKET sd = clientSocket[i].socket;

        //if valid socket descriptor then add to read list
        if(sd > 0){
            FD_SET( sd , &readfds);
        }

        //highest file descriptor number, need it for the select function
        if(sd > max_sd)
            max_sd = sd;
    }

    //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely

    int activity = select( max_sd + 1 , &readfds , NULL , NULL , &timeout);

    if ((activity < 0) && (errno!=EINTR))
    {
        fprintf(stderr,"select error");
    }

    //If something happened on the socket server, then its an incoming connection
    if (FD_ISSET(sockfd, &readfds))
    {



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
    }

	 return newsockfd;

}

bool CServer::socketReady(SOCKET sock){

	if (FD_ISSET( sock , &readfds)){
		return true;
	}

	return false;
}

//--------------------------------------------------------------------
//  PUBLIC
CServer::CServer()
{
	src_port=0;
	dst_port=0;
	ipaddr=0;
	host=NULL;//"127.0.0.1";
	RequestToDisConnect=false;


	initialized  =  false;
	Want_reconnection = false;




	for(int i=0; i < MAX_SOCKETS; i++){
		clientSocket[i].idxClient=i;
		clientSocket[i].socket=INVALID_SOCKET;
		clientSocket[i].header_sent=false;
		freeSocket[i]=i;
	}

	n_freeSockets=MAX_SOCKETS;

	message=NULL;

	connected  =  false;
	RequestToConnect  =  false;
	configured = false;

	thread = NULL;
	end_loop_mdb=false;

	sockfd=-1;

	portno=-1;
}

void CServer::mainLoop(CServer *tcp){
	tcp->update();
}
//---------------------------------------------------------------------------------------------------------------------------
bool  CServer::setup(  int _portno, const char *server_name)  //  Reads  configuration  of  machine  &  init  sdl_net...
{
	int opt = TRUE;
	// kill thread if is active...
	host = server_name;

	unLoad();

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = _portno;

	setTimeout(DEFAULT_TIMEOUT_SECONDS);

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



		 // create socket for server...
		 sockfd = socket(AF_INET, SOCK_STREAM, 0);
		 if (sockfd < 0){
			fprintf(stderr,"ERROR opening socket");
			return false;
		 }

		 //set server socket to allow multiple connections , this is just a good habit, it will work without this
		 if( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
		 {
			 fprintf(stderr,"setsockopt");
			 return false;
		 }

		 // setup parameters for server binding ...
		 serv_addr.sin_family = AF_INET;
		 serv_addr.sin_addr.s_addr = INADDR_ANY;
		 serv_addr.sin_port = htons(_portno);

		if (bind(sockfd, (struct sockaddr *) &serv_addr,
					  sizeof(serv_addr)) < 0){
					  fprintf(stderr,"ERROR on binding");
					  return false;
		}

		listen(sockfd,5); // block until new connection is established...
#endif



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
bool  CServer::isConnected() {
	return  connected;
}
//--------------------------------------------------------------------
void CServer::resume(){
	connect();
}
void CServer::pause(){
	disconnect();
}

void CServer::closeSocket(SOCKET sock){

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
tClientSocket * CServer::getFreeSlot(){

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

bool CServer::freeSlot(tClientSocket *clientSock){

	if(n_freeSockets < MAX_SOCKETS){

		if(clientSock->socket != INVALID_SOCKET){

			freeSocket[n_freeSockets]=clientSock->idxClient;

			//socketDel(clientSock->socket);
			closeSocket(clientSock->socket);
			clientSock->socket = INVALID_SOCKET;
			clientSock->header_sent = false;


			n_freeSockets++;

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
bool CServer::gestServerBase()
{
	int  numready=0;





		//socketAdd(c->socket);

	return true;
}

void CServer::gestServer()
{


	//gestServerBase();

	SOCKET new_socket = INVALID_SOCKET;


	// accept socket client (you're ready to get / send messages from/to this socket)...

	if((new_socket =socketAccept()) != INVALID_SOCKET){

		tClientSocket *c = getFreeSlot();

		if(c==NULL){ // no space left ... reject client ...
			fprintf(stderr,"*** Maximum client count reached - rejecting client connection ***\n");
			closeSocket(new_socket);
		}else{

#if __DEBUG__
	printf("Adding new client %i\n",new_socket);
#endif

			c->socket=new_socket;
		}
	}



	for (int clientNumber = 0; clientNumber < MAX_SOCKETS; clientNumber++)  {
		// If the socket is ready (i.e. it has data we can read)... (SDLNet_SocketReady returns non-zero if there is activity on the socket, and zero if there is no activity)
		if(clientSocket[clientNumber].socket != INVALID_SOCKET){
			int clientSocketActivity = socketReady(clientSocket[clientNumber].socket);
#if __DEBUG__
			printf("Just checked client number %i  and received activity status is: %i\n", clientNumber,clientSocketActivity);
#endif
			// If there is any activity on the client socket...
			if (clientSocketActivity != 0)
			{
				if(getMsg(clientSocket[clientNumber].socket,  (uint8_t  *)buffer))  //  message readed ...
				{
					if(!gestMessage(clientSocket[clientNumber].socket,buffer, sizeof(buffer))){
#if __DEBUG__
						printf("gestMessage:Erasing client %i (gestMessage)\n",clientNumber);
#endif
						freeSlot(&clientSocket[clientNumber]);
					}
				}else{ // remove that socket because client closed the connection ...
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
void  CServer::internal_disconnect()
{
	if( connected)
	{
		connected  =  false;


			// remove all clients boot (only for TCP protocol)
			for(int i = 0; i < MAX_SOCKETS; i++){
				if(clientSocket[i].socket!=INVALID_SOCKET){
					closeSocket(clientSocket[i].socket);
					clientSocket[i].socket=INVALID_SOCKET;
					clientSocket[i].header_sent=false;
				}
			}

			for(int i=0; i < MAX_SOCKETS; i++){
				freeSocket[i]=i;
			}

			n_freeSockets=MAX_SOCKETS;

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
void CServer::unLoad()
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

CServer::~CServer() {
	unLoad();
}
//---------------------------------------------------------------------------------------------------------v
void  CServer::getMessage()
{
	if(!connected)  return;

	gestServer();
}
//--------------------------------------------------------------------
void  CServer::connect()
{
	RequestToConnect  =  true;
}
//--------------------------------------------------------------------
void  CServer::disconnect()
{
	RequestToDisConnect  =  true;
}
//--------------------------------------------------------------------
void  CServer::Reconnection() {
	Want_reconnection = true;
}
//--------------------------------------------------------------------
void  CServer::update()  //  Receive  messages,  gest  &  send...
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

		usleep(20000); // 10ms
	}

	//return 0;
}

//--------------------------------------------------------------------
void  CServer::PrintStatus()
{
}
