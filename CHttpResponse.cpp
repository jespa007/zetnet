#include "zetnet.h"

namespace zetnet{

	CHttpResponse::tHtmlError CHttpResponse::html_error [MAX_ERROR_TYPES]={
		 {"400 - Bad Request","Please check the above url is valid"}
		,{"404 - Not found","We could not found the above page on our server"}
		,{"405 - Method not allowed","The method specified in the Request Line is not allowed for the resource identified by the request. Please ensure that you have the proper MIME type set up for the resource you are requesting<br><br>. Please contact the server's administrator if this problem persists."}
	};


	CHttpResponse::BufferData  CHttpResponse::generateError(int error_id, CHttpServer * webserver)
	{

		tHtmlError error;
		std::string int_error = string::to_string(error_id);
		BufferData data;

		if(error_id < MAX_ERROR_TYPES){
			error = html_error[error_id];
		}else{
			error.title=(const char *)int_error.c_str();
			error.description="not implemented";
		}

		std::string str=
				"<html>"
					"<body>"
						"<center>"
							"<div>"
								"<div style=\"float:left;width:50%;text-align:right;margin-top:30\">"
									"<img  src=\"" + webserver->LOGO_BASE64+ "\"/>"
								"</div>"
								"<div style=\"float:right;width:48%;text-align:left;font-family:Arial\">"
									"<h1>" + error.title + "</h1>"
									"<h3>"+  error.description+"</h3>"
								"</div>"
							"</div>"
						"</center>"
					"</body>"
				"</html>";


		data.size = str.size();
		data.buffer = (uint8_t *)malloc(data.size);
		memset(data.buffer,0,data.size);
		strcpy((char *)data.buffer,(char *)str.c_str());


		return data;

	}

	CHttpResponse::CHttpResponse( const std::string & status,const std::string & mime,bool is_binary, BufferData data) {
			this->data = data;
			this->status = status;
			this->mime = mime;
			this->is_binary = is_binary;

	}


	CHttpResponse * CHttpResponse::makeFromString(const std::string & str, const std::string & mime)
	{
		BufferData data;
		data.size=str.size();
		data.buffer=(uint8_t *)malloc(data.size);
		memset(data.buffer,0,data.size);
		strcpy((char *)data.buffer,(char *)str.c_str());


		return new CHttpResponse("200 OK", mime, false, data);
	}

	CHttpResponse * CHttpResponse::makeMethodNotAllowed(CHttpServer * webserver)
	{
		return new CHttpResponse("405 Method not allowed", "html/text", false, CHttpResponse::generateError(HTML_ERROR_405, webserver));
	}

	CHttpResponse * CHttpResponse::makeNullRequest(CHttpServer * webserver)
	{
		return new CHttpResponse("400 Bad Request", "html/text",false,  CHttpResponse::generateError(HTML_ERROR_400, webserver));
	}

	CHttpResponse * CHttpResponse::makePageNotFound(CHttpServer * webserver)
	{
		return new CHttpResponse("404 Bad Request", "html/text", false, CHttpResponse::generateError(HTML_ERROR_404, webserver));
	}

	CHttpResponse *CHttpResponse::from(CHttpRequest * request, CHttpServer * webserver) {
		if (request == NULL)
			return makeNullRequest(webserver);

		std::string file="";
		std::string path="";

		bool ok = false;

		if(request->type== "GET"){

			std::string path_url = url::unescape(request->URL);
	#ifdef WIN32
			path_url = string::replace(path_url, '/','\\');//CUri::unescape(request->URL)
	#endif

			std::string filename_with_path = /*CZetNetUtils::getCwd()
				+"\\"*/
				  webserver->WEB_DIR
				+ path_url;

			ok = false;

			file = path::get_filename(filename_with_path);
			path = path::get_directory(filename_with_path);

	#ifdef __DEBUG__
			printf("try_file:%s request:%s\n",filename_with_path.c_str(),request->URL.c_str());
	#endif


			if (io::file_exists(filename_with_path)/* && fi.Extension.Contains(".")*/)
			{
	#ifdef __DEBUG__
				printf("file exists!!!\n");
	#endif
				ok = true;
			}
			else
			{
				if (!io::is_directory(path)){
					return makePageNotFound(webserver);
				}

				std::vector<std::string> list_file = io::get_files(path);//,"*.html",false);

				for(unsigned f=0; f < list_file.size() && !ok; f++){ //foreach(FileInfo ff in files){
					//String n = ff.Name;
					std::string n = path::get_filename(list_file[f]);

	#ifdef __DEBUG__
					printf("try_file2:%s\n",n.c_str());
	#endif

					if(n == "index.html"){
						file=n;
						ok = true;
					}
				}
			}

			if (ok)
			{
	#ifdef _WIN32
	#define SEPARATOR_DIR "\\"
	#else
	#define SEPARATOR_DIR "/"
	#endif

			std::string filename_to_load = path+SEPARATOR_DIR+file;

			BufferData data;

			data.buffer=io::read_file(filename_to_load,data.size, false);

				return new CHttpResponse("200 OK", request->mime, request->is_binary, data);
			}

		}
		else{
			return makeMethodNotAllowed(webserver);
		}

		return makePageNotFound(webserver);
	}

	void CHttpResponse::post(SOCKET dst_socket, CHttpServer * webserver) //, const string & response_action)
	{
		std::string send_message="";

		char *buffer;

		send_message= "HTTP/1.1 " + this->status + "\r\n";
		send_message+="Server: " + webserver->NAME + "\r\n";

		if (this->status == "200 OK") // send response content
		{
			send_message+="Content-Type: " + mime + "\r\n";
			send_message+="Accept-Ranges: bytes\r\n";
			send_message+="Connection: Keep-Alive\r\n";
			send_message+="Content-Length: " + string::to_string(this->data.size) + "\r\n";

			if(is_binary){
			/*if (   mime == "application/pdf"
				|| mime == "image/png"
				|| mime == "application/octet-stream"
					) // binary ...
			{*/
				send_message+="Content-Transfer-Encoding: binary\r\n";
				//txt_message=false;
			}

			send_message+="\r\n";

	#ifdef __DEBUG__
			printf("POST:\n%s\n",send_message.c_str());
	#endif

			int total_size = send_message.size()+this->data.size+2;

			buffer=(char *)malloc(total_size);
			memset(buffer,0,total_size);
			memcpy(buffer										,send_message.c_str(),send_message.size());
			memcpy(buffer+send_message.size()				    ,this->data.buffer,this->data.size);
			memcpy(buffer+send_message.size()+this->data.size,"\r\n",2);

			CServer::putMsg(dst_socket,(uint8_t *)buffer,total_size);

			free(buffer);
		}
		else // send error message
		{
			std::string error = (char *)data.buffer;
			error +="\r\n";
			send_message+="Content-Length: " + string::to_string(error.size()) + "\r\n\r\n";
			CServer::putMsg(dst_socket,(uint8_t *)send_message.c_str(),send_message.size());
			CServer::putMsg(dst_socket,(uint8_t *)error.c_str(),error.size());

		}

	}

	CHttpResponse::~CHttpResponse(){
		free(this->data.buffer);
	}
};
