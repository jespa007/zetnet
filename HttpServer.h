#pragma once

namespace zetnet{

	class HttpServer:public CServer
	{
	public:
		std::string MSG_DIR;
		std::string WEB_DIR;
		std::string VERSION;
		std::string NAME;
		std::string LOGO_BASE64;

		HttpServer(
		);

		void setup(	int _port,const std::string & web_dir,const std::string &  instance_name);


		virtual void onGetUserRequest(SOCKET  _socket_client,const std::vector<HttpRequest::ParamValue> & param);

	protected:
		void setLogoBase64(std::string _image_base_64);
		virtual bool gestMessage(SOCKET in_socket, uint8_t *buffer, uint32_t len);

	private:

		void * web_client;

	};

};
