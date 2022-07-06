#include "zetnet.h"

HttpRoute *HttpRoute_New(const char *_url, const char *_path, HttpRouteOnRequest _on_request){
	HttpRoute *route=ZN_MALLOC(HttpRoute);
	route->url=malloc(strlen(_url)+1);
	route->path=malloc(strlen(_path)+1);

	strcpy(route->url,_url);
	strcpy(route->path,_path);
	route->on_request=_on_request;

	return route;
}

void HttpRoute_Delete(HttpRoute * _this){
	ZN_FREE(_this->url);
	ZN_FREE(_this->path);
	ZN_FREE(_this);
}
