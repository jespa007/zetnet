#include "zetnet.h"

SOCKET TcpUtils_NewSocketServer(int _portno){

	SOCKET socket_server=INVALID_SOCKET;
	struct addrinfo  serv_addr;
	uint32_t  ipaddr=0;
	bzero((char *) &serv_addr, sizeof(serv_addr));
	int i_result;
	//int opt = 1;
	int error=0;
//#ifdef _WIN32
	// Initialize Winsock


	struct addrinfo *result = NULL;
//#ifdef _WIN32
	serv_addr.ai_family = AF_INET;
	serv_addr.ai_socktype = SOCK_STREAM;
	serv_addr.ai_protocol = IPPROTO_TCP;
	serv_addr.ai_flags = AI_PASSIVE;
/*#else // linux
	serv_addr.sin_family = AF_INET;
	serv_addr.ai_socktype = SOCK_STREAM;
	serv_addr.ai_protocol = IPPROTO_TCP;
	serv_addr.ai_flags = AI_PASSIVE;
#endif*/

	// Resolve the server address and port
	i_result = getaddrinfo(NULL, (const char *)zn_str_from_int(_portno), &serv_addr, &result);
	if ( i_result != 0 ) {
	   fprintf(stderr,"\ngetaddrinfo failed with error: %d", i_result);
	   return INVALID_SOCKET;
	}

	// Create a SOCKET for connecting to server
	socket_server = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (socket_server == INVALID_SOCKET) {
#ifdef _WIN32
		error=WSAGetLastError();
#endif
		fprintf(stderr,"\nsocket failed with error: %i", error);
		freeaddrinfo(result);
		return INVALID_SOCKET;
	}

	 //set server socket to allow multiple connections , this is just a good habit, it will work without this
	/* if(( setsockopt(socket_server, SOL_SOCKET, 0, (char *)&opt, sizeof(opt))) < 0 )
	 {
#ifdef _WIN32
		error=WSAGetLastError();
#endif
		 fprintf(stderr,"\nsetsockopt failed error: %i", error);
		 return INVALID_SOCKET;
	 }*/

	// Setup the TCP listening socket
	if(bind( socket_server, result->ai_addr, (int)result->ai_addrlen)<0){

#ifdef _WIN32
		error=WSAGetLastError();
#endif
		switch(error){
#ifdef _WIN32
		case WSAEADDRINUSE:
			fprintf(stderr,"\nport %i alraedy in use",_portno);
#endif
			break;
		default:
			fprintf(stderr,"\nbind failed with error: %i", error);
			break;
		}
		freeaddrinfo(result);
		TcpUtils_CloseSocket(&socket_server);
		return INVALID_SOCKET;
	}

	freeaddrinfo(result);

	i_result = listen(socket_server, MAX_SOCKETS);
	if (i_result == SOCKET_ERROR) {
#ifdef _WIN32
		error=WSAGetLastError();
#endif
		fprintf(stderr,"\nlisten failed with error: %i", error);

		TcpUtils_CloseSocket(&socket_server);
		return INVALID_SOCKET;
	}


/*#else // GNU
	int opt = 1;
	 // create socket for server...
	socket_server = socket(AF_INET, SOCK_STREAM, 0);
	 if (socket_server < 0){
		fprintf(stderr,"\nERROR opening socket");
		return INVALID_SOCKET;
	 }

	 //set server socket to allow multiple connections , this is just a good habit, it will work without this
	 if(( setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))) < 0 )
	 {
		 fprintf(stderr,"\nsetsockopt failed");
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

	listen(socket_server,5); // block until new connection is established...
#endif*/


	printf("\nSetup server  (%d.%d.%d.%d:%i)",
			ipaddr>>24,
			(ipaddr>>16)&0xff,
			(ipaddr>>8)&0xff,
			ipaddr&0xff,
			_portno);

	return socket_server;
}


SOCKET TcpUtils_NewSocketClient(int _portno){
	SOCKET socket_client=INVALID_SOCKET;
	struct addrinfo	serv_addr;
	int error=0;
	bzero((char *) &serv_addr, sizeof(serv_addr));
//#ifdef _WIN32
	struct        addrinfo         *ptr = NULL,*result=NULL;



	serv_addr.ai_family = AF_INET;
	serv_addr.ai_socktype = SOCK_STREAM;
	serv_addr.ai_protocol = IPPROTO_TCP;


	// Resolve the server address and port
	int i_result = getaddrinfo("127.0.0.1", (const char *)zn_str_from_int(_portno), &serv_addr, &result);
	if ( i_result != 0 ) {
		fprintf(stderr,"getaddrinfo failed with error: %d\n", i_result);
		return INVALID_SOCKET;
	}

	 // Attempt to connect to an address until one succeeds
	for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

		// Create a SOCKET for connecting to server
		socket_client = socket(ptr->ai_family, ptr->ai_socktype,ptr->ai_protocol);
		if (socket_client == INVALID_SOCKET) {
	#ifdef _WIN32
			error=WSAGetLastError();
	#endif
			fprintf(stderr,"\nsocket failed with error: %i", error);
			return INVALID_SOCKET;
		}

		// Connect to server.
		i_result = connect( socket_client, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (i_result == SOCKET_ERROR) {
	#ifdef _WIN32
			error=WSAGetLastError();
	#endif
			fprintf(stderr,"\nconnect failed with error: %i", error);
			TcpUtils_CloseSocket(&socket_client);
			socket_client = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (socket_client == INVALID_SOCKET) {
		fprintf(stderr,"Unable to connect to server!\n");
	}
/*#else

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

	if (connect(socket_client, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		fprintf(stderr,"\nConnection Failed \n");
		return INVALID_SOCKET;
	}
#endif*/
	return socket_client;

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

	if(*sock == INVALID_SOCKET){
		return;
	}

#ifdef _WIN32
	closesocket(*sock);
#else
	close(*sock);
#endif

	// now is invalid...
	*sock=INVALID_SOCKET;
}
