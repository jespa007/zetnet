#ifndef __HTTP_HANDLE_CLIENT__
#define __HTTP_HANDLE_CLIENT__

typedef struct HttpHandleClient HttpHandleClient;

struct HttpHandleClient //:public CThread
{
	SOCKET socket_client;
	HttpServer *server;
	uint8_t *rcv_buffer_data;
	uint32_t    rcv_buffer_data_len;
	//std::string clNo;


};

HttpHandleClient * HttpHandleClient_New(SOCKET _socket_client, HttpServer *_server, uint8_t *_rcv_data, uint32_t _rcv_data_len);
void HttpHandleClient_DoHandle(HttpHandleClient * http_handle_client);
void HttpHandleClient_Delete(HttpHandleClient * http_handle_client);


#endif
