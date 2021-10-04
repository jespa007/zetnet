#ifndef __HTTP_SERVER__
#define __HTTP_SERVER__

typedef struct HttpServer HttpServer;


typedef struct{
	void (*callback_function)(HttpServer * http_server,SocketClient  * _socket_client, HttpParamValue  * param, size_t param_len,void *user_data);
	void *user_data;
}HttpServerOnGetUserRequest;

struct HttpServer
{
	const char * msg_dir;
	const char * web_dir;
	const char * version;
	const char * name;
	const char * logo_base64;
	bool send_same_site_attribute;
	HttpServerOnGetUserRequest on_get_user_request;

	//private
	void * data;

};

HttpServer *HttpServer_New(const char * web_dir,const char *  instance_name);
bool HttpServer_Start(HttpServer *http_server, int port);
void HttpServer_Stop(HttpServer *http_server);
void HttpServer_Delete(HttpServer *http_server);


#endif
