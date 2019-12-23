#ifndef __HTTP_SERVER__
#define __HTTP_SERVER__

typedef struct HttpServer HttpServer;


typedef struct{
	void (*callback_function)(HttpServer * http_server,SOCKET  _socket_client, HttpParamValue  * param, size_t param_len,void *user_data);
	void *user_data;
}HttpServerOnGetUserRequest;

struct HttpServer
{
	const char * MSG_DIR;
	const char * WEB_DIR;
	const char * VERSION;
	const char * NAME;
	const char * LOGO_BASE64;

	TcpServer * tcp_server;

	HttpServerOnGetUserRequest http_server_on_get_user_request;

	//private
	void * web_client;

};

HttpServer *HttpServer_New(const char * web_dir,const char *  instance_name);
void HttpServer_Start(HttpServer *http_server, int port);
void HttpServer_Stop(HttpServer *http_server);
void HttpServer_Delete(HttpServer *http_server);


#endif
