#include "zetnet.h"


HttpHandleClient * HttpHandleClient_New(SocketClient * _socket_client, HttpServer * _http_server, uint8_t *_rcv_data, uint32_t _rcv_len)
{
	HttpHandleClient * _http_handle_client = ZN_NEW(HttpHandleClient);
	_http_handle_client->socket_client = _socket_client;
	_http_handle_client->http_server = _http_server;

	_http_handle_client->rcv_buffer_data=_rcv_data;
	_http_handle_client->rcv_buffer_data_len=_rcv_len;

	return _http_handle_client;
}

void HttpHandleClient_DoHandle(HttpHandleClient *_http_handle_client)
{
	//std::string msg = "";
	HttpRequest *http_request = NULL;

#ifdef __ZN_VERBOSE__
	LOG_DEBUG("received:%s\nreceived_len:%i\n",_http_handle_client->rcv_buffer_data,_http_handle_client->rcv_buffer_data_len);
#endif

	http_request = HttpRequest_GetRequest((const char *)_http_handle_client->rcv_buffer_data);

	char filename_with_path[MAX_PATH]={0};
	char *unescaped_url=NULL;
	char path_url[MAX_PATH]={0};
	HttpRoute *route_found=NULL;
	HttpResponse *http_response =NULL;

	unescaped_url=zn_url_unescape(http_request->URL);
	sprintf(path_url,"%s",unescaped_url);

#ifdef WIN32
	zn_str_replace_by_char(path_url, '/','\\');//CUri::unescape(request->URL)
#endif

	if(strcmp(http_request->type,"GET")==0){
		// route GET
		route_found=HttpServer_SearchGetRoute(_http_handle_client->http_server,unescaped_url);

	}else if(strcmp(http_request->type,"POST")==0){
		// route POST
		route_found=HttpServer_SearchPostRoute(_http_handle_client->http_server,unescaped_url);

	}else{
		// request type not allowed
		http_response=HttpResponse_MakeMethodNotAllowed(_http_handle_client->http_server);
	}

	// deallocate unescaped_url, not used anymore
	ZN_FREE(unescaped_url);

	if(route_found != NULL){

		if(route_found->on_request.callback_function!=NULL){
			void *param=NULL;
			size_t param_len=0;
			HttpResponseCallback cf=_http_handle_client->http_server->on_request;

			if(http_request->param!=NULL){
				param=http_request->param->items;
				param_len=http_request->param->count;
			}

			http_response=route_found->on_request.callback_function(_http_handle_client->http_server,(HttpParamValue *)param,param_len,cf.user_data);
		}else{
			// try send a file
			// substract url concat to filename_with_path
			size_t len=strlen(route_found->url);
			int rest=0;
			if(route_found->url[len-1]=='*' && route_found->url[len-2]=='/'){
				rest=1;
			}

			sprintf(filename_with_path,"%s/",route_found->path);
			strcat(filename_with_path,path_url+(len-rest));

			http_response = HttpResponse_FromFile(http_request, filename_with_path);
		}

	}else{
		// send file by default
		// webdir by default
		sprintf(filename_with_path,"%s%s",_http_handle_client->http_server->web_dir,path_url);
		http_response = HttpResponse_FromFile(http_request, filename_with_path);
	}

	if(http_response == NULL){
		http_response=HttpResponse_MakePageNotFound(_http_handle_client->http_server);
	}

	HttpResponse_Send(http_response,_http_handle_client->socket_client->socket, _http_handle_client->http_server);
	HttpResponse_Delete(http_response);
	HttpRequest_Delete(http_request);
	HttpHandleClient_Delete(_http_handle_client);

}

void HttpHandleClient_Delete(HttpHandleClient *_http_handle_client){
	if(_http_handle_client!=NULL){
		ZN_FREE(_http_handle_client);
	}
}


