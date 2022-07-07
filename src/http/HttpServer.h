#ifndef __HTTP_SERVER__
#define __HTTP_SERVER__



struct HttpServer
{
	const char * msg_dir;
	const char * web_dir;
	const char * version;
	const char * name;
	const char * logo_base64;
	bool send_same_site_attribute;
	HttpResponseCallback  on_request;

	//private
	void * data;

};

HttpServer *HttpServer_New(const char * _web_dir,const char * _instance_name);
void HttpServer_AddGetRoute(HttpServer * _this,const char *_url,const char *_path,HttpResponseCallback  *_on_request);
void HttpServer_AddPostRoute(HttpServer * _this,const char *_url,const char *_path,HttpResponseCallback  *_on_request);
HttpRoute *HttpServer_SearchGetRoute(HttpServer * _this,const char *_url_start_request);
HttpRoute *HttpServer_SearchPostRoute(HttpServer * _this,const char *_url_start_request);

bool HttpServer_Start(HttpServer *_this, int _port);
void HttpServer_Stop(HttpServer *_this);
void HttpServer_Delete(HttpServer *_this);


#endif
