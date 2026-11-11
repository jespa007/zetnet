#ifndef __ZN_HTTP_ROUTER_H__
#define __ZN_HTTP_ROUTER_H__

typedef struct ZN_HttpRouter ZN_HttpRouter;

struct ZN_HttpRouter{
	void *data;
};


ZN_HttpRouter *			ZN_HttpRouter_New(void);
void 					ZN_HttpRouter_AddRoute(ZN_HttpRouter * _this, const char *_url,const char *_path,ZN_HttpResponseCallback  *_on_request);
ZN_HttpRoute *			ZN_HttpRouter_SearchRoute(ZN_HttpRouter * _this, const char *__url);
void 					ZN_HttpRouter_Delete(ZN_HttpRouter * _this);

#endif

