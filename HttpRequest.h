#ifndef __HTTP_REQUEST__
#define __HTTP_REQUEST__

#define MAX_URL_LEN 	4096
#define MAX_CONTENT_LEN 100
#define MAX_HOST_LEN 	100
#define MAX_TYPE_LEN 	100
#define MAX_REFERER_LEN 100


typedef struct HttpRequest HttpRequest;

struct HttpRequest
{

	char  type[MAX_TYPE_LEN];
	char  URL[MAX_URL_LEN];
	char  host[MAX_HOST_LEN];
	char referer[MAX_REFERER_LEN];
	const char *mime;
	bool is_binary;
	char  content_type[MAX_CONTENT_LEN];
	ZNList * param;

};


HttpRequest * HttpRequest_GetRequest(const char * request);
void		  HttpRequest_Delete(HttpRequest *http_request);

#endif
