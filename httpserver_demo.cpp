#include "HttpServer.h"

class MyCustomHttpServer: public HttpServer{


public:

	virtual void onGetUserRequest(TCPsocket in_socket, const vector<HttpRequest::tParamValue> & Param){


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

	CThread::createSingletons();

	if(SDLNet_Init()==-1) {
	    printf("SDLNet_Init: %s\n", SDLNet_GetError());
	    exit(-1);
	}


	//bool server_enable = false;

	HttpServer *Server = new MyCustomHttpServer();

	Server->setup(8081, ".","httpservercpp_demo");



	Server->connect();
	
	do{
	
	}while(getchar()!='s');
	
	delete Server;

	CThread::destroySingletons();

	SDLNet_Quit();

	return 0;
}
