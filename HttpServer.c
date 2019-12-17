#include "zetnet.h"


void HttpServer_SetLogoBase64( HttpServer *http_server,const char * _image_base_64);


bool Http_GestMessageDefault(HttpServer * http_server,SOCKET socket, uint8_t *data, uint32_t size){

	HttpHandleClient * hc = HttpHandleClient_New(socket, http_server,data, size);
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
	TcpServer_Setup(http_server->tcp_server,_port,http_server->NAME);
}



void HttpServer_Connect(HttpServer *http_server){
	TcpServer_Connect(http_server->tcp_server);
}


void HttpServer_OnGetUserRequest(HttpServer * http_server,SOCKET  _socket_client,ZNList * param){

	HttpResponse *resp = HttpResponse_MakeFromString("onGetUserRequest is not implemented!", "application/json");

	HttpResponse_Post(resp,_socket_client, http_server);

	HttpResponse_Delete(resp);
}




