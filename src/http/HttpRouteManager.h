#ifndef __HTTP_ROUTE_MANAGER_H__
#define __HTTP_ROUTE_MANAGER_H__

typedef struct HttpRouteManager HttpRouteManager;

struct HttpRouteManager{
	void *data;
};


HttpRouteManager *	HttpRouteManager_New(void);
void 				HttpRouteManager_AddRoute(HttpRouteManager * _this, const char *_url,const char *_path,HttpResponseCallback  *_on_request);
HttpRoute *			HttpRouteManager_SearchRoute(HttpRouteManager * _this, const char *__url);
void 				HttpRouteManager_Delete(HttpRouteManager * _this);

#endif

