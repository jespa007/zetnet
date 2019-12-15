#ifndef __HTTP_RESPONSE__
#define __HTTP_RESPONSE__

class CHttpResponse
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
	}BufferData;


	BufferData data;

	std::string status;
	std::string mime;
	bool is_binary;


	struct tHtmlError{
		const char * title;
		const char * description;
	};

	static tHtmlError html_error [MAX_ERROR_TYPES];

	static CHttpResponse * makeFromFile(const std::string & file, const std::string & mime);

	static CHttpResponse * makeMethodNotAllowed(CHttpServer * webserver);
	static CHttpResponse * makeNullRequest(CHttpServer *webserver);
	static CHttpResponse * makePageNotFound(CHttpServer *webserver);

public:

	static CHttpResponse * from(CHttpRequest * request, CHttpServer  * webserver);
	static BufferData generateError(int error_id, CHttpServer * webserver);
	static CHttpResponse * makeFromString(const std::string & si, const std::string & mime);


	CHttpResponse(
			 const std::string  & status
			,const std::string & mime
			, bool is_binary
			,BufferData  data
			);

	void post(SOCKET dst_socket, CHttpServer * webserver);

	~CHttpResponse();

};


#endif
