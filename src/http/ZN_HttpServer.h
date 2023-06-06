#ifndef __ZN_HTTP_SERVER__
#define __ZN_HTTP_SERVER__



struct ZN_HttpServer
{
	const char * msg_dir;
	const char * web_dir;
	const char * version;
	const char * name;
	const char * logo_base64;
	bool send_same_site_attribute;
	ZN_HttpResponseCallback  on_request;

	//private
	void * data;

};

ZN_HttpServer *	ZN_HttpServer_New(const char * _web_dir,const char * _instance_name);
void 			ZN_HttpServer_AddGetRoute(ZN_HttpServer * _this,const char *_url,const char *_path,ZN_HttpResponseCallback  *_on_request);
void 			ZN_HttpServer_AddPostRoute(ZN_HttpServer * _this,const char *_url,const char *_path,ZN_HttpResponseCallback  *_on_request);
ZN_HttpRoute *	ZN_HttpServer_SearchGetRoute(ZN_HttpServer * _this,const char *_url_start_request);
ZN_HttpRoute *	ZN_HttpServer_SearchPostRoute(ZN_HttpServer * _this,const char *_url_start_request);

bool 			ZN_HttpServer_Start(ZN_HttpServer *_this, int _port);
void 			ZN_HttpServer_Stop(ZN_HttpServer *_this);
void 			ZN_HttpServer_Delete(ZN_HttpServer *_this);


#endif
