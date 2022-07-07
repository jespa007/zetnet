#ifndef __HTTP_ROUTE_H__
#define __HTTP_ROUTE_H__

typedef struct{
	void (*callback_function)(HttpServer * http_server,SocketClient  * _socket_client, HttpParamValue  * _param, size_t _param_len,void *_user_data);
	void *user_data;
}HttpRouteOnRequest;

typedef struct HttpRoute HttpRoute;

struct HttpRoute{
	char *url;
	char *path;
	HttpRouteOnRequest on_request;
};


HttpRoute *HttpRoute_New(const char *_url, const char *_path, HttpRouteOnRequest *_on_request);
void HttpRoute_Delete(HttpRoute * _this);

#endif
