#ifndef __ZN_HTTP_REQUEST__
#define __ZN_HTTP_REQUEST__

#define MAX_URL_LEN 	4096
#define MAX_CONTENT_LEN 100
#define MAX_HOST_LEN 	100
#define MAX_TYPE_LEN 	100
#define MAX_REFERER_LEN 100


typedef struct ZN_HttpRequest ZN_HttpRequest;

struct ZN_HttpRequest
{

	char  type[MAX_TYPE_LEN];
	char  URL[MAX_URL_LEN];
	char  host[MAX_HOST_LEN];
	char referer[MAX_REFERER_LEN];
	const char *mime;
	bool is_binary;
	char  content_type[MAX_CONTENT_LEN];
	ZN_List * param;

};


ZN_HttpRequest * ZN_HttpRequest_GetRequest(const char * request);
void		  ZN_HttpRequest_Delete(ZN_HttpRequest *http_request);

#endif
