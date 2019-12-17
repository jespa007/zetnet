#include "zetnet.h"



HttpRequest * HttpRequest_New(const char *  _type
		, const char * _url
		, const char * _host
		, const char * _referer
		, const char * _mime
		, bool _is_binary
		, const char * _content_type
		, ZNList * _param
		)
{
	HttpRequest * http_request=malloc(sizeof(HttpRequest));
	strcpy(http_request->type, _type);
	strcpy(http_request->URL ,_url);
	strcpy(http_request->host, _host);
	strcpy(http_request->referer, _referer);
	strcpy(http_request->mime, _mime);
	http_request->is_binary 	= _is_binary;
	strcpy(http_request->content_type, _content_type);
	http_request->param 		= _param;

	return http_request;

}

HttpRequest *HttpRequest_GetRequest(const char * str_request) {
	char url[4096]="";
	char *request=malloc(strlen(str_request));
	char file_extension[10] = "";
	char * find_extension=NULL;
	ZNList * param=NULL;
	char * content_type="";
	const char *mime = "text/html";
	bool is_header = true;
	ZNList * sub_token=NULL;
	ZNList * lst=NULL;
	bool is_binary;
	ZNList * tokens = NULL;
	ZNList * url_token = NULL;
	char *type = 0; // GET/POST/etc...

	if(request==NULL){
		return NULL;
	}

	if (strcmp(request,"")==0)//String.IsNullOrEmpty(request))
	{
		return NULL;
	}

	ZNString_ReplaceString(request,"\r", "");

	is_binary= false;
	request=ZNUrl_Unescape(request);
	tokens = ZNString_Split(request,'\n');
	url_token = ZNString_Split(tokens->items[0],' ');
	type = url_token->items[0]; // GET/POST/etc...


	if(url_token->count >= 2){
		char *p=strrchr(url_token->items[1], ' ');
		if(p!=NULL){
			strncpy(url,url_token->items[1],p-(char *)url_token->items[1]+1);
		}
		//url = url_token->items[1].substr(0,url_token[1].find_last_of(' '));
	}



	find_extension=strrchr(url,'.');


	if(find_extension != NULL){

			strncpy(file_extension,find_extension,find_extension-url+1);//.substr(find_extension);//CZetNetUtils::getExtension(url);// System.IO.Path.GetExtension(url);

	#ifdef __DEBUG__
			printf("file extension: %s\n",file_extension);
	#endif

			if(strcmp(file_extension,".png")==0){
				mime = "image/png";
				is_binary=true;
			}
			else if(strcmp(file_extension,".css")==0){
				mime = "text/css";

			}else if(strcmp(file_extension,  ".json")==0){
				mime = "application/json";
			}
			else if(strcmp(file_extension, ".pdf")==0){
				mime = "application/pdf";
				is_binary=true;
			}
			else if(
					 (strcmp(file_extension,".eot")==0)
				  || (strcmp(file_extension,".svg")==0)
				  || (strcmp(file_extension,".ttf")==0)
				  || (strcmp(file_extension,".woff")==0)
				  || (strcmp(file_extension,".woff2")==0)
				){
				mime="application/octet-stream";
				is_binary=true;
			}

	}


	lst= ZNString_Split(url, '?');//.Split('?');
	if (lst->count > 1)
	{
		strcpy(url,lst->items[0]);
	}

	char *host = "";// tokens[2];
	char *referer = "";

	for (unsigned i = 0; i < tokens->count; i++)
	{
		char variable[256]="";
		char value[256]="";

		if (strcmp(tokens->items[i],"")==0)
		{
			is_header = false;
		}

		if (is_header)
		{

			sub_token = ZNString_Split(tokens->items[i],':'); // split only the first : occurrence ...

			if (sub_token->count > 1) // it has header value ...
			{
				strcpy(variable,sub_token->items[0]);
				strcpy(value,sub_token->items[1]);

				if (strcmp(variable,"Referer")==0){
					referer = value;
				}else if(strcmp(variable , "Host")==0){
					host = value;
				}else if(strcmp(variable ,  "Accept")==0){

				}else if(strcmp(variable ,  "Content-Type")==0){
					ZNList *tl=ZNString_Split(value,';')->items[0];
					if(tl->count > 0){
						content_type =tl->items[0];
					}
					ZNString_ReplaceString(content_type," ","");
				}
			}
		}
		else // check parameters...
		{
			ZNList * pre_check_param = ZNString_Split(tokens->items[i],'&');

			if (pre_check_param->count >= 1)
			{
				for (unsigned j = 0; j < pre_check_param->count; j++)
				{
					sub_token = ZNString_Split(pre_check_param->items[j], '=' ); // split only the first = occurrence ...

					if (sub_token->count == 2)
					{
						ZNList_Add(param,
							 HttpParamValue_New(
								sub_token->items[0],
								sub_token->items[1]
							)
						);
					}
				}

			}
		}
	}

	return HttpRequest_New(type, url, host, referer,mime, is_binary,content_type, param);

}


void		  HttpRequest_Delete(HttpRequest *http_request){
	if(http_request!=NULL){
		ZNList_Delete(http_request->param);
	}
}
