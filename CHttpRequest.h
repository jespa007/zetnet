#pragma once

namespace zetnet{

	class CHttpRequest
	{


	public:

		struct ParamValue
		{
			std::string name;
			std::string value;

			ParamValue(const std::string & _name, const std::string & _value)
			{
				name = _name;
				value = _value;
			}
		};

		std::string type;
		std::string URL;
		std::string host;
		std::string referer;
		std::string mime;
		bool is_binary;
		std::string content_type;
		std::vector<ParamValue> param;


		static CHttpRequest * getRequest(const std::string & request);

	protected:

	private:
		CHttpRequest(const std::string &  _type
				, const std::string & _url
				, const std::string & _host
				, const std::string & _referer
				, const std::string & _mime
				, bool _is_binary
				, const std::string & _content_type
				, const std::vector<ParamValue> & _param
				)
		{
			type 		= _type;
			URL 		= _url;
			host 		= _host;
			referer 	= _referer;
			mime 		= _mime;
			is_binary 	= _is_binary;
			content_type= _content_type;
			param 		= _param;

		}


	};

};
