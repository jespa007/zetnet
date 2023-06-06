#include "zetnet.h"

typedef struct{
	ZN_List *routes;
}ZN_HttpRouteManagerData;

ZN_HttpRouteManager *ZN_HttpRouteManager_New(void){
	ZN_HttpRouteManager *http_route_manager=ZN_NEW(ZN_HttpRouteManager);
	ZN_HttpRouteManagerData *data=ZN_NEW(ZN_HttpRouteManagerData);
	data->routes=ZN_List_New();

	http_route_manager->data=data;
	return http_route_manager;
}

void ZN_HttpRouteManager_AddRoute(ZN_HttpRouteManager * _this, const char *_url,const char *_path,ZN_HttpResponseCallback  *_on_request){
	ZN_HttpRouteManagerData *data=_this->data;
	ZN_HttpRoute *route=ZN_HttpRoute_New(_url,_path, _on_request);
	ZN_List_Add(data->routes,route);
}

ZN_HttpRoute *ZN_HttpRouteManager_SearchRoute(ZN_HttpRouteManager * _this, const char *_url){
	ZN_HttpRouteManagerData *data=_this->data;

	// search the most equal _url
	for(unsigned i=0; i < data->routes->count; i++){
		ZN_HttpRoute *route=data->routes->items[i];
		int res=strcmp(_url,route->url);
		if(res==0){
			// url matches
			return route;
		}else if(strcmp(_url,route->url) > 0){
			// starts with
			int url_len=strlen(route->url);
			if(url_len>2){
				// it takes all folders and subfolders
				if(route->url[url_len-1]=='*' && route->url[url_len-2]=='/'){
					return route;
				}
			}
		}
	}

	return NULL;
}

void ZN_HttpRouteManager_Delete(ZN_HttpRouteManager * _this){
	ZN_HttpRouteManagerData *data=_this->data;
	for(unsigned i=0; i < data->routes->count; i++){
		ZN_HttpRoute_Delete(data->routes->items[i]);
	}

	ZN_List_Delete(data->routes);

	ZN_FREE(data);
	ZN_FREE(_this);
}

