﻿#ifndef __HTTP_SERVER__
#define __HTTP_SERVER__

typedef struct HttpServer HttpServer;


struct HttpServer
{
	const char * MSG_DIR;
	const char * WEB_DIR;
	const char * VERSION;
	const char * NAME;
	const char * LOGO_BASE64;

	TcpServer * tcp_server;

	void (*OnGetUserRequest)(HttpServer * http_server,SOCKET  _socket_client, HttpParamValue  * param, size_t param_len);

	//private
	void * web_client;

};

HttpServer *HttpServer_New(int _port,const char * web_dir,const char *  instance_name);
void HttpServer_Connect(HttpServer *http_server);
void HttpServer_Delete(HttpServer *http_server);


#endif
