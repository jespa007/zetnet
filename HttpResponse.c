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

BufferData  HttpResponse_GenerateError(int error_id,HttpServer * http_server)
{
	HtmlError error;
	char int_error[100];
	BufferData data;
	const char * template="<html>"
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
	"</html>";

	sprintf(int_error,"%i",error_id);

	if(error_id < MAX_ERROR_TYPES){
		error = html_error[error_id];
	}else{
		error.title=int_error;
		error.description="not implemented";
	}

	data.size=strlen(template)+strlen(http_server->logo_base64)+strlen(error.description)+strlen(error.title);
	data.buffer = (uint8_t *)ZN_MALLOC(data.size+1); // +1 for end str

	sprintf((char *)data.buffer,
			template
		,http_server->logo_base64
		,error.title
		,error.description
	);
	//strcpy((char *)data.buffer,(char *)str);


	return data;

}


HttpResponse * HttpResponse_New( const char * status,const char * mime,bool is_binary, BufferData data) {
	HttpResponse * http_response = ZN_MALLOC(sizeof(HttpResponse));
	http_response->data = data;
	http_response->status = status;
	http_response->mime = mime;
	http_response->is_binary = is_binary;

	return http_response;
}


HttpResponse * HttpResponse_MakeFromString(const char * str, const char * mime)
{
	BufferData data;
	data.size=strlen(str);
	data.buffer=(uint8_t *)ZN_MALLOC(data.size+1);

	strcpy((char *)data.buffer,(char *)str);


	return HttpResponse_New("200 OK", mime, false, data);
}

HttpResponse * HttpResponse_MakeMethodNotAllowed(HttpServer * webserver)
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
	char filename_with_path[MAX_PATH]={0};
	char path_url[MAX_PATH]={0};
	char file[MAX_PATH]="";
	char path[MAX_PATH]="";

	bool ok = false;

	if (request == NULL){
		return HttpResponse_MakeNullRequest(webserver);
	}

	if(strcmp(request->type,"GET")==0){

		char *unescaped_url=ZNUrl_Unescape(request->URL);
		sprintf(path_url,"%s",unescaped_url);
		ZN_FREE(unescaped_url);
#ifdef WIN32
		ZNString_ReplaceChar(path_url, '/','\\');//CUri::unescape(request->URL)
#endif

		sprintf(filename_with_path,
				"%s%s",
			  webserver->web_dir,
			path_url);

		ok = false;



#ifdef __DEBUG__
		printf("\ntry_file:%s request:%s",filename_with_path,request->URL);
#endif

		ZNPath_GetDirectory(path,filename_with_path);

		if (ZNIO_FileExists(filename_with_path)/* && fi.Extension.Contains(".")*/)
		{
			ZNPath_GetFilename(file,filename_with_path);

#ifdef __DEBUG__
			printf("\nfile \"%s\" filename with ok!",filename_with_path);
#endif
			ok = true;
		}
		else // file not exist try to solve automatically...
		{
			ZNList * list_file=NULL;

			printf("\nfile \"%s\" not exist ...",filename_with_path);

			if (!ZNIO_IsDirectory(path)){
				return HttpResponse_MakePageNotFound(webserver);
			}

			list_file = ZNIO_ListFiles(path,NULL,false);//,"*.html",false);

			for(unsigned f=0; f < list_file->count && !ok; f++){ //foreach(FileInfo ff in files){
				//String n = ff.Name;
				char n[MAX_PATH]="";

				if(ZNPath_GetFilename(n,list_file->items[f])){

	#ifdef __DEBUG__
					printf("\ntry_file2:%s",n);
	#endif

					if(strcmp(n,"index.html")==0){
						strcpy(file,n);
						ok = true;
					}
				}
			}

			ZNList_DeleteAndFreeAllItems(list_file);
		}

		if (ok)
		{
#ifdef _WIN32
#define SEPARATOR_DIR "\\"
#else
#define SEPARATOR_DIR "/"
#endif

			char filename_to_load[MAX_PATH];
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
	char header_str[4096]={0};
	size_t header_str_len=0;
	size_t data_len=0;
	char *buffer;
	int total_size=0;

	strcat(header_str,"HTTP/1.1 ");
	strcat(header_str,http_response->status);
	strcat(header_str,"\n");
	strcat(header_str,"Server: ");
	strcat(header_str,http_server->name);
	strcat(header_str,"\n");

	if (strcmp(http_response->status,"200 OK")==0) // send response content
	{
		data_len=http_response->data.size;
		if(!http_response->is_binary){
			data_len=strlen((char *)http_response->data.buffer);
		}
		strcat(header_str,"Content-Type: ");
		strcat(header_str, http_response->mime);
		strcat(header_str,"\n");
		strcat(header_str,"Accept-Ranges: bytes\n");
		strcat(header_str,"Connection: Keep-Alive\n");
		strcat(header_str,"Content-Length: ");
		strcat(header_str,ZNString_IntToString(data_len));

		if(http_response->is_binary){
			strcat(header_str,"Content-Transfer-Encoding: binary");
		}

		strcat(header_str,"\n\n");

#ifdef __DEBUG__
		printf("POST:\n%s\n",header_str);
#endif


	}
	else // only basic html error information
	{

		strcat(header_str,"Content-Type: ");
		strcat(header_str, http_response->mime);
		strcat(header_str,"\n");
		strcat(header_str,"Content-Length: ");
		strcat(header_str,ZNString_IntToString(http_response->data.size));
		strcat(header_str,"\n\n");

	}

	header_str_len=strlen(header_str);
	total_size = header_str_len+data_len+1+1; // +2 header + body + \n\n + 0

	buffer=(char *)ZN_MALLOC(total_size);

	memcpy(buffer										 ,header_str,header_str_len);
	memcpy(buffer+header_str_len				         ,http_response->data.buffer,data_len);
	buffer[header_str_len+data_len]=10;

	TcpUtils_SendBytes(dst_socket,(uint8_t *)buffer,total_size);

	ZN_FREE(buffer);

}

void HttpResponse_Delete(HttpResponse * http_response){
	if(http_response != NULL){
		if(http_response->data.buffer!=NULL){
			ZN_FREE(http_response->data.buffer);
		}

		ZN_FREE(http_response);
	}
}

