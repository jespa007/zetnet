#ifndef __ZN_TCP_SERVER__
#define __ZN_TCP_SERVER__

// TODO : Make max clients variable through configuration
#define  	ZN_TCP_SERVER_MAX_CLIENTS			100
#define 	ZN_TCP_SERVER_BUFFER_LENGTH			512

typedef  struct  {
	SOCKET 		socket;
	int    		idx_client;
	bool   		streaming_header_sent; // for streaming purposes!
}  ZN_TcpServerClient;

typedef struct ZN_TcpServer ZN_TcpServer;

typedef struct {
	bool 		(* callback_function)(ZN_TcpServer * tcp_server,ZN_TcpServerClient * client_socket, uint8_t *buffer, size_t len, void *user_data);
	void 		*user_data;
}ZN_TcpServerOnGestMessage;

struct  ZN_TcpServer
{
	uint8_t  			buffer[ZN_TCP_SERVER_BUFFER_LENGTH];
	ZN_TcpServerClient  clients[ZN_TCP_SERVER_MAX_CLIENTS];
	int 				free_slots[ZN_TCP_SERVER_MAX_CLIENTS];
	int        			n_free_slots; // 1-n_freeSlot => TOTAL ACTIVE CLIENTS!!!
	pthread_t 			thread;
	bool 				configured;
	char  		*		message;

	int	      			src_port,dst_port;

	bool  				is_streaming_server;


	char  				value_variable_host[256];

	unsigned  			initialized;

	ZN_TcpServerOnGestMessage on_gest_message;

	//set of socket descriptors
	fd_set 			readfds;
	struct 			timeval timeout;


	bool 			end_loop_mdb;
	unsigned long 	time_delay_ms;

	SOCKET 			sockfd;
	int 			portno;

};
// static

ZN_TcpServer * ZN_TcpServer_New(ZN_TcpServerOnGestMessage on_gest_message);


bool  ZN_TcpServer_Start(ZN_TcpServer *tcp_server,const char * _host, int  port);  //  Reads  configuration  of  machine  &  init  sdl_net...
void  ZN_TcpServer_Stop(ZN_TcpServer *tcp_server);

void  ZN_TcpServer_SetTimeout(ZN_TcpServer *tcp_server, int seconds);
bool  ZN_TcpServer_IsConnected(ZN_TcpServer *tcp_server);

bool  ZN_TcpServer_CloseClient(ZN_TcpServer * tcp_server,ZN_TcpServerClient *socket_client);
bool  ZN_TcpServer_CloseChannel(ZN_TcpServer * tcp_server,ZN_TcpServerClient *socket_client, int _channel);

void  ZN_TcpServer_Unload(ZN_TcpServer *tcp_server);
void  ZN_TcpServer_Delete(ZN_TcpServer *tcp_server);

#endif
