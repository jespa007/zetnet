#include "zetnet.h"


namespace zetnet{

	HttpRequest *HttpRequest::getRequest(const std::string & str_request) {

		std::string request = str_request;

		if (request == "")//String.IsNullOrEmpty(request))
		{
			return NULL;
		}

		string::replace(request,"\r", "");

		bool is_binary= false;
		request=CUri::unescape(request);
		std::vector<std::string> tokens = string::split(request,'\n');
		std::vector<std::string> url_token = string::split(tokens[0],' ');//split(new char[] { ' ' }, 2);
		std::string type = url_token[0]; // GET/POST/etc...
		std::string url = "";

		if(url_token.size() >= 2){

			url = url_token[1].substr(0,url_token[1].find_last_of(' '));
		}



		std::string mime = "text/html";
		std::string content_type="";
		std::vector<ParamValue> param;// = new List<tParamValue>();
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


		std::vector<string> lst= string::split(url, '?');//.Split('?');
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

		return new HttpRequest(type, url, host, referer,mime, is_binary,content_type, param);

	}

};
