#ifndef __HTTP_RESPONSE__
#define __HTTP_RESPONSE__

typedef struct{
	uint8_t * 	buffer;
	size_t 		size;
}BufferData;

typedef struct HttpServer HttpServer;
typedef struct HttpResponse HttpResponse;

struct HttpResponse
{
	BufferData data;

	const char *status;
	const char *mime;
	bool is_binary;

};

typedef struct{
	HttpResponse * (*callback_function)(HttpServer * _http_server, HttpParamValue  * _param, size_t _param_len,void *_user_data);
	void *user_data;
}HttpResponseCallback;

HttpResponse *HttpResponse_New(
		 const char * status
		,const char * mime
		, bool is_binary
		,BufferData  data
		);


HttpResponse * 	HttpResponse_MakePageNotFound(HttpServer * _http_server);
HttpResponse *  HttpResponse_MakeFromString(const char * si, const char * mime);
HttpResponse * 	HttpResponse_MakeMethodNotAllowed(HttpServer * _http_server);
HttpResponse *	HttpResponse_FromFile(HttpRequest * _request,const char *_filename_with_path);
void 			HttpResponse_Send(HttpResponse * http_response,SOCKET dst_socket, HttpServer * http_server);
void 			HttpResponse_Delete(HttpResponse * http_response);



#endif
