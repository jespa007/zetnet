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

HttpResponse *HttpResponse_New(
		 const char * status
		,const char * mime
		, bool is_binary
		,BufferData  data
		);

HttpResponse *  HttpResponse_MakeFromString(const char * si, const char * mime);
HttpResponse *	HttpResponse_From(HttpRequest * request, HttpServer * http_server);
void 			HttpResponse_Post(HttpResponse * http_response,SOCKET dst_socket, HttpServer * http_server);
void 			HttpResponse_Delete(HttpResponse * http_response);



#endif
