#pragma once


class HttpServer;
class HttpHandleClient //:public CThread
{
	TCPsocket socket_client;
	HttpServer *server;
	Uint8 *rcv_buffer_data;
	int    rcv_buffer_data_len;
	string clNo;
public:

	HttpHandleClient(void * _socket_client, HttpServer *_server, Uint8 *_rcv_data, int _rcv_data_len);
	void doHandle();

	~HttpHandleClient();
};
