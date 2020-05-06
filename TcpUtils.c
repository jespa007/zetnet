#include "zetnet.h"


SOCKET TcpUtils_NewSocketClient(int _portno){
	SOCKET socket_client=INVALID_SOCKET;
	struct addrinfo *result = NULL,
	                    *ptr = NULL,
						serv_addr;
	bzero((char *) &serv_addr, sizeof(serv_addr));
#ifdef _WIN32

	serv_addr.ai_family = AF_UNSPEC;
	serv_addr.ai_socktype = SOCK_STREAM;
	serv_addr.ai_protocol = IPPROTO_TCP;


	// Resolve the server address and port
	int iResult = getaddrinfo("127.0.0.1", (const char *)ZNString_IntToString(_portno), &serv_addr, &result);
	if ( iResult != 0 ) {
		fprintf(stderr,"getaddrinfo failed with error: %d\n", iResult);
		return INVALID_SOCKET;
	}

	 // Attempt to connect to an address until one succeeds
	for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

		// Create a SOCKET for connecting to server
		socket_client = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (socket_client == INVALID_SOCKET) {
			fprintf(stderr,"socket failed with error: %ld\n", (long int)WSAGetLastError());
			return INVALID_SOCKET;
		}

		// Connect to server.
		iResult = connect( socket_client, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(socket_client);
			socket_client = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (socket_client == INVALID_SOCKET) {
		fprintf(stderr,"Unable to connect to server!\n");
	}
#else

	 // create socket for server...
	socket_client = socket(AF_INET, SOCK_STREAM, 0);
	 if (socket_client < 0){
		fprintf(stderr,"\nERROR opening socket");
		return INVALID_SOCKET;
	 }


	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(_portno);



	// Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
	{
		fprintf(stderr,"\nInvalid address/ Address not supported \n");
		return INVALID_SOCKET;
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		fprintf(stderr,"\nConnection Failed \n");
		return INVALID_SOCKET;
	}
#endif
	return socket_client;

}

SOCKET TcpUtils_NewSocketServer(int _portno){

	SOCKET socket_server=INVALID_SOCKET;
	struct addrinfo  serv_addr;
	uint32_t  ipaddr=0;
	bzero((char *) &serv_addr, sizeof(serv_addr));
#ifdef _WIN32
	// Initialize Winsock
	int iResult;
	struct addrinfo *result = NULL;

	serv_addr.ai_family = AF_INET;
	serv_addr.ai_socktype = SOCK_STREAM;
	serv_addr.ai_protocol = IPPROTO_TCP;
	serv_addr.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, (const char *)ZNString_IntToString(_portno), &serv_addr, &result);
	if ( iResult != 0 ) {
	   fprintf(stderr,"\ngetaddrinfo failed with error: %d", iResult);
	   return INVALID_SOCKET;
	}

	// Create a SOCKET for connecting to server
	socket_server = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (socket_server == INVALID_SOCKET) {
		fprintf(stderr,"\nsocket failed with error: %i", WSAGetLastError());
		freeaddrinfo(result);
		return INVALID_SOCKET;
	}

	// Setup the TCP listening socket
	iResult = bind( socket_server, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		fprintf(stderr,"\nbind failed with error: %i", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(socket_server);
		return INVALID_SOCKET;
	}

	freeaddrinfo(result);

	iResult = listen(socket_server, MAX_SOCKETS);
	if (iResult == SOCKET_ERROR) {
		fprintf(stderr,"\nlisten failed with error: %d", WSAGetLastError());
		closesocket(socket_server);
		return INVALID_SOCKET;
	}


#else // GNU

	 // create socket for server...
	socket_server = socket(AF_INET, SOCK_STREAM, 0);
	 if (socket_server < 0){
		fprintf(stderr,"\nERROR opening socket");
		return INVALID_SOCKET;
	 }

	 //set server socket to allow multiple connections , this is just a good habit, it will work without this
	 if(( setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))) < 0 )
	 {
		 fprintf(stderr,"\nsetsockopt:%s",getErrorSockOpt());
		 return INVALID_SOCKET;
	 }

	 // setup parameters for server binding ...
	 serv_addr.sin_family = AF_INET;
	 serv_addr.sin_addr.s_addr = INADDR_ANY;
	 serv_addr.sin_port = htons(_portno);

	if (bind(socket_server, (struct sockaddr *) &serv_addr,
				  sizeof(serv_addr)) < 0){
				  fprintf(stderr,"\nERROR on binding");
				  return INVALID_SOCKET;
	}

	listen(tcp_server->socket_server,5); // block until new connection is established...
#endif


	printf("\nSetup server  (%d.%d.%d.%d:%i)",
			ipaddr>>24,
			(ipaddr>>16)&0xff,
			(ipaddr>>8)&0xff,
			ipaddr&0xff,
			_portno);

	return socket_server;
}

//  receive  a  buffer  from  a  TCP  socket  with  error  checking
//  this  function  handles  the  memory,  so  it  can't  use  any  []  arrays
//  returns  0  on  any  errors,  or  a  valid  char*  on  success
int  TcpUtils_ReceiveBytes(SOCKET  sock,  uint8_t  *buf)
{
//#define MAXLEN 1024
	int result;
	//char msg[MAXLEN];

	result = recv(sock,(char *)buf,ZETNET_TCP_MESSAGE_LENGTH,0);

	if(result <= 0) {
		// TCP Connection is broken. (because of error or closure)
		return 0;
	}
	else {
		if(result < (ZETNET_TCP_MESSAGE_LENGTH-1)){
			buf[result] = 0;

		}
		else {
			fprintf(stderr,"TcpUtils_ReceiveBytes: Max message reached %i<%i!\n",result,ZETNET_TCP_MESSAGE_LENGTH);
			return 0;
		}
	}

	return  result;
}

//  send  a  CString  buffer  over  a  TCP  socket  with  error  checking
//  returns  0  on  any  errors,  length  sent  on  success
int  TcpUtils_SendBytes(SOCKET  sock,  uint8_t  *buf,  uint32_t  len) {
	uint32_t  result=0;

	if(!len) {
		fprintf(stderr,"TcpUtils_SendBytes: 0 bytes to send or buffer is NULL!\n");
		return 0;
	}

	//  send  the  buffer,  with  the  NULL  as  well
	result=send(sock,(const char *)buf,len,0);

	if(result<len) {
		fprintf(stderr,"TcpUtils_SendBytes (%i<%i)\n",result,len);
		return(0);
	}
	return(result);
}


void TcpUtils_CloseSocket(SOCKET *sock){
#ifdef _WIN32
	// shutdown the connection since we're done
	int iResult = shutdown(*sock, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		fprintf(stderr,"\nshutdown failed with error: %d", WSAGetLastError());
	}

	closesocket(*sock);
#else
	close(*sock);
#endif

	// now is invalid...
	*sock=INVALID_SOCKET;
}
