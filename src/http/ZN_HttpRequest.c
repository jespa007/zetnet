#include "zetnet.h"



ZN_HttpRequest * ZN_HttpRequest_New(char *  _type
		, char * _url
		, char * _host
		, char * _referer
		, const char * _mime
		, bool _is_binary
		, char * _content_type
		, ZN_List * _param
		)
{
	ZN_HttpRequest * http_request=ZN_NEW(ZN_HttpRequest);
	strcpy(http_request->type, _type);
	strcpy(http_request->URL ,_url);
	strcpy(http_request->host, _host);
	strcpy(http_request->referer, _referer);
	http_request->mime=_mime;
	http_request->is_binary 	= _is_binary;
	strcpy(http_request->content_type, _content_type);
	http_request->param 		= _param;

	return http_request;

}

ZN_HttpRequest *ZN_HttpRequest_GetRequest(const char * str_request) {
	 ZN_HttpRequest *http_request=NULL;
	char content_type[ZN_MAX_CONTENT_LEN]={0};
	const char *mime = "text/html"; //default plain text
	char url[ZN_MAX_URL_LEN]={0};
	char file_extension[10] = {0};
	char * find_extension=NULL;
	bool is_header = true;
	char  host[ZN_MAX_HOST_LEN]={0};
	char referer[ZN_MAX_REFERER_LEN]={0};

	ZN_List * params=NULL;
	ZN_List * lst=NULL;
	ZN_List * tokens = NULL;
	ZN_List * url_tokens = NULL;

	bool is_binary=false;
	char *type = 0; // GET/POST/etc...
	char *request=NULL;
	size_t request_len=strlen(str_request)+1;
	char *request_aux=NULL;

	request=ZN_MALLOC(request_len);


	if(request==NULL){
		return NULL;
	}

	memset(request,0,request_len);
	strcpy(request,str_request);

	if (strcmp(request,"")==0)//String.IsNullOrEmpty(request))
	{
		return NULL;
	}

	ZN_String_DeleteChar(request,'\r'); // avoid windows \r

	is_binary= false;

	request_aux=request; // save old pointer...
	request=ZN_Url_Unescape(request_aux); // unescape request...


	tokens = ZN_String_Split(request,'\n');
	url_tokens = ZN_String_Split(tokens->items[0],' ');

	// get type
	type = url_tokens->items[0]; // GET/POST/etc...

	// get url
	if(url_tokens->count >= 2){
		strcpy(url,url_tokens->items[1]);
	}

	find_extension=strrchr(url,'.');

	if(find_extension != NULL){

		size_t pos = find_extension-url+1;
		if(strlen(url)-pos > 5){
			fprintf(stderr,"\nError max extension (url: %s)\n",url);
		}
		else{
			strcpy(file_extension,find_extension);//.substr(find_extension);//CZetNetUtils::getExtension(url);// System.IO.Path.GetExtension(url);

	#ifdef __DEBUG__
			printf("file extension: %s\n",file_extension);
	#endif

			if(strcmp(file_extension,".png")==0){
				mime = "image/png";
				is_binary=true;
			}
			else if(strcmp(file_extension,".css")==0){
				mime = "text/css";
			}else if(strcmp(file_extension,".js")==0){
				mime = "text/javascript";
			}else if(strcmp(file_extension,  ".json")==0){
				mime = "application/json";
			}else if(strcmp(file_extension,  ".gltf")==0){
				mime = "model/gltf+json";
			}else if(strcmp(file_extension, ".pdf")==0){
				mime = "application/pdf";
				is_binary=true;
			}else if(strcmp(file_extension, ".wasm")==0){
				mime = "application/wasm";
				is_binary=true;
			}else if(
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

	}


	lst= ZN_String_Split(url, '?');//.Split('?');
	if (lst->count > 1)
	{
		strcpy(url,lst->items[0]);
	}

	for (unsigned i = 0; i < tokens->count; i++)
	{
		char variable[256]="";
		char value[4096]="";

		if (strcmp(tokens->items[i],"")==0)
		{
			is_header = false;
		}

		if (is_header)
		{

			ZN_List * sub_tokens = ZN_String_Split(tokens->items[i],':'); // split only the first : occurrence ...

			if (sub_tokens->count > 1) // it has header value ...
			{
				strcpy(variable,sub_tokens->items[0]);
				strcpy(value,sub_tokens->items[1]);

				if (strcmp(variable,"Referer")==0){
					strcpy(referer,value);
				}else if(strcmp(variable , "Host")==0){
					strcpy(host,value);
				}else if(strcmp(variable ,  "Accept")==0){

				}else if(strcmp(variable ,  "Content-Type")==0){
					ZN_List *tl=ZN_String_Split(value,';');
					if(tl->count > 0){
						strcpy(content_type,tl->items[0]);
					}
					ZN_String_DeleteChar(content_type,' ');
					ZN_List_DeleteAndFreeAllItems(tl);
				}
			}

			ZN_List_DeleteAndFreeAllItems(sub_tokens);


		}
		else // check parameters...
		{
			ZN_List * pre_check_params = ZN_String_Split(tokens->items[i],'&');

			if (pre_check_params->count >= 1)
			{
				for (unsigned j = 0; j < pre_check_params->count; j++)
				{
					ZN_List *sub_tokens = ZN_String_Split(pre_check_params->items[j], '=' ); // split only the first = occurrence ...

					if (sub_tokens->count == 2)
					{
						ZN_List_Add(params,
							 ZN_HttpParamValue_New(
								sub_tokens->items[0],
								sub_tokens->items[1]
							)
						);
					}

					ZN_List_DeleteAndFreeAllItems(sub_tokens);
				}

			}

			ZN_List_DeleteAndFreeAllItems(pre_check_params);
		}
	}



	http_request=ZN_HttpRequest_New(type, url, host, referer,mime, is_binary,content_type, params);

	// finally ZN_FREE all depending resources...
	ZN_List_DeleteAndFreeAllItems(tokens);
	ZN_List_DeleteAndFreeAllItems(url_tokens);
	ZN_List_DeleteAndFreeAllItems(lst);
	ZN_FREE(request);
	ZN_FREE(request_aux);

	return http_request;
}


void		  ZN_HttpRequest_Delete(ZN_HttpRequest *http_request){
	if(http_request!=NULL){
		ZN_List_DeleteAndFreeAllItems(http_request->param);
		ZN_FREE(http_request);
	}
}
