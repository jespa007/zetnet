#include "zetnet.h"


HttpHandleClient * HttpHandleClient_New(SocketClient * _socket_client, HttpServer * _http_server, uint8_t *_rcv_data, uint32_t _rcv_len)
{
	HttpHandleClient * http_handle_client = ZN_MALLOC(sizeof(HttpHandleClient));
	http_handle_client->socket_client = _socket_client;
	http_handle_client->http_server = _http_server;

	http_handle_client->rcv_buffer_data=_rcv_data;
	http_handle_client->rcv_buffer_data_len=_rcv_len;

	return http_handle_client;
}

void HttpHandleClient_DoHandle(HttpHandleClient *http_handle_client)
{
	//std::string msg = "";
	HttpRequest *req = NULL;

#ifdef __DEBUG__
	printf("\nreceived:%s",http_handle_client->rcv_buffer_data );
#endif

	req = HttpRequest_GetRequest((const char *)http_handle_client->rcv_buffer_data );

	if(strcmp(req->type,"POST")==0){
		void *param=NULL;
		size_t param_len=0;
		HttpServerOnGetUserRequest cf=http_handle_client->http_server->on_get_user_request;

		if(req->param!=NULL){
			param=req->param->items;
			param_len=req->param->count;
		}

		cf.callback_function(http_handle_client->http_server,http_handle_client->socket_client,(HttpParamValue *)param,param_len,cf.user_data);
	}
	else{
		HttpResponse *resp = HttpResponse_From(req, http_handle_client->http_server);
		HttpResponse_Post(resp,http_handle_client->socket_client->socket, http_handle_client->http_server);
		HttpResponse_Delete(resp);
	}

	HttpRequest_Delete(req);
	HttpHandleClient_Delete(http_handle_client);

}

void HttpHandleClient_Delete(HttpHandleClient *http_handle_client){
	if(http_handle_client!=NULL){
		ZN_FREE(http_handle_client);
	}
}


