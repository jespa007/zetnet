#ifndef __ZN_HTTP_ROUTE_MANAGER_H__
#define __ZN_HTTP_ROUTE_MANAGER_H__

typedef struct ZN_HttpRouteManager ZN_HttpRouteManager;

struct ZN_HttpRouteManager{
	void *data;
};


ZN_HttpRouteManager *	ZN_HttpRouteManager_New(void);
void 					ZN_HttpRouteManager_AddRoute(ZN_HttpRouteManager * _this, const char *_url,const char *_path,ZN_HttpResponseCallback  *_on_request);
ZN_HttpRoute *			ZN_HttpRouteManager_SearchRoute(ZN_HttpRouteManager * _this, const char *__url);
void 					ZN_HttpRouteManager_Delete(ZN_HttpRouteManager * _this);

#endif

