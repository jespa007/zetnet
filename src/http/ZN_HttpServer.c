#include "zetnet.h"

typedef struct{
	ZN_TcpServer * tcp_server;
	ZN_HttpRouteManager	* get_routes;
	ZN_HttpRouteManager	* post_routes;
}ZN_HttpServerData;

bool ZN_HttpServer_GestMessageDefault(ZN_TcpServer * tcp_server,ZN_SocketClient * socket_client, uint8_t *data, size_t data_len, void * user_data){
	ZN_UNUSUED_PARAM(tcp_server);


	ZN_HttpServer * http_server = (ZN_HttpServer *)user_data;

	ZN_HttpHandleClient * hc = ZN_HttpHandleClient_New(socket_client, http_server,data, data_len);
	ZN_HttpHandleClient_DoHandle(hc);
	return true;
}

ZN_HttpServer * ZN_HttpServer_New(
	const char * web_dir
	,const char * server_name
	){
	ZN_HttpServer 		* http_server=ZN_NEW(ZN_HttpServer);
	ZN_HttpServerData  * data=ZN_NEW(ZN_HttpServerData);
	data->tcp_server=NULL;
	data->get_routes=ZN_HttpRouteManager_New();
	data->post_routes=ZN_HttpRouteManager_New();
	http_server->data=data;

	//http_server->NAME = instance_name;
	http_server->web_dir = web_dir;

	if(http_server->web_dir==NULL){
		http_server->web_dir = ".";
	}

	http_server->name=server_name;

	return http_server;
}



bool ZN_HttpServer_Start(ZN_HttpServer *_this,int _port){
	ZN_HttpServerData *data = _this->data;
	if(data->tcp_server==NULL){
		ZN_TcpServerOnGestMessage on_gest_message=(ZN_TcpServerOnGestMessage){
					.callback_function=ZN_HttpServer_GestMessageDefault
					,.user_data=_this
		};

		data->tcp_server=ZN_TcpServer_New(on_gest_message);

		return ZN_TcpServer_Start(data->tcp_server,_port);
	}
	return false;
}

void ZN_HttpServer_Stop(ZN_HttpServer *http_server){
	ZN_HttpServerData *data = http_server->data;
	if(data->tcp_server!=NULL){
		ZN_TcpServer_Delete(data->tcp_server);
		data->tcp_server=NULL;
	}
}

void ZN_HttpServer_OnGetUserRequest(ZN_TcpServer * tcp_server,SOCKET  _socket_client, ZN_HttpParamValue  * param, size_t param_len, void *data){
	ZN_UNUSUED_PARAM(tcp_server);
	ZN_UNUSUED_PARAM(param);
	ZN_UNUSUED_PARAM(param_len);

	ZN_HttpServer * http_server = (ZN_HttpServer* )data;

	ZN_HttpResponse *resp = ZN_HttpResponse_MakeFromString("onGetUserRequest is not implemented!", "application/json");

	ZN_HttpResponse_Send(resp,_socket_client, http_server);

	ZN_HttpResponse_Delete(resp);
}

void ZN_HttpServer_AddGetRoute(ZN_HttpServer * _this,const char *_url,const char *_path,ZN_HttpResponseCallback  *_on_request){
	ZN_HttpServerData *data = _this->data;
	ZN_HttpRouteManager_AddRoute(data->get_routes,_url,_path,_on_request);
}

void ZN_HttpServer_AddPostRoute(ZN_HttpServer * _this,const char *_url,const char *_path,ZN_HttpResponseCallback  *_on_request){
	ZN_HttpServerData *data = _this->data;
	ZN_HttpRouteManager_AddRoute(data->post_routes,_url,_path,_on_request);
}

ZN_HttpRoute *ZN_HttpServer_SearchGetRoute(ZN_HttpServer * _this,const char *_url){
	ZN_HttpServerData *data = _this->data;
	return ZN_HttpRouteManager_SearchRoute(data->get_routes,_url);
}

ZN_HttpRoute *ZN_HttpServer_SearchPostRoute(ZN_HttpServer * _this,const char *_url){
	ZN_HttpServerData *data = _this->data;
	return ZN_HttpRouteManager_SearchRoute(data->post_routes,_url);
}

void ZN_HttpServer_Delete(ZN_HttpServer *_this){
	if(_this!=NULL){
		ZN_HttpServerData *data = _this->data;
		ZN_HttpRouteManager_Delete(data->get_routes);
		ZN_HttpRouteManager_Delete(data->post_routes);
		ZN_TcpServer_Delete(data->tcp_server);
		ZN_FREE(_this->data);
		ZN_FREE(_this);
	}
}

