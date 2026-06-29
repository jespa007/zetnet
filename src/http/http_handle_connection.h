#ifndef __ZN_HTTP_HANDLE_CONNECTION__
#define __ZN_HTTP_HANDLE_CONNECTION__

typedef struct ZN_HttpHandleConnection ZN_HttpHandleConnection;

struct ZN_HttpHandleConnection
{
	ZN_TcpServerClient	* 	client;
	ZN_HttpServer 		*	http_server;
	uint8_t    			*	rcv_buffer_data;
	uint32_t    			rcv_buffer_data_len;
};

ZN_HttpHandleConnection * ZN_HttpHandleConnection_New(ZN_TcpServerClient * _socket_client, ZN_HttpServer *http_server, uint8_t *_rcv_data, uint32_t _rcv_data_len);
void ZN_HttpHandleConnection_DoHandle(ZN_HttpHandleConnection * http_handle_client);
void ZN_HttpHandleConnection_Delete(ZN_HttpHandleConnection * http_handle_client);


#endif
