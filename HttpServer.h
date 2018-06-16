#pragma once

namespace zetnet{

class HttpServer:public CServer
{
public:
	string MSG_DIR;
	string WEB_DIR;
	string VERSION;
	string NAME;
	string LOGO_BASE64;
	//Func<List<Request.tParamValue>, string> gest_message_action = null;


	HttpServer(

			//,Func<List<Request.tParamValue>, string> _gest_message_action = null
	);

	void setup(//IPAddress _ip,
	int _port
	,const string & web_dir
	,const string &  instance_name);


	virtual void onGetUserRequest(SOCKET  _socket_client,const vector<HttpRequest::tParamValue> & param);

protected:
	void SetLogoBase64(string _image_base_64);
	virtual bool gestMessage(SOCKET in_socket, uint8_t *buffer, uint32_t len);

	//virtual void  gestServer();

private:

	void * web_client;

};

};
