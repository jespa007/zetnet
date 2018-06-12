#include "HttpServer.h"


const char * SERVER_FULL     = "FULL";






int  CNetTcp::getMsg(SOCKET sock, uint8_t  *buf){

	return TCP_getMsg(sock, buf);

}

int  CNetTcp::putMsg(SOCKET sock,uint8_t  *buf,  uint32_t  len){

		return TCP_putMsg(sock, buf,len);

}

/*
void CNetTcp::closeSocket(SOCKET sock){
#ifdef _WIN32
		closeSocket(sock);
#else
		close(sock);
#endif
		//SDLNet_TCP_Close((TCPsocket)sock);

}*/
/*
int CNetTcp::socketAdd(void *sock){



		return SDLNet_TCP_AddSocket(socketSet, (TCPsocket)sock);

}
*/
/*
int CNetTcp::socketDel(void *sock){

		return SDLNet_TCP_DelSocket(socketSet, (TCPsocket)sock);

}*/

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
	socklen_t clilen;
	newsockfd = accept(sockfd,
	                 (struct sockaddr *) &cli_addr,
	                 &clilen);
	     if (newsockfd < 0) {
	          fprintf(stderr,"ERROR on accept\n");
	          //return 0;
	     }
#endif

return newsockfd;

		//return SDLNet_TCP_Accept((TCPsocket)sock);

}

int CNetTcp::socketReady(SOCKET sock){

	unsigned long int count;
	ioctl(sock, FIONREAD, &count);

	return count;

	//return SDLNet_SocketReady((SDLNet_GenericSocket)sock);
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
	//n_clients  =  0;
	//socketSet  =  NULL;
	//socket  = NULL;

	connected  =  false;
	RequestToConnect  =  false;
	configured = false;


	//TimeToReconnect = 0;
	//TimerPolling = 0;
	//TimerActivityNet=0;

	thread = NULL;
	end_loop_mdb=false;

	sockfd=-1;

	portno=-1;
}

void CNetTcp::mainLoop(CNetTcp *tcp){
	tcp->update();
}
/*
bool CNetTcp::setupHost()
{

}*/
//---------------------------------------------------------------------------------------------------------------------------
/*bool  CNetTcp::internal_connect()
{


	if(TCP_GetConnection()){

		//socketAdd(socket);

		return true;
	}

	return false;
}*/

//---------------------------------------------------------------------------------------------------------------------------
bool  CNetTcp::setup(  int _portno, const char *name_server)  //  Reads  configuration  of  machine  &  init  sdl_net...
{


	/*src_port = _src_port;
	dst_port = _dst_port;
	ValueVariableHost = "0.0.0.0";


	// init common variables ...
	if(!createSocketSet()){
		return;
	}

	// setupHost();
	setupHost();

	configured = true;*/

	// kill thread if is active...
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

	        /*  iResult = listen(sockfd, SOMAXCONN);
	          if (iResult == SOCKET_ERROR) {
	              fprintf(stderr,"listen failed with error: %d\n", WSAGetLastError());
	              closeSocket(sockfd);
	              WSACleanup();
	              return false;
	          }*/


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
#endif







	timeout  =  5000;

	printf("Setup server  %s (%d.%d.%d.%d:%i)\n",
			host,
			ipaddr>>24,
			(ipaddr>>16)&0xff,
			(ipaddr>>8)&0xff,
			ipaddr&0xff,
			portno);




	//CThread::start();
	thread = new std::thread(mainLoop,this);//mainLoop(this));

	return true;
}
//---------------------------------------------------------------
/*void CNetTcp::WaitToDisconnect() {
	while(connected)
	{
		printf("Wait to disconnect!\n");
	}
}*/
//---------------------------------------------------------------
bool  CNetTcp::isConnected() {
	return  connected;
}
//--------------------------------------------------------------------
/*bool  		CNetTcp::sendMessageToServer(uint8_t  *data,  uint32_t  len) {
	int len_sended;

	if(socket == NULL) {
		fprintf(stderr,"Cannot send message because, client not connected to server!\n");
		return false;
	}

	len_sended  =  putMsg(socket,data,len);

	if(len_sended  !=  (int)len) {
		fprintf(stderr,"Error  to  to  send  message  (expected %i  bytes and %i was sent)!\n",len,len_sended);
		return  false;
	}

	return true;
}*/

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
//  create  a  socket  socketSet  that  has  the  server  socket  and  all  the  client  sockets
/*bool CNetTcp::createSocketSet()
{
	if(socketSet!=NULL)  //  free-it  ?
	{
		fprintf(stderr,"Socket socketSet already created!\n");
		return false;
	}
	// Allocate for server + clients...
	socketSet=SDLNet_AllocSocketSet(MAX_SOCKETS);

	if(socketSet==NULL)
	{
		fprintf(stderr,"SDLNet_AllocSocketSet:  %s\n",  SDLNet_GetError());
		return  false;  //most  of  the  time  this  is  a  major  error,  but  do  what  you  want.
	}
	return true;
}*/
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
	//char buffer[100];
	int  numready=0;

	/*if(socketSet == NULL){
		return false;
	}*/

	//numready=SDLNet_CheckSockets(socketSet,timeout);
