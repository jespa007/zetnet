#include "zetnet.h"

namespace zetnet{

	HttpResponse::tHtmlError HttpResponse::html_error [MAX_ERROR_TYPES]={
		 {"400 - Bad Request","Please check the above url is valid"}
		,{"404 - Not found","We could not found the above page on our server"}
		,{"405 - Method not allowed","The method specified in the Request Line is not allowed for the resource identified by the request. Please ensure that you have the proper MIME type set up for the resource you are requesting<br><br>. Please contact the server's administrator if this problem persists."}
	};


	HttpResponse::tBufferData  HttpResponse::generateError(int error_id, HttpServer * webserver)
	{

		tHtmlError error;
		string int_error = string::to_string(error_id);
		BufferData data;

		if(error_id < MAX_ERROR_TYPES){
			error = html_error[error_id];
		}else{
			error.title=(const char *)int_error.c_str();
			error.description="not implemented";
		}

		string str=
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


		return data;//new ((uint8_t *)str.c_str(), str.size());

	}

	HttpResponse::HttpResponse( const std::string & status,const std::string & mime,bool is_binary, BufferData data) {
			//this->dst_socket = _dst_socket;
			this->data = data;
			this->status = status;
			this->mime = mime;
			this->is_binary = is_binary;

	}

	HttpResponse::~HttpResponse(){
		// data is ByteBuffer allocated from Response::From
		free(this->data.buffer);
	}
	/*
	HttpResponse * HttpResponse::MakeFromFile(const string & file, const string & mime)
	{
		ByteBuffer *buffer = CZetNetUtils::readFile(file);
		return new HttpResponse("200 OK", mime, buffer);
	}*/

	HttpResponse * HttpResponse::makeFromString(const std::string & str, const std::string & mime)
	{
		BufferData data;
		data.size=str.size();
		data.buffer=(uint8_t *)malloc(data.size);
		memset(data.buffer,0,data.size);
		strcpy((char *)data.buffer,(char *)str.c_str());


		return new HttpResponse("200 OK", mime, false, data);
	}

	HttpResponse * HttpResponse::makeMethodNotAllowed(HttpServer * webserver)
	{
		return new HttpResponse("405 Method not allowed", "html/text", false, HttpResponse::generateError(HTML_ERROR_405, webserver));
	}

	HttpResponse * HttpResponse::makeNullRequest(HttpServer * webserver)
	{
		return new HttpResponse("400 Bad Request", "html/text",false,  HttpResponse::generateError(HTML_ERROR_400, webserver));
	}

	HttpResponse * HttpResponse::makePageNotFound(HttpServer * webserver)
	{
		return new HttpResponse("404 Bad Request", "html/text", false, HttpResponse::generateError(HTML_ERROR_404, webserver));
	}

	string  getFileName(const std::string & _filename) {
	  size_t found;
	  string ss=_filename;

	  //JEME_MEM_DISABLE_REGISTER_MEMORYLEAKS;

	  found=_filename.find_last_of("/\\");
	  if((int)found != -1)
		  ss= _filename.substr(found+1);

	  //JEME_MEM_ENABLE_REGISTER_MEMORYLEAKS;
	  return ss;
	}

	HttpResponse *HttpResponse::From(HttpRequest * request, HttpServer * webserver) {
		if (request == NULL)
			return MakeNullRequest(webserver);

		std::string file="";
		std::string path="";

		//FileInfo fi = null;
		bool ok = false;

		if(request->Type== "GET"){

			string path_url = CUri::unescape(request->URL);
	#ifdef WIN32
			path_url = string::replace(path_url, '/','\\');//CUri::unescape(request->URL)
	#endif

			string filename_with_path = /*CZetNetUtils::getCwd()
				+"\\"*/
				  webserver->WEB_DIR
				+ path_url;

			/*string filename_with_path=_filename_with_path;
			if(_filename_with_path[0]=='/'){
				filename_with_path = _filename_with_path.substr (1,_filename_with_path.size());
			}*/

			//fi = new FileInfo(file);
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

				//DirectoryInfo di = new DirectoryInfo(fi+"/");

				if (!io::is_directory(path)){
					return MakePageNotFound(webserver);
				}

				std::vector<std::string> list_file = io::get_files(path);//,"*.html",false);

				//FileInfo [] files = di.GetFiles();
				for(unsigned f=0; f < list_file.size() && !ok; f++){ //foreach(FileInfo ff in files){
					//String n = ff.Name;
					string n = path::get_filename(list_file[f]);

	#ifdef __DEBUG__
					printf("try_file2:%s\n",n.c_str());
	#endif

					if(n == "index.html"){
						file=n;
						ok = true;
					}

			/*		if(
						n.find("default.html")
						|| n.find("default.htm")
	#if DEBUG
						|| n.find("index_debug.html") // get this first for debug purpouses...
	#endif
						|| n.find("index.htm")
						|| n.find("index.html")
					  )
					{
						fi=ff;
						ok = true;
					}*/
				}
			}

			if (ok)
			{
	#ifdef _WIN32
	#define SEPARATOR_DIR "\\"
	#else
	#define SEPARATOR_DIR "/"
	#endif

			string filename_to_load = path+SEPARATOR_DIR+file;

			BufferData data;

			data.buffer=io::read_file(filename_to_load,data.size, false);



				return new HttpResponse("200 OK", request->Mime, request->IsBinary, data);

				//return MakeFromFile(path+"\\"+file, request->Mime);
			}

		}
		else{
			return MakeMethodNotAllowed(webserver);
		}

		return MakePageNotFound(webserver);
	}

	void HttpResponse::Post(SOCKET dst_socket, HttpServer * webserver) //, const string & response_action)
	{
		string send_message="";

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
			//SDLNet_TCP_Send(dst_socket,this->data->data_buffer,this->data->length);

			//if(txt_message){
				// sends carry end...
			//SDLNet_TCP_Send(dst_socket,"\r\n",2);
			//}

		}
		else // send error message
		{
			string error = (char *)data.buffer;
			error +="\r\n";
			send_message+="Content-Length: " + string::to_string(error.size()) + "\r\n\r\n";
			CServer::putMsg(dst_socket,(uint8_t *)send_message.c_str(),send_message.size());
			CServer::putMsg(dst_socket,(uint8_t *)error.c_str(),error.size());

		}



	}
};
