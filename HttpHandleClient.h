#pragma once


namespace zetnet{


	class CHttpServer;
	class CHttpHandleClient //:public CThread
	{
		SOCKET socket_client;
		CHttpServer *server;
		uint8_t *rcv_buffer_data;
		uint32_t    rcv_buffer_data_len;
		std::string clNo;
	public:

		CHttpHandleClient(SOCKET _socket_client, CHttpServer *_server, uint8_t *_rcv_data, uint32_t _rcv_data_len);
		void doHandle();

		~CHttpHandleClient();
	};

};
