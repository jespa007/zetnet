#ifndef __TCP_SERVER__
#define __TCP_SERVER__


#define  MAX_LENGTH_MESSAGE 16384
#define  MAX_SOCKETS					100

typedef  struct  {
	SOCKET socket;
	int    idxClient;
	bool    header_sent; // for streaming purposes!

}  ClientSocket;

typedef struct TcpServer TcpServer;

typedef struct {
	bool 		(* callback_function)(TcpServer * tcp_server,SOCKET in_socket, uint8_t *buffer, uint32_t len, void *user_data);
	void 		*user_data;
}TcpServerOnGestMessage;

struct  TcpServer
{

	ClientSocket  	clientSocket[MAX_SOCKETS];
	int 			freeSocket[MAX_SOCKETS];
	int        		n_freeSockets; // 1-n_freeSlot => TOTAL ACTIVE CLIENTS!!!
	pthread_t 		thread;
	bool configured;
	char  *message;
	const char *host;
	uint32_t  ipaddr;
	uint8_t  buffer[MAX_LENGTH_MESSAGE];
	int	      src_port,dst_port;

	bool  	connected
	,reconnection_request
	,disconnect_request
	,is_streaming_server;


	char  value_variable_host[256];

	unsigned  initialized;

	TcpServerOnGestMessage tcp_server_on_gest_message;

/*	void TcpServer_SetTimeDelay(unsigned long delay);

	void  TcpServer_InternalDisconnect(TcpServer * tcp_server);
	bool  TcpServer_InternalConnect(TcpServer * tcp_server);
	void  TcpServer_GetMessage(TcpServer * tcp_server);
	void  TcpServer_GestClient(TcpServer * tcp_server);

	bool TcpServer_GestServerBase(TcpServer * tcp_server);
	void  TcpServer_GestServer(TcpServer * tcp_server);

	ClientSocket 		*TcpServer_GetFreeSlot(TcpServer * tcp_server); // adds client
	bool 				TcpServer_FreeSlot(TcpServer * tcp_server,ClientSocket * sockClient); // removes client...




	void TcpServer_closeSocket(TcpServer * tcp_server,SOCKET sock);
	SOCKET TcpServer_socketAccept(TcpServer * tcp_server);
	bool TcpServer_socketReady(TcpServer * tcp_server,SOCKET sock);
	void  TcpServer_Update(TcpServer * tcp_server);  //  Receive  messages,  gest  &  send...*/

	 //set of socket descriptors
	  fd_set readfds;

#ifdef _WIN32
	WSADATA wsaData;

#endif

	struct timeval timeout;
	bool end_loop_mdb;
	unsigned long time_delay_ms;

	// only linux ...

	struct addrinfo  serv_addr, cli_addr;
	SOCKET sockfd;
	int portno;

};
// static
int  TcpServer_SendBytes(SOCKET  sock,  uint8_t  *buf,  uint32_t  len);


TcpServer * TcpServer_New();


bool  TcpServer_Setup(TcpServer *tcp_server,int  port, const char *name_client);  //  Reads  configuration  of  machine  &  init  sdl_net...
void  TcpServer_SetTimeout(TcpServer *tcp_server, int seconds);

bool  TcpServer_isConnected(TcpServer *tcp_server);
void  TcpServer_Connect(TcpServer *tcp_server);
void  TcpServer_Disconnect(TcpServer *tcp_server);
void  TcpServer_Reconnection(TcpServer *tcp_server);

void TcpServer_Resume(TcpServer *tcp_server);
void TcpServer_Pause(TcpServer *tcp_server);

void  TcpServer_Unload(TcpServer *tcp_server);
void  TcpServer_Delete(TcpServer *tcp_server);

#endif
