#ifndef __HTTP_ROUTE_H__
#define __HTTP_ROUTE_H__

typedef struct HttpRoute HttpRoute;

struct HttpRoute{
	char *url;
	char *path;
	HttpResponseCallback on_request;
};


HttpRoute *HttpRoute_New(const char *_url, const char *_path, HttpResponseCallback *_on_request);
void HttpRoute_Delete(HttpRoute * _this);

#endif
