#ifndef __ZN_TCP_SERVER__
#define __ZN_TCP_SERVER__

#define  ZN_MAX_SOCKETS			100

typedef  struct  {
	SOCKET socket;
	int    idx_client;
	bool   streaming_header_sent; // for streaming purposes!

}  ZN_SocketClient;

typedef struct ZN_TcpServer ZN_TcpServer;

typedef struct {
	bool 		(* callback_function)(ZN_TcpServer * tcp_server,ZN_SocketClient * client_socket, uint8_t *buffer, size_t len, void *user_data);
	void 		*user_data;
}ZN_TcpServerOnGestMessage;

struct  ZN_TcpServer
{

	ZN_SocketClient  	socket_client[ZN_MAX_SOCKETS];
	int 			free_socket[ZN_MAX_SOCKETS];
	int        		n_free_sockets; // 1-n_freeSlot => TOTAL ACTIVE CLIENTS!!!
	pthread_t 		thread;
	bool configured;
	char  *message;


	uint8_t  buffer[ZN_TCP_MESSAGE_LENGTH];
	int	      src_port,dst_port;

	bool  is_streaming_server;


	char  value_variable_host[256];

	unsigned  initialized;

	ZN_TcpServerOnGestMessage on_gest_message;

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

ZN_TcpServer * ZN_TcpServer_New(ZN_TcpServerOnGestMessage on_gest_message);


bool  ZN_TcpServer_Start(ZN_TcpServer *tcp_server,int  port);  //  Reads  configuration  of  machine  &  init  sdl_net...
void  ZN_TcpServer_Stop(ZN_TcpServer *tcp_server);

void  ZN_TcpServer_SetTimeout(ZN_TcpServer *tcp_server, int seconds);
bool  ZN_TcpServer_IsConnected(ZN_TcpServer *tcp_server);

bool  ZN_TcpServer_CloseSocketClient(ZN_TcpServer * tcp_server,ZN_SocketClient *socket_client);


void  ZN_TcpServer_Unload(ZN_TcpServer *tcp_server);
void  ZN_TcpServer_Delete(ZN_TcpServer *tcp_server);

#endif
