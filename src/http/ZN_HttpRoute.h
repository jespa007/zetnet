#ifndef __ZN_HTTP_ROUTE_H__
#define __ZN_HTTP_ROUTE_H__

typedef struct ZN_HttpRoute ZN_HttpRoute;

struct ZN_HttpRoute{
	char *url;
	char *path;
	ZN_HttpResponseCallback on_request;
};


ZN_HttpRoute *ZN_HttpRoute_New(const char *_url, const char *_path, ZN_HttpResponseCallback *_on_request);
void ZN_HttpRoute_Delete(ZN_HttpRoute * _this);

#endif
