#include "zetnet.h"


namespace zetnet{

HttpRequest *HttpRequest::GetRequest(const string & str_request) {

	string request = str_request;

	if (request == "")//String.IsNullOrEmpty(request))
	{
		return NULL;
	}

	string_utils::replace(request,"\r", "");

	bool is_binary= false;
	request=CUri::unescape(request);
	vector<string> tokens = string_utils::split(request,'\n');
	vector<string> url_token = string_utils::split(tokens[0],' ');//split(new char[] { ' ' }, 2);
	string type = url_token[0]; // GET/POST/etc...
	string url = "";

	if(url_token.size() >= 2){

		url = url_token[1].substr(0,url_token[1].find_last_of(' '));
	}



	string mime = "text/html";
	string content_type="";
	vector<tParamValue> param;// = new List<tParamValue>();
	bool is_header = true;


	string file_extension = "";

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

	vector<string> sub_token;


	vector<string> lst= string_utils::split(url, '?');//.Split('?');
	if (lst.size() > 1)
	{
		url = lst[0];
	}

	string host = "";// tokens[2];
	string referer = "";

	for (unsigned i = 0; i < tokens.size(); i++)
	{
		string variable="";
		string value="";

		if (tokens[i] == "")
		{
			is_header = false;
		}

		if (is_header)
		{

			sub_token = string_utils::split(tokens[i],':'); // split only the first : occurrence ...

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
					content_type = string_utils::split(value,';')[0];
					string_utils::replace(content_type," ","");
				}
			}
		}
		else // check parameters...
		{
			vector<string> pre_check_param = string_utils::split(tokens[i],'&');

			if (pre_check_param.size() >= 1)
			{
				for (unsigned j = 0; j < pre_check_param.size(); j++)
				{
					sub_token = string_utils::split(pre_check_param[j], '=' ); // split only the first = occurrence ...

					if (sub_token.size() == 2)
					{
						param.push_back(
							 tParamValue(
								sub_token[0],
								sub_token[1]
							)
						);
					}
				}

			}
		}
	}

	return new HttpRequest(type, url, host, referer,mime, is_binary,content_type, param);

}

};
