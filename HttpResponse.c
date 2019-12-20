#include "zetnet.h"

typedef struct{
	const char * title;
	const char * description;
}HtmlError;

typedef enum{
	HTML_ERROR_400=0,
	HTML_ERROR_404,
	HTML_ERROR_405,
	MAX_ERROR_TYPES
}HTML_ERROR;

HtmlError html_error [MAX_ERROR_TYPES]={
	 {"400 - Bad Request","Please check the above url is valid"}
	,{"404 - Not found","We could not found the above page on our server"}
	,{"405 - Method not allowed","The method specified in the Request Line is not allowed for the resource identified by the request. Please ensure that you have the proper MIME type set up for the resource you are requesting<br><br>. Please contact the server's administrator if this problem persists."}
};

// STATIC
HttpResponse * HttpResponse_MakeFromFile(const char * file, const char * mime){
	return NULL;
}

HttpResponse * HttpResponse_MakeMethodNotAllowed(HttpServer * webserver){
	return NULL;
}


HttpResponse * HttpResponse_MakeFromString(const char * si, const char * mime){
	return NULL;
}



BufferData  HttpResponse_GenerateError(int error_id, HttpServer * http_server)
{

	HtmlError error;
	char int_error[100];
	BufferData data;
	char str[2096];

	sprintf(int_error,"%i",error_id);

	if(error_id < MAX_ERROR_TYPES){
		error = html_error[error_id];
	}else{
		error.title=int_error;
		error.description="not implemented";
	}


	sprintf(str,
			"<html>"
				"<body>"
					"<center>"
						"<div>"
							"<div style=\"float:left;width:50%%;text-align:right;margin-top:30\">"
								"<img  src=\"%s\"/>"
							"</div>"
							"<div style=\"float:right;width:48%%;text-align:left;font-family:Arial\">"
								"<h1>%s</h1>"
								"<h3>%s</h3>"
							"</div>"
						"</div>"
					"</center>"
				"</body>"
			"</html>"
	,http_server->LOGO_BASE64
	,error.title
	,error.description
	);


	data.size = strlen(str);//.size();
	data.buffer = (uint8_t *)malloc(data.size+1); // +1 for end str
	memset(data.buffer,0,data.size);
	strcpy((char *)data.buffer,(char *)str);


	return data;

}

HttpResponse * HttpResponse_New( const char * status,const char * mime,bool is_binary, BufferData data) {
	HttpResponse * http_response = malloc(sizeof(HttpResponse));
	http_response->data = data;
	http_response->status = status;
	http_response->mime = mime;
	http_response->is_binary = is_binary;

	return http_response;
}


HttpResponse * HttpResponse_makeFromString(const char * str, const char * mime)
{
	BufferData data;
	data.size=strlen(str);
	data.buffer=(uint8_t *)malloc(data.size+1);
	memset(data.buffer,0,data.size);
	strcpy((char *)data.buffer,(char *)str);


	return HttpResponse_New("200 OK", mime, false, data);
}

HttpResponse * HttpResponse_makeMethodNotAllowed(HttpServer * webserver)
{
	return HttpResponse_New("405 Method not allowed", "html/text", false, HttpResponse_GenerateError(HTML_ERROR_405, webserver));
}

HttpResponse * HttpResponse_MakeNullRequest(HttpServer * webserver)
{
	return HttpResponse_New("400 Bad Request", "html/text",false,  HttpResponse_GenerateError(HTML_ERROR_400, webserver));
}

HttpResponse * HttpResponse_MakePageNotFound(HttpServer * webserver)
{
	return HttpResponse_New("404 Bad Request", "html/text", false, HttpResponse_GenerateError(HTML_ERROR_404, webserver));
}

