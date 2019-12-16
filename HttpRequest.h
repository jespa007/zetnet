#ifndef __HTTP_REQUEST__
#define __HTTP_REQUEST__

typedef struct ParamValue ParamValue;

struct ParamValue
{
	char name[256];
	char value[256];
};

ParamValue * ParamValue_New(const char * _name, const char * _value);

//--

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




protected:

private:



};

static CHttpRequest * getRequest(const std::string & request);

#endif
