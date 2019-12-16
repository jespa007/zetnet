#include "zetnet.h"

ParamValue * ParamValue_New(const char * _name, const char * _value)
{
	ParamValue * param_value = malloc(sizeof(ParamValue));
	strcpy(param_value->name, _name);
	strcpy(param_value->value, _value);

	return param_value
}

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
	HttpRequest http_request=malloc(sizeof(HttpRequest));
	strcpy(http_request->type, _type);
	strcpy(http_request->URL ,_url);
	strcpy(http_request->host, _host);
	strcpy(http_request->referer, _referer);
	strcpy(http_request->mime, _mime);
	http_request->is_binary 	= _is_binary);
	strcpy(http_request->content_type, _content_type);
	http_request->param 		= _param;

}

HttpRequest *HttpRequest_GetRequest(const char * str_request) {

	char *request=malloc(strlen(str_request));
	if(request==NULL){
		return NULL;
	}

	if (strcmp(request,"")==0)//String.IsNullOrEmpty(request))
	{
		return NULL;
	}

	ZNString_replace(request,"\r", "");

	bool is_binary= false;
	request=ZNUrl_Unescape(request);
	ZNList * tokens = ZNString_Split(request,'\n');
	ZNList * url_token = ZNString_Split(tokens[0],' ');
	char *type = url_token->items[0]; // GET/POST/etc...
	std::string url = "";

	if(url_token->count >= 2){

		url = url_token->items[1].substr(0,url_token[1].find_last_of(' '));
	}



	const char *mime = "text/html";
	std::string content_type="";
	std::vector<ParamValue> param;
	bool is_header = true;


	std::string file_extension = "";

	int find_extension=url.find_last_of(".");

	if(find_extension != -1){

		try{

			file_extension =url.substr(find_extension);//CZetNetUtils::getExtension(url);// System.IO.Path.GetExtension(url);

	#ifdef __DEBUG__
			printf("file extension: %s\n",file_extension.c_str());
	#endif

			if (file_extension==".png"){
				mime = "image/png";
				is_binary=true;
			}

			if (file_extension==".css"){
				mime = "text/css";

			}else if (file_extension==  ".json"){
				mime = "application/json";
			}
			else if (file_extension== ".pdf"){
				mime = "application/pdf";
				is_binary=true;
			}
			else if(
					 file_extension==".eot"
				  || file_extension==".svg"
				  || file_extension==".ttf"
				  || file_extension==".woff"
				  || file_extension==".woff2"
				){
				mime="application/octet-stream";
				is_binary=true;
			}
		}catch(std::exception & ex){
	#ifdef __DEBUG__
			printf("GetRequest:%s \n",ex.what());
	#endif
		}
	}

	std::vector<std::string> sub_token;


	std::vector<std::string> lst= string::split(url, '?');//.Split('?');
	if (lst.size() > 1)
	{
		url = lst[0];
	}

	std::string host = "";// tokens[2];
	std::string referer = "";

	for (unsigned i = 0; i < tokens.size(); i++)
	{
		std::string variable="";
		std::string value="";

		if (tokens[i] == "")
		{
			is_header = false;
		}

		if (is_header)
		{

			sub_token = string::split(tokens[i],':'); // split only the first : occurrence ...

			if (sub_token.size() > 1) // it has header value ...
			{
				variable = sub_token[0];
				value = sub_token[1];

				if (variable == "Referer"){
					referer = value;
				}else if(variable == "Host"){
					host = value;
				}else if(variable ==  "Accept"){

				}else if(variable ==  "Content-Type"){
					content_type = string::split(value,';')[0];
					string::replace(content_type," ","");
				}
			}
		}
		else // check parameters...
		{
			std::vector<std::string> pre_check_param = string::split(tokens[i],'&');

			if (pre_check_param.size() >= 1)
			{
				for (unsigned j = 0; j < pre_check_param.size(); j++)
				{
					sub_token = string::split(pre_check_param[j], '=' ); // split only the first = occurrence ...

					if (sub_token.size() == 2)
					{
						param.push_back(
							 ParamValue(
								sub_token[0],
								sub_token[1]
							)
						);
					}
				}

			}
		}
	}

	return new CHttpRequest(type, url, host, referer,mime, is_binary,content_type, param);

}


