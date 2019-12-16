#ifndef __HTTP_RESPONSE__
#define __HTTP_RESPONSE__




typedef struct{
	uint8_t * buffer;
	uint32_t size;
}BufferData;

typedef struct{
	const char * title;
	const char * description;
}HtmlError;


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

void HttpResponse_Post(SOCKET dst_socket, CHttpServer * webserver);

void HttpResponse_Delete(HttpResponse * http_response);



#endif
