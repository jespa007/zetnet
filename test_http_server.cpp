#include "jeme_base.h"

class MyCustomHttpServer: public HttpServer{


public:

	//MyCustomHttpServer();//:HttpServer(port,path,name){};

	virtual void onGetUserRequest(TCPsocket in_socket, const vector<HttpRequest::tParamValue> & Param){

        //t//ServerResponse * response = new tServerResponse();
        //response.cmd = "unknow";
        //response.message = "";
        //string [] column_filter;
        //string where_filter = "";
        //response->status = false;
        //response->result = "---";
        //string param_value="";

		string returning_value = "{\"status\":false, \"msg\":\"unknow value\"}";

        if (Param.size() > 0)
        {
            if (Param[0].Name == "cmd")
            {
            	if(Param[0].Value == "text"){
            		returning_value="{\"status\":ok, \"msg\":\"message ok\"}";
            	}
            	else{
            		returning_value = "{\"status\":false, \"msg\":\"unknow value  "+Param[0].Name+" for cmd value\"}";
            	}

            }else{
            	returning_value = "{\"status\":false, \"msg\":\"unknow cmd "+Param[0].Name+"\"}";
            }
        }


    	HttpResponse *resp = HttpResponse::MakeFromString(returning_value, "application/json");


    	resp->Post(in_socket, this);

    	delete resp;
        //return returning_value;
	}
};


int main(int argc, char *argv[])
{
//	CEngine *m_engine = CEngine::getInstance();

	//bool m_exit = false;
#if defined(__DEBUG__) && defined(__MEMMANAGER__)
	JEME_MEM_ENABLE_REGISTER_MEMORYLEAKS;
#endif

	CThread::createSingletons();

	if(SDLNet_Init()==-1) {
	    printf("SDLNet_Init: %s\n", SDLNet_GetError());
	    exit(2);
	}

	// init gfx system
	CVideo::getInstance()->createMainWindow(-800,10,
			640,
			480,
			32,
			false
			);



	//bool server_enable = false;

	HttpServer *Server = new MyCustomHttpServer();

	Server->setup(8081, "data\\www","TestServer");



	Server->connect();
	//Server->setupAsServer(8081,8081,"Server",CNet::CNET_TCP_PROTOCOL);

	do{

		//CEngine::getInstance()->run();
		JEME_INPUT->updateInput();

		SDL_Delay(100);


	}while(!JEME_INPUT->key[JEMEK_ESCAPE]);


	delete Server;
	//	delete Input;

	CInput::destroySingletons();
	CVideo::destroySingletons();
	CThread::destroySingletons();
	CLog::destroySingletons();

	SDLNet_Quit();
//	CEngine::destroy();

#if defined(__DEBUG__) && defined(__MEMMANAGER__)
  MEM_ViewStatus();
  OGL_ViewStatus();
  JEME_MEM_DISABLE_REGISTER_MEMORYLEAKS;
#endif


	return 0;
}
