#include "zetnet.h"

namespace zetnet{

	HttpHandleClient::HttpHandleClient(SOCKET _socket_client, HttpServer * _server, uint8_t *_rcv_data, uint32_t _rcv_len)
	{
		this->socket_client = _socket_client;
		this->server = _server;

		this->rcv_buffer_data=_rcv_data;
		this->rcv_buffer_data_len=_rcv_len;
	}

	void HttpHandleClient::doHandle()
	{
		std::string msg = "";
		HttpRequest *req = NULL;

	#ifdef __DEBUG__
		printf("received:%s",this->rcv_buffer_data );
	#endif

		req = HttpRequest::getRequest((const char *)this->rcv_buffer_data );

		if(req->type == "POST"){
			server->onGetUserRequest(this->socket_client,req->param);
		}
		else{

			HttpResponse *resp = HttpResponse::from(req, this->server);
			resp->post(this->socket_client, this->server);
			delete resp;
		}
		delete req;

	}


	HttpHandleClient::~HttpHandleClient(){

	}

};