#ifdef _WIN32
	int iResult = listen(sockfd, MAX_SOCKETS);
	    if (iResult == SOCKET_ERROR) {
	        fprintf(stderr,"listen failed with error: %d\n", WSAGetLastError());
	        //closeSocket(socketfd);
	        //WSACleanup();
	        return false;
	    }
#else
	listen(sockfd,MAX_SOCKETS);
#endif

	/*if(numready==-1) {
		fprintf(stderr,"SDLNet_CheckSockets:  %s\n",SDLNet_GetError());
		return false;
	}*/
	//if(numready > 0) {
	//if(socketReady(socket))
	{
		tClientSocket *c = getFreeSlot();

#if __DEBUG__
		printf("Adding new client\n");
#endif

		if(c==NULL){ // no space left ... reject client ...
			fprintf(stderr,"*** Maximum client count reached - rejecting client connection ***\n");
			// Accept the client connection to clear it from the incoming connections list
			//void * tempSock = socketAccept(socket);

			//putMsg(tempSock, (Uint8 *)SERVER_FULL, strlen(SERVER_FULL) + 1);

			// Shutdown, disconnect, and close the socket to the client
			//socketClose(tempSock);
			return false;
		}
		// accept socket client (you're ready to get / send messages from/to this socket)...
		c->socket =socketAccept();
		//socketAdd(c->socket);
	}
	return true;
}

void CNetTcp::gestServer()
{
	if(!gestServerBase())
	{
		return;
	}

	for (int clientNumber = 0; clientNumber < MAX_CLIENTS; clientNumber++)  {
		// If the socket is ready (i.e. it has data we can read)... (SDLNet_SocketReady returns non-zero if there is activity on the socket, and zero if there is no activity)
		if(clientSocket[clientNumber].socket != 0){
			int clientSocketActivity = socketReady(clientSocket[clientNumber].socket);
			//cout << "Just checked client number " << clientNumber << " and received activity status is: " << clientSocketActivity << endl;
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
				if(clientSocket[i].socket!=0){
					closeSocket(clientSocket[i].socket);
					clientSocket[i].socket=0;
					clientSocket[i].header_sent=false;
				}
			}

			for(int i=0; i < MAX_CLIENTS; i++){
				freeSocket[i]=i;
			}

			n_freeSockets=MAX_CLIENTS;

			if(sockfd != 0){
				// close and free socket server...
				//socketDel(socket);
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
		//disconnect();
		//WaitToDisconnect();
		end_loop_mdb=true;
		thread->join();
		//CThread::stop();
		internal_disconnect();

		delete thread;
		thread=NULL;

        closeSocket(sockfd);
        WSACleanup();
	}
	//SDL_Delay(500);
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

		//TimerPolling = SDL_GetTicks()+1000;

		/*if(RequestToConnect)// && !Want_reconnection)
		{
			if(!connected)
			{
				if(TimeToReconnect < SDL_GetTicks())
				{
					if(!internal_connect())
					{
						TimeToReconnect = SDL_GetTicks() + TIME_TO_RECONNECT;
						fprintf(stderr,"  Not  connected!  -->  try  connection  to  3s\n");
					}
					else{
						RequestToConnect=false;
					}
				}
				SDL_Delay(100);
			}
			//  else  is  connected...
		}
		else
		{
			if(RequestToDisConnect) //Want_reconnection)
			{
				RequestToDisConnect = false;
				internal_disconnect();
			}
		}*/
		getMessage();  //  For  server  update  connections  &  get  messages  from  clients...

	    //}
	}

	//return 0;
}


