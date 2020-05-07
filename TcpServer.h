#ifndef __TCP_SERVER__
#define __TCP_SERVER__



#define  MAX_SOCKETS			100

typedef  struct  {
	SOCKET socket;
	int    idx_client;
	bool   streaming_header_sent; // for streaming purposes!

}  SocketClient;

typedef struct TcpServer TcpServer;

typedef struct {
	bool 		(* callback_function)(TcpServer * tcp_server,SocketClient * client_socket, uint8_t *buffer, size_t len, void *user_data);
	void 		*user_data;
}TcpServerOnGestMessage;

struct  TcpServer
{

	SocketClient  	socket_client[MAX_SOCKETS];
	int 			free_socket[MAX_SOCKETS];
	int        		n_free_sockets; // 1-n_freeSlot => TOTAL ACTIVE CLIENTS!!!
	pthread_t 		thread;
	bool configured;
	char  *message;


	uint8_t  buffer[ZETNET_TCP_MESSAGE_LENGTH];
	int	      src_port,dst_port;

	bool  	connected
	,reconnection_request
	,disconnect_request
	,is_streaming_server;


	char  value_variable_host[256];

	unsigned  initialized;

	TcpServerOnGestMessage on_gest_message;

	 //set of socket descriptors

	  fd_set readfds;
	  struct timeval timeout;


	bool end_loop_mdb;
	unsigned long time_delay_ms;

	// only linux ...


	SOCKET sockfd;
	int portno;

};
// static

TcpServer * TcpServer_New(TcpServerOnGestMessage on_gest_message);


bool  TcpServer_Start(TcpServer *tcp_server,int  port);  //  Reads  configuration  of  machine  &  init  sdl_net...
void  TcpServer_Stop(TcpServer *tcp_server);

void  TcpServer_SetTimeout(TcpServer *tcp_server, int seconds);
bool  TcpServer_IsConnected(TcpServer *tcp_server);

bool  TcpServer_CloseSocketClient(TcpServer * tcp_server,SocketClient *socket_client);


void  TcpServer_Unload(TcpServer *tcp_server);
void  TcpServer_Delete(TcpServer *tcp_server);

#endif
