#ifndef __ZN_HTTP_HANDLE_CLIENT__
#define __ZN_HTTP_HANDLE_CLIENT__

typedef struct ZN_HttpHandleClient ZN_HttpHandleClient;

struct ZN_HttpHandleClient
{
	ZN_SocketClient	* 	socket_client;
	ZN_HttpServer 		*	http_server;
	uint8_t    		*	rcv_buffer_data;
	uint32_t    		rcv_buffer_data_len;
};

ZN_HttpHandleClient * ZN_HttpHandleClient_New(ZN_SocketClient * _socket_client, ZN_HttpServer *http_server, uint8_t *_rcv_data, uint32_t _rcv_data_len);
void ZN_HttpHandleClient_DoHandle(ZN_HttpHandleClient * http_handle_client);
void ZN_HttpHandleClient_Delete(ZN_HttpHandleClient * http_handle_client);


#endif
