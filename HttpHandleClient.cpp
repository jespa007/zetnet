#include "HttpServer.h"

HttpHandleClient::HttpHandleClient(intptr_t _socket_client, HttpServer * _server, uint8_t *_rcv_data, uint32_t _rcv_len)
{
	this->socket_client = _socket_client;
	this->server = _server;

	this->rcv_buffer_data=_rcv_data;
	this->rcv_buffer_data_len=_rcv_len;

	//this.clNo = clineNo;
	//Thread ctThread = new Thread(doHandle);
	//ctThread.Start();
}

void HttpHandleClient::doHandle()
{
	string msg = "";
	HttpRequest *req = NULL;

#ifdef __DEBUG__
	printf("received:%s",this->rcv_buffer_data );
#endif

	req = HttpRequest::GetRequest((const char *)this->rcv_buffer_data );


	if(req->Type == "POST"){
		server->onGetUserRequest(this->socket_client,req->Param);
	}
	else{

		HttpResponse *resp = HttpResponse::From(req, this->server);
		resp->Post(this->socket_client, this->server);
		delete resp;
	}
	delete req;

}


HttpHandleClient::~HttpHandleClient(){
	//delete this->rcv_buffer;
}
