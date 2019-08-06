#pragma once


namespace zetnet{


	class HttpServer;
	class HttpHandleClient //:public CThread
	{
		SOCKET socket_client;
		HttpServer *server;
		uint8_t *rcv_buffer_data;
		uint32_t    rcv_buffer_data_len;
		std::string clNo;
	public:

		HttpHandleClient(SOCKET _socket_client, HttpServer *_server, uint8_t *_rcv_data, uint32_t _rcv_data_len);
		void doHandle();

		~HttpHandleClient();
	};

};
