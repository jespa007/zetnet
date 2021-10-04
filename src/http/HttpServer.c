#include "zetnet.h"

typedef struct{
	TcpServer * tcp_server;

}HttpServerData;

bool HttpServer_GestMessageDefault(TcpServer * tcp_server,SocketClient * socket_client, uint8_t *data, size_t data_len, void * user_data){
	ZN_UNUSUED_PARAM(tcp_server);


	HttpServer * http_server = (HttpServer *)user_data;

	HttpHandleClient * hc = HttpHandleClient_New(socket_client, http_server,data, data_len);
	HttpHandleClient_DoHandle(hc);
	return true;
}

HttpServer * HttpServer_New(
	const char * web_dir
	,const char * server_name
	){
	HttpServer 		* http_server=ZN_MALLOC(sizeof(HttpServer));
	HttpServerData  * data=ZN_MALLOC(sizeof(HttpServerData));
	data->tcp_server=NULL;
	http_server->data=data;

	//http_server->NAME = instance_name;
	http_server->web_dir = web_dir;
	http_server->name=server_name;


	return http_server;
}



bool HttpServer_Start(HttpServer *http_server,int _port){
	HttpServerData *data = http_server->data;
	if(data->tcp_server==NULL){
		TcpServerOnGestMessage on_gest_message=(TcpServerOnGestMessage){
					.callback_function=HttpServer_GestMessageDefault
					,.user_data=http_server
		};

		data->tcp_server=TcpServer_New(on_gest_message);

		return TcpServer_Start(data->tcp_server,_port);
	}
	return false;
}

void HttpServer_Stop(HttpServer *http_server){
	HttpServerData *data = http_server->data;
	if(data->tcp_server!=NULL){
		TcpServer_Delete(data->tcp_server);
		data->tcp_server=NULL;
	}
}


void HttpServer_OnGetUserRequest(TcpServer * tcp_server,SOCKET  _socket_client, HttpParamValue  * param, size_t param_len, void *data){
	ZN_UNUSUED_PARAM(tcp_server);
	ZN_UNUSUED_PARAM(param);
	ZN_UNUSUED_PARAM(param_len);

	HttpServer * http_server = (HttpServer* )data;

	HttpResponse *resp = HttpResponse_MakeFromString("onGetUserRequest is not implemented!", "application/json");

	HttpResponse_Post(resp,_socket_client, http_server);

	HttpResponse_Delete(resp);
}


void HttpServer_Delete(HttpServer *http_server){
	if(http_server!=NULL){
		HttpServerData *data = http_server->data;
		TcpServer_Delete(data->tcp_server);
		ZN_FREE(http_server->data);
		ZN_FREE(http_server);
	}
}

