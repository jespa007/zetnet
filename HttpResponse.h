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

		static HttpResponse * makeFromFile(const std::string & file, const std::string & mime);

		static HttpResponse * makeMethodNotAllowed(HttpServer * webserver);
		static HttpResponse * makeNullRequest(HttpServer *webserver);
		static HttpResponse * makePageNotFound(HttpServer *webserver);

	public:

		static HttpResponse * from(HttpRequest * request, HttpServer  * webserver);
		static BufferData generateError(int error_id, HttpServer * webserver);
		static HttpResponse * makeFromString(const std::string & si, const std::string & mime);


		HttpResponse(
				 const std::string  & status
				,const std::string & mime
				, bool is_binary
				,BufferData  data
				);

		void post(SOCKET dst_socket, HttpServer * webserver);

		~HttpResponse();

	};

};
