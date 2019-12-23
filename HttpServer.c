#include "zetnet.h"



bool TcpServer_HttpServer_GestMessageDefault(TcpServer * tcp_server,SOCKET socket, uint8_t *buffer, uint32_t buffer_len, void * user_data){
	HttpServer * http_server = (HttpServer *)user_data;

	HttpHandleClient * hc = HttpHandleClient_New(socket, http_server,buffer, buffer_len);
	HttpHandleClient_DoHandle(hc);
	return true;
}


HttpServer * HttpServer_New(
		int _port
		,const char * web_dir
		,const char * instance_name){
	HttpServer * http_server=malloc(sizeof(HttpServer));

	http_server->NAME = instance_name;
	http_server->WEB_DIR = web_dir;
	http_server->web_client = NULL;

	// init tcp server...
	http_server->tcp_server=TcpServer_New();
	http_server->tcp_server->tcp_server_on_gest_message=(TcpServerOnGestMessage){
		.callback_function=TcpServer_HttpServer_GestMessageDefault
		,.user_data=http_server
	};
	TcpServer_Setup(http_server->tcp_server,_port,http_server->NAME);

	return http_server;
}



void HttpServer_Connect(HttpServer *http_server){
	TcpServer_Connect(http_server->tcp_server);
}


void HttpServer_OnGetUserRequest(TcpServer * tcp_server,SOCKET  _socket_client, HttpParamValue  * param, size_t param_len, void *data){

	HttpServer * http_server = (HttpServer* )data;

	HttpResponse *resp = HttpResponse_MakeFromString("onGetUserRequest is not implemented!", "application/json");

	HttpResponse_Post(resp,_socket_client, http_server);

	HttpResponse_Delete(resp);
}


void HttpServer_Delete(HttpServer *http_server){
	if(http_server!=NULL){
		TcpServer_Delete(http_server->tcp_server);
		free(http_server);
	}
}

