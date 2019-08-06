#include "zetnet.h"

namespace zetnet{


	HttpServer::HttpServer(){

		this->NAME = "unknow";
		this->WEB_DIR = ".";
		this->LOGO_BASE64="";
		web_client = NULL;
	}


	 void HttpServer::setup(int _port
				,const std::string & web_dir
				,const std::string & instance_name){

			this->NAME = instance_name;
			this->WEB_DIR = web_dir;
			this->LOGO_BASE64="data:image/png;base64,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";
			web_client = NULL;
			CServer::setup(_port,this->NAME.c_str());

	 }

	void  HttpServer::SetLogoBase64(std::string _image_base_64)
	{
		LOGO_BASE64 = _image_base_64;
	}

	bool HttpServer::gestMessage(SOCKET socket, uint8_t *data, uint32_t size){

		HttpHandleClient hc = HttpHandleClient(socket, this,data, size);
		hc.doHandle();
		return true;
	}


	void HttpServer::onGetUserRequest(SOCKET  _socket_client,const std::vector<HttpRequest::ParamValue> & param){

		HttpResponse *resp = HttpResponse::MakeFromString("onGetUserRequest is not implemented!", "application/json");

		resp->post(_socket_client, this);

		delete resp;;
	}

	//---------------------------------------------------------------------------------------------------------------

};
