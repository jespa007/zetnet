#include "zetnet.h"



HttpHandleClient * HttpHandleClient_New(SOCKET _socket_client, HttpServer * _server, uint8_t *_rcv_data, uint32_t _rcv_len)
{
	HttpHandleClient * http_handle_client = malloc(sizeof(HttpHandleClient));
	http_handle_client->socket_client = _socket_client;
	http_handle_client->server = _server;

	http_handle_client->rcv_buffer_data=_rcv_data;
	http_handle_client->rcv_buffer_data_len=_rcv_len;

	return http_handle_client;
}

void HttpHandleClient_DoHandle(HttpHandleClient *http_handle_client)
{
	//std::string msg = "";
	HttpRequest *req = NULL;

#ifdef __DEBUG__
	printf("received:%s",http_handle_client->rcv_buffer_data );
#endif

	req = HttpRequest_GetRequest((const char *)http_handle_client->rcv_buffer_data );

	if(req->type == "POST"){
		HttpServer_OnGetUserRequest(http_handle_client->server,http_handle_client->socket_client,req->param);
	}
	else{

		HttpResponse *resp = HttpResponse_From(req, http_handle_client->server);
		HttpResponse_Post(resp,http_handle_client->socket_client, http_handle_client->server);
		HttpResponse_Delete(resp);
	}
	HttpRequest_Delete(req);

}


void HttpHandleClient_Delete(HttpHandleClient *http_handle_client){
	if(http_handle_client!=NULL){
		free(http_handle_client);
	}

}


