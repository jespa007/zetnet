#pragma once

namespace zetnet{

class HttpServer;
class HttpResponse
{
	enum HTML_ERROR{
		HTML_ERROR_400=0,
		HTML_ERROR_404,
		HTML_ERROR_405,
		MAX_ERROR_TYPES
	};

	typedef struct{
		uint8_t * buffer;
		uint32_t size;
	}tBufferData;


	tBufferData data;

	string status;
	string mime;
	bool is_binary;


	struct tHtmlError{
		const char * title;
		const char * description;
	};

	static tHtmlError html_error [MAX_ERROR_TYPES];

	static HttpResponse * MakeFromFile(const string & file, const string & mime);

	static HttpResponse * MakeMethodNotAllowed(HttpServer * webserver);
	static HttpResponse * MakeNullRequest(HttpServer *webserver);
	static HttpResponse * MakePageNotFound(HttpServer *webserver);

public:

	static HttpResponse * From(HttpRequest * request, HttpServer  * webserver);
	static tBufferData GenerateError(int error_id, HttpServer * webserver);
	static HttpResponse * MakeFromString(const string & si, const string & mime);


	HttpResponse(
			//TCPsocket _dst_socket
			 const string  & status
			,const string & mime
			, bool is_binary
			,tBufferData  data
			);

	void Post(SOCKET dst_socket, HttpServer * webserver);

	~HttpResponse();

};

};