HttpResponse *HttpResponse_From(HttpRequest * request, HttpServer * webserver) {
	char filename_with_path[4096]={0};
	char path_url[4096]={0};
	const char *file="";
	const char *path="";
	ZNList * list_file=NULL;
	bool ok = false;

	if (request == NULL){
		return HttpResponse_MakeNullRequest(webserver);
	}

	if(strcmp(request->type,"GET")==0){

		sprintf(path_url,"%s",ZNUrl_Unescape(request->URL));
#ifdef WIN32
		ZNString_ReplaceChar(path_url, '/','\\');//CUri::unescape(request->URL)
#endif

		sprintf(filename_with_path,
				"%s%s",
			  webserver->WEB_DIR,
			path_url);

		ok = false;

		file = ZNPath_GetFilename(filename_with_path);
		path = ZNPath_GetDirectory(filename_with_path);

#ifdef __DEBUG__
		printf("try_file:%s request:%s\n",filename_with_path,request->URL);
#endif


		if (ZNIO_FileExists(filename_with_path)/* && fi.Extension.Contains(".")*/)
		{
#ifdef __DEBUG__
			printf("file exists!!!\n");
#endif
			ok = true;
		}
		else
		{
			if (!ZNIO_IsDirectory(path)){
				return HttpResponse_MakePageNotFound(webserver);
			}

			list_file = ZNIO_GetFiles(path,NULL,NULL);//,"*.html",false);

			for(unsigned f=0; f < list_file->count && !ok; f++){ //foreach(FileInfo ff in files){
				//String n = ff.Name;
				const char * n = ZNPath_GetFilename(list_file->items[f]);

#ifdef __DEBUG__
				printf("try_file2:%s\n",n);
#endif

				if(strcmp(n,"index.html")==0){
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

			char filename_to_load[4096];
			sprintf(filename_to_load,"%s%s%s",path,SEPARATOR_DIR,file);

			BufferData data;

			data.buffer=ZNIO_ReadFile(filename_to_load,&data.size);

			return HttpResponse_New("200 OK", request->mime, request->is_binary, data);
		}

	}
	else{
		return HttpResponse_MakeMethodNotAllowed(webserver);
	}

	return HttpResponse_MakePageNotFound(webserver);
}

void HttpResponse_Post(HttpResponse *http_response,SOCKET dst_socket, HttpServer * http_server) //, const string & response_action)
{
	char send_message[4096]={0};
	size_t send_message_len=0;
	char *buffer;
	int total_size=0;

	strcat(send_message,"HTTP/1.1 ");
	strcat(send_message,http_response->status);
	strcat(send_message,"\n");
	strcat(send_message,"Server: ");
	strcat(send_message,http_server->NAME);
	strcat(send_message,"\n");

	if (strcmp(http_response->status,"200 OK")==0) // send response content
	{
		strcat(send_message,"Content-Type: ");
		strcat(send_message, http_response->mime);
		strcat(send_message,"\n");
		strcat(send_message,"Accept-Ranges: bytes\n");
		strcat(send_message,"Connection: Keep-Alive\n");
		strcat(send_message,"Content-Length: ");
		strcat(send_message,ZNString_IntToString(http_response->data.size));

		if(http_response->is_binary){
			strcat(send_message,"Content-Transfer-Encoding: binary");
		}

		strcat(send_message,"\n\n");

#ifdef __DEBUG__
		printf("POST:\n%s\n",send_message);
#endif

		send_message_len=strlen(send_message);
		total_size = strlen(send_message)+http_response->data.size+2;

		buffer=(char *)malloc(total_size);
		memset(buffer,0,total_size);
		memcpy(buffer										,send_message,send_message_len);
		memcpy(buffer+send_message_len				    ,http_response->data.buffer,http_response->data.size);
		memcpy(buffer+send_message_len+http_response->data.size,"\n",2);

		TcpServer_SendBytes(dst_socket,(uint8_t *)buffer,total_size);

		free(buffer);
	}
	else // send error message
	{

		char error[4096]={0};
		sprintf("%s\n"
				"Content-Length: %s"
				"\n"
				"\n"
				,(char *)http_response->data.buffer
				,ZNString_IntToString(strlen(error))
		);


		TcpServer_SendBytes(dst_socket,(uint8_t *)send_message,strlen(send_message));
		TcpServer_SendBytes(dst_socket,(uint8_t *)error,strlen(error));

	}

}

void HttpResponse_Delete(HttpResponse * http_response){
	if(http_response != NULL){
		if(http_response->data.buffer!=NULL){
			free(http_response->data.buffer);
		}

		free(http_response);
	}
}

