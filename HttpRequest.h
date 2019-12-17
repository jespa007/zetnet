#ifndef __HTTP_REQUEST__
#define __HTTP_REQUEST__

typedef struct HttpRequest HttpRequest;

struct HttpRequest
{

	char  type[100];
	char  URL[4096];
	char  host[1024];
	char referer[1024];
	char mime[60];
	bool is_binary;
	char  content_type[512];
	ZNList * param;

};

HttpRequest * HttpRequest_GetRequest(const char * request);
void		  HttpRequest_Delete(HttpRequest *http_request);

#endif
