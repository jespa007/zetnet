#ifndef __ZN_HTTP_RESPONSE__
#define __ZN_HTTP_RESPONSE__

typedef struct{
	uint8_t * 	buffer;
	size_t 		size;
}BufferData;

typedef struct ZN_HttpServer ZN_HttpServer;
typedef struct ZN_HttpResponse ZN_HttpResponse;

struct ZN_HttpResponse
{
	BufferData data;

	const char *status;
	const char *mime;
	bool is_binary;

};

typedef struct{
	ZN_HttpResponse * (*callback_function)(ZN_HttpServer * _http_server, ZN_HttpParamValue  * _param, size_t _param_len,void *_user_data);
	void *user_data;
}ZN_HttpResponseCallback;

ZN_HttpResponse *ZN_HttpResponse_New(
		 const char * status
		,const char * mime
		, bool is_binary
		,BufferData  data
		);


ZN_HttpResponse * 	ZN_HttpResponse_MakePageNotFound(ZN_HttpServer * _http_server);
ZN_HttpResponse *  ZN_HttpResponse_MakeFromString(const char * si, const char * mime);
ZN_HttpResponse * 	ZN_HttpResponse_MakeMethodNotAllowed(ZN_HttpServer * _http_server);
ZN_HttpResponse *	ZN_HttpResponse_FromFile(ZN_HttpRequest * _request,const char *_filename_with_path);
void 			ZN_HttpResponse_Send(ZN_HttpResponse * http_response,SOCKET dst_socket, ZN_HttpServer * http_server);
void 			ZN_HttpResponse_Delete(ZN_HttpResponse * http_response);



#endif
