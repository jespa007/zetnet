#include "zetnet.h"

typedef struct{
	zn_list *routes;
}HttpRouteManagerData;

HttpRouteManager *HttpRouteManager_New(void){
	HttpRouteManager *http_route_manager=ZN_NEW(HttpRouteManager);
	HttpRouteManagerData *data=ZN_NEW(HttpRouteManagerData);
	data->routes=ZNList_New();

	http_route_manager->data=data;
	return http_route_manager;
}

void HttpRouteManager_AddRoute(HttpRouteManager * _this, const char *_url,const char *_path,HttpResponseCallback  *_on_request){
	HttpRouteManagerData *data=_this->data;
	HttpRoute *route=HttpRoute_New(_url,_path, _on_request);
	ZNList_Add(data->routes,route);
}

HttpRoute *HttpRouteManager_SearchRoute(HttpRouteManager * _this, const char *_url){
	HttpRouteManagerData *data=_this->data;

	// search the most equal _url
	for(unsigned i=0; i < data->routes->count; i++){
		HttpRoute *route=data->routes->items[i];
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

void HttpRouteManager_Delete(HttpRouteManager * _this){
	HttpRouteManagerData *data=_this->data;
	for(unsigned i=0; i < data->routes->count; i++){
		HttpRoute_Delete(data->routes->items[i]);
	}

	ZNList_Delete(data->routes);

	ZN_FREE(data);
	ZN_FREE(_this);
}

