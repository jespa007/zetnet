#pragma once

class HttpRequest
{


public:

	struct tParamValue
	{
		string Name;
		string Value;

		tParamValue(const string & _Name, const string & _Value)
		{
			Name = _Name;
			Value = _Value;
		}
	};

	string Type;
	string URL;
	string Host;
	string Referer;
	string Mime;
	bool IsBinary;
	string ContentType;
	vector<tParamValue> Param;

	static ByteBuffer * GetStreamFromUrl(const string & uri);
	static HttpRequest * GetRequest(const string & request);

protected:

private:
	HttpRequest(const string &  _type
			, const string & _url
			, const string & _host
			, const string & _referer
			, const string & _mime
			, bool is_binary
			, const string & _content_type
			, const vector<tParamValue> & _param
			)
	{
		Type = _type;
		URL = _url;
		Host = _host;
		Referer = _referer;
		Mime = _mime;
		IsBinary = is_binary;
		ContentType = _content_type;
		Param = _param;

	}


};

