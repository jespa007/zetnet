#include "zetnet.h"

typedef struct{
	TcpServer * tcp_server;
	HttpRouteManager	* get_routes;
	HttpRouteManager	* post_routes;
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
	HttpServer 		* http_server=ZN_NEW(HttpServer);
	HttpServerData  * data=ZN_NEW(HttpServerData);
	data->tcp_server=NULL;
	data->get_routes=HttpRouteManager_New();
	data->post_routes=HttpRouteManager_New();
	http_server->data=data;

	//http_server->NAME = instance_name;
	http_server->web_dir = web_dir;

	if(http_server->web_dir==NULL){
		http_server->web_dir = ".";
	}

	http_server->name=server_name;

	return http_server;
}



bool HttpServer_Start(HttpServer *_this,int _port){
	HttpServerData *data = _this->data;
	if(data->tcp_server==NULL){
		TcpServerOnGestMessage on_gest_message=(TcpServerOnGestMessage){
					.callback_function=HttpServer_GestMessageDefault
					,.user_data=_this
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

void HttpServer_AddGetRoute(HttpServer * _this,const char *_url,const char *_path,HttpRouteOnRequest  *_on_request){
	HttpServerData *data = _this->data;
	HttpRouteManager_AddRoute(data->get_routes,_url,_path,_on_request);
}

void HttpServer_AddPostRoute(HttpServer * _this,const char *_url,const char *_path,HttpRouteOnRequest  *_on_request){
	HttpServerData *data = _this->data;
	HttpRouteManager_AddRoute(data->post_routes,_url,_path,_on_request);
}

HttpRoute *HttpServer_SearchGetRoute(HttpServer * _this,const char *_url){
	HttpServerData *data = _this->data;
	return HttpRouteManager_SearchRoute(data->get_routes,_url);
}

HttpRoute *HttpServer_SearchPostRoute(HttpServer * _this,const char *_url){
	HttpServerData *data = _this->data;
	return HttpRouteManager_SearchRoute(data->post_routes,_url);
}

void HttpServer_Delete(HttpServer *_this){
	if(_this!=NULL){
		HttpServerData *data = _this->data;
		HttpRouteManager_Delete(data->get_routes);
		HttpRouteManager_Delete(data->post_routes);
		TcpServer_Delete(data->tcp_server);
		ZN_FREE(_this->data);
		ZN_FREE(_this);
	}
}

