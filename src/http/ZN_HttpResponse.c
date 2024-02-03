#include "zetnet.h"

#ifdef _WIN32
#define ZN_SEPARATOR_DIR "\\"
#else
#define ZN_SEPARATOR_DIR "/"
#endif

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

BufferData  ZN_HttpResponse_GenerateError(int error_id,ZN_HttpServer * http_server)
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

	data.size=strlen(template)+(http_server->logo_base64!=NULL?strlen(http_server->logo_base64):0)+strlen(error.description)+strlen(error.title);
	data.buffer = (uint8_t *)malloc(data.size+1); // +1 for end str

	sprintf((char *)data.buffer,
			template
		,http_server->logo_base64!=NULL?http_server->logo_base64:""
		,error.title
		,error.description
	);
	return data;
}

ZN_HttpResponse * ZN_HttpResponse_New( const char * status,const char * mime,bool is_binary, BufferData data) {
	ZN_HttpResponse * http_response = ZN_NEW(ZN_HttpResponse);
	http_response->data = data;
	http_response->status = status;
	http_response->mime = mime;
	http_response->is_binary = is_binary;

	return http_response;
}

ZN_HttpResponse * ZN_HttpResponse_MakeFromString(const char * str, const char * mime)
{
	BufferData data;
	data.size=strlen(str);
	data.buffer=(uint8_t *)malloc(data.size+1);

	strcpy((char *)data.buffer,(char *)str);

	return ZN_HttpResponse_New("200 OK", mime, false, data);
}

ZN_HttpResponse * ZN_HttpResponse_MakeMethodNotAllowed(ZN_HttpServer * _http_server)
{
	return ZN_HttpResponse_New("405 Method not allowed", "html/text", false, ZN_HttpResponse_GenerateError(HTML_ERROR_405, _http_server));
}

ZN_HttpResponse * ZN_HttpResponse_MakeNullRequest(ZN_HttpServer * _http_server)
{
	return ZN_HttpResponse_New("400 Bad Request", "html/text",false,  ZN_HttpResponse_GenerateError(HTML_ERROR_400, _http_server));
}

ZN_HttpResponse * ZN_HttpResponse_MakePageNotFound(ZN_HttpServer * _http_server)
{
	return ZN_HttpResponse_New("404 Bad Request", "html/text", false, ZN_HttpResponse_GenerateError(HTML_ERROR_404, _http_server));
}

ZN_HttpResponse *ZN_HttpResponse_FromFile(ZN_HttpRequest * _request,const char *_filename_with_path){

	char path[MAX_PATH]="";
	char file[MAX_PATH]="";
	bool ok = false;

#ifdef __DEBUG__
	printf("try_file:%s request:%s\n",_filename_with_path,_request->URL);
#endif

	ZN_Path_GetDirectoryName(path,_filename_with_path);

	if (ZN_File_Exists(_filename_with_path)/* && fi.Extension.Contains(".")*/)
	{
		ZN_Path_GetFileName(file,_filename_with_path);

#ifdef __DEBUG__
		printf("file \"%s\" filename with ok!\n",_filename_with_path);
#endif
		ok = true;
	}
	else if(zn_dir_exists(_filename_with_path)) // file not exist try to index.html  in the directory...
	{
		ZN_List * list_file=NULL;

		list_file = zn_dir_list_files(_filename_with_path,NULL,false);//,"*.html",false);

		for(unsigned f=0; f < list_file->count && !ok; f++){ //foreach(FileInfo ff in files){
			//String n = ff.Name;
			char n[MAX_PATH]="";

			ZN_Path_GetFileName(n,list_file->items[f]);

	#ifdef __DEBUG__
			printf("try_file2:%s\n",n);
#endif

			if(strcmp(n,"index.html")==0){
				strcpy(path,_filename_with_path);
				strcpy(file,n);
				ok = true;
			}
		}

		ZN_List_DeleteAndFreeAllItems(list_file);
	}

	if (ok)
	{
		char filename_to_load[MAX_PATH];
		strcpy(filename_to_load,path);
		strcat(filename_to_load,ZN_SEPARATOR_DIR);
		strcat(filename_to_load,file);

#ifdef __DEBUG__
		printf("send file:%s\n",filename_to_load);
#endif

		BufferData data;

		data.buffer=ZN_File_Read(filename_to_load,&data.size);

		return ZN_HttpResponse_New("200 OK", _request->mime, _request->is_binary, data);
	}else{
#ifdef __DEBUG__
		printf("not found\n");
#endif
	}

	return NULL;
}

void ZN_HttpResponse_Send(ZN_HttpResponse *http_response,SOCKET dst_socket, ZN_HttpServer * http_server) //, const string & response_action)
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

		strcat(header_str,"Content-Type: ");
		strcat(header_str, http_response->mime);
		strcat(header_str,"\n");
		strcat(header_str,"Accept-Ranges: bytes\n");
		strcat(header_str,"Connection: Keep-Alive\n");
		if(http_server->send_same_site_attribute){
			strcat(header_str,"Set-Cookie: cross-site-cookie=name; SameSite=None; Secure\n");
		}
		strcat(header_str,"Content-Length: ");
		strcat(header_str,ZN_String_FromInt(data_len));

		if(http_response->is_binary){
			strcat(header_str,"\nContent-Transfer-Encoding: binary");
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
		strcat(header_str,ZN_String_FromInt(http_response->data.size));
		strcat(header_str,"\n\n");
	}

	header_str_len=strlen(header_str);
	total_size = header_str_len+data_len+1+1; // +2 header + body + \n\n + 0

	buffer=(char *)ZN_MALLOC(total_size);

	memcpy(buffer										 ,header_str,header_str_len);
	memcpy(buffer+header_str_len				         ,http_response->data.buffer,data_len);
	buffer[header_str_len+data_len]=10;

	ZN_TcpUtils_SendBytes(dst_socket,(uint8_t *)buffer,total_size);

	ZN_FREE(buffer);
}

void ZN_HttpResponse_Delete(ZN_HttpResponse * http_response){
	if(http_response != NULL){
		if(http_response->data.buffer!=NULL){
			ZN_FREE(http_response->data.buffer);
		}

		ZN_FREE(http_response);
	}
}

