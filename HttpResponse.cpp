#include "HttpServer.h"

HttpResponse::tHtmlError HttpResponse::html_error [MAX_ERROR_TYPES]={
	 {"400 - Bad Request","Please check the above url is valid"}
	,{"404 - Not found","We could not found the above page on our server"}
	,{"405 - Method not allowed","The method specified in the Request Line is not allowed for the resource identified by the request. Please ensure that you have the proper MIME type set up for the resource you are requesting<br><br>. Please contact the server's administrator if this problem persists."}
};


ByteBuffer * HttpResponse::GenerateError(int error_id, HttpServer * webserver)
{

	tHtmlError error;
	string int_error = CIO_Utils::intToString(error_id);

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


	return new ByteBuffer((uint8_t *)str.c_str(), str.size());

}

HttpResponse::HttpResponse( const string & status,const string & mime,bool is_binary, ByteBuffer *data) {
		//this->dst_socket = _dst_socket;
		this->data = data;
		this->status = status;
		this->mime = mime;
		this->is_binary = is_binary;
}

HttpResponse::~HttpResponse(){
	// data is ByteBuffer allocated from Response::From
	delete this->data;
}
/*
HttpResponse * HttpResponse::MakeFromFile(const string & file, const string & mime)
{
	ByteBuffer *buffer = CIO_Utils::readFile(file);
	return new HttpResponse("200 OK", mime, buffer);
}*/

HttpResponse * HttpResponse::MakeFromString(const string & str, const string & mime)
{
	ByteBuffer * buffer = new ByteBuffer((uint8_t *)str.c_str(),str.size());

	return new HttpResponse("200 OK", mime, false, buffer);
}

HttpResponse * HttpResponse::MakeMethodNotAllowed(HttpServer * webserver)
{
	return new HttpResponse("405 Method not allowed", "html/text", false, HttpResponse::GenerateError(HTML_ERROR_405, webserver));
}

HttpResponse * HttpResponse::MakeNullRequest(HttpServer * webserver)
{
	return new HttpResponse("400 Bad Request", "html/text",false,  HttpResponse::GenerateError(HTML_ERROR_400, webserver));
}

HttpResponse * HttpResponse::MakePageNotFound(HttpServer * webserver)
{
	return new HttpResponse("404 Bad Request", "html/text", false, HttpResponse::GenerateError(HTML_ERROR_404, webserver));
}

string  getFileName(const string & _filename) {
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

	string file="";
	string path="";

	//FileInfo fi = null;
	bool ok = false;

	if(request->Type== "GET"){

		string path_url = CUri::unescape(request->URL);
#ifdef WIN32
		path_url = CIO_Utils::replace(path_url, '/','\\');//CUri::unescape(request->URL)
#endif

		string filename_with_path = /*CIO_Utils::getCwd()
		    +"\\"*/
			  webserver->WEB_DIR
			+ path_url;

		/*string filename_with_path=_filename_with_path;
		if(_filename_with_path[0]=='/'){
			filename_with_path = _filename_with_path.substr (1,_filename_with_path.size());
		}*/

		//fi = new FileInfo(file);
		ok = false;

		file = CIO_Utils::getFileName(filename_with_path);
		path = CIO_Utils::getDirectory(filename_with_path);

#ifdef __DEBUG__
		printf("try_file:%s request:%s\n",filename_with_path.c_str(),request->URL.c_str());
#endif


		if (CIO_Utils::fileExists(filename_with_path)/* && fi.Extension.Contains(".")*/)
		{
#ifdef __DEBUG__
			printf("file exists!!!\n");
#endif
			ok = true;
		}
		else
		{

			//DirectoryInfo di = new DirectoryInfo(fi+"/");

			if (!CIO_Utils::isDirectory(path)){
				return MakePageNotFound(webserver);
			}

			vector<string> list_file = CIO_Utils::getFiles(path);//,"*.html",false);

			//FileInfo [] files = di.GetFiles();
			for(unsigned f=0; f < list_file.size() && !ok; f++){ //foreach(FileInfo ff in files){
				//String n = ff.Name;
				string n = CIO_Utils::getFileName(list_file[f]);

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

			return new HttpResponse("200 OK", request->Mime, request->IsBinary, CIO_Utils::readFile(filename_to_load, false) );

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
		send_message+="Content-Length: " + CIO_Utils::intToString(this->data->length) + "\r\n";

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


		buffer=(char *)malloc(send_message.size()+this->data->length+2+1);
		memset(buffer,0,send_message.size()+this->data->length+2+1);
		memcpy(buffer										,send_message.c_str(),send_message.size());
		memcpy(buffer+send_message.size()				    ,this->data->data_buffer,this->data->length);
		memcpy(buffer+send_message.size()+this->data->length,"\r\n",2);

		CNetTcp::putMsg(dst_socket,(uint8_t *)buffer,send_message.size()+this->data->length+2);

		free(buffer);
		//SDLNet_TCP_Send(dst_socket,this->data->data_buffer,this->data->length);

		//if(txt_message){
			// sends carry end...
		//SDLNet_TCP_Send(dst_socket,"\r\n",2);
		//}

	}
	else // send error message
	{
		string error = (char *)data->data_buffer;
		error +="\r\n";
		send_message+="Content-Length: " + CIO_Utils::intToString(error.size()) + "\r\n\r\n";
		CNetTcp::putMsg(dst_socket,(uint8_t *)send_message.c_str(),send_message.size());
		CNetTcp::putMsg(dst_socket,(uint8_t *)error.c_str(),error.size());

	}



}
