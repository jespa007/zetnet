#ifndef __ZN_HTTP_REQUEST__
#define __ZN_HTTP_REQUEST__

#define ZN_MAX_URL_LEN 	4096
#define ZN_MAX_CONTENT_LEN 100
#define ZN_MAX_HOST_LEN 	100
#define ZN_MAX_TYPE_LEN 	100
#define ZN_MAX_REFERER_LEN 100


typedef struct ZN_HttpRequest ZN_HttpRequest;

struct ZN_HttpRequest
{

	char  type[ZN_MAX_TYPE_LEN];
	char  URL[ZN_MAX_URL_LEN];
	char  host[ZN_MAX_HOST_LEN];
	char referer[ZN_MAX_REFERER_LEN];
	const char *mime;
	bool is_binary;
	char  content_type[ZN_MAX_CONTENT_LEN];
	ZN_List * param;

};


ZN_HttpRequest * ZN_HttpRequest_GetRequest(const char * request);
void		  ZN_HttpRequest_Delete(ZN_HttpRequest *http_request);

#endif