//------------------------------------------------------------------------------------------------------------
/*void  CNetTcp::TCP_GetIpAddressFromSocket(TCPsocket  sock,  char  *buffer)
{
	Uint32  IPAddress;

	if(sock)
	{
		IPaddress  *remoteIP  =  SDLNet_TCP_GetPeerAddress(sock);

		IPAddress=SDL_SwapBE32(remoteIP->host);

		//  output  the  IP  address  nicely
		sprintf(buffer,"%d.%d.%d.%d",
				(IPAddress)>>24,
				((IPAddress)>>16)&0xff,
				((IPAddress)>>8)&0xff,
				(IPAddress)&0xff);
	}
}
*/
//---------------------------------------------------------------------------------------------------------------------------------------

//  receive  a  buffer  from  a  TCP  socket  with  error  checking
//  this  function  handles  the  memory,  so  it  can't  use  any  []  arrays
//  returns  0  on  any  errors,  or  a  valid  char*  on  success
int  CNetTcp::TCP_getMsg(SOCKET  sock,  uint8_t  *buf)
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
int  CNetTcp::TCP_putMsg(SOCKET  sock,  uint8_t  *buf,  uint32_t  len) {
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
//---------------------------------------------------------------------------------------------------------------------------------------
/*bool  CNetTcp::TCP_GetConnection() // Try connect to server...
{
	if(connected)   {
		return  true;
	}
	// Disconnect from net....
	internal_disconnect();

	socket=SDLNet_TCP_Open(&ip);
	if(!socket) {
		fprintf(stderr,"%s %d.%d.%d.%d:%i : %s\n",
			host,
			ipaddr>>24,
			(ipaddr>>16)&0xff,
			(ipaddr>>8)&0xff,
			ipaddr&0xff,
			dst_port,
			SDLNet_GetError());
		return  false;
	}
	connected = true;

	printf("Established connection the TCP socket at port %i\n", dst_port);

	return  connected;
}*/

//---------------------------------------------------------------------------------------------------------------------------------------
/*void  CNetTcp::TCP_GestClient()
{
	int  numready=0;

	if(socketSet == NULL) return;

	numready=SDLNet_CheckSockets(socketSet,  timeout);

	if(numready==-1)  //  Not  connected!
	{
		printf("SDLNet_CheckSockets:  %s\n",SDLNet_GetError());
		connected  =  false;
		return;
	}

	//  check  to  see  if  the  server  sent  us  data
	if(SDLNet_SocketReady(socket)  &&  numready) // recived messaje from server...
	{
		//print_info_cr("Incoming server msg! %i %i",type_protocol,CNetTcp::CNET_TCP_PROTOCOL);
		//  if  numready  is  >  0  (Server  send  data!)...
		//  getMsg  is  in  tcputil.h,  it  gets  a  CString  from  the  socket
		//  with  a  bunch  of  error  handling
		if(!TCP_getMsg((TCPsocket)socket,(Uint8  *)buffer))
		{

		}

	}
}*/

//--------------------------------------------------------------------
void  CNetTcp::PrintStatus()
{
}
