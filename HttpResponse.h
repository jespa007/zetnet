#ifndef __HTTP_RESPONSE__
#define __HTTP_RESPONSE__




typedef struct{
	uint8_t * buffer;
	uint32_t size;
}BufferData;


typedef struct HttpServer HttpServer;
typedef struct HttpResponse HttpResponse;

struct HttpResponse
{
	BufferData data;

	char *status;
	char *mime;
	bool is_binary;

};

HttpResponse *HttpResponse_New(
		 const char * status
		,const char * mime
		, bool is_binary
		,BufferData  data
		);

void HttpResponse_Post(HttpResponse * http_response,SOCKET dst_socket, HttpServer * http_server);
void HttpResponse_Delete(HttpResponse * http_response);



#endif
