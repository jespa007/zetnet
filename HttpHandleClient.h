#pragma once





class HttpServer;
class HttpHandleClient //:public CThread
{
	intptr_t socket_client;
	HttpServer *server;
	uint8_t *rcv_buffer_data;
	uint32_t    rcv_buffer_data_len;
	string clNo;
public:

	HttpHandleClient(intptr_t _socket_client, HttpServer *_server, uint8_t *_rcv_data, uint32_t _rcv_data_len);
	void doHandle();

	~HttpHandleClient();
};
