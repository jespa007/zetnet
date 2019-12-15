#include "zetnet.h"

namespace zetnet{

	CHttpHandleClient::CHttpHandleClient(SOCKET _socket_client, CHttpServer * _server, uint8_t *_rcv_data, uint32_t _rcv_len)
	{
		this->socket_client = _socket_client;
		this->server = _server;

		this->rcv_buffer_data=_rcv_data;
		this->rcv_buffer_data_len=_rcv_len;
	}

	void CHttpHandleClient::doHandle()
	{
		std::string msg = "";
		CHttpRequest *req = NULL;

	#ifdef __DEBUG__
		printf("received:%s",this->rcv_buffer_data );
	#endif

		req = CHttpRequest::getRequest((const char *)this->rcv_buffer_data );

		if(req->type == "POST"){
			server->onGetUserRequest(this->socket_client,req->param);
		}
		else{

			CHttpResponse *resp = CHttpResponse::from(req, this->server);
			resp->post(this->socket_client, this->server);
			delete resp;
		}
		delete req;

	}


	CHttpHandleClient::~CHttpHandleClient(){

	}

};
