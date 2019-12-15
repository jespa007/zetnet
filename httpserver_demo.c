#include "zetnet.h"

using namespace zetnet;

class MyCustomHttpServer: public CHttpServer{


public:

	virtual void onGetUserRequest(SOCKET in_socket, const std::vector<CHttpRequest::ParamValue> & param){


		std::string returning_value = "{\"status\":false, \"msg\":\"unknow value\"}";

        if (param.size() > 0)
        {
            if (param[0].name == "cmd")
            {
            	if(param[0].value == "text"){
            		returning_value="{\"status\":ok, \"msg\":\"message ok\"}";
            	}
            	else{
            		returning_value = "{\"status\":false, \"msg\":\"unknow value  "+param[0].name+" for cmd value\"}";
            	}

            }else{
            	returning_value = "{\"status\":false, \"msg\":\"unknow cmd "+param[0].name+"\"}";
            }
        }


    	CHttpResponse *resp = CHttpResponse::makeFromString(returning_value, "application/json");


    	resp->post(in_socket, this);

    	delete resp;
        //return returning_value;
	}
};


int main(int argc, char *argv[])
{


	CHttpServer *Server = new MyCustomHttpServer();

	Server->setup(8081, ".","httpservercpp_demo");

	Server->connect();
	
	getchar();
	
	printf("shutdown...\n");

	delete Server;

#ifdef __MEMMGR__
	MEMMGR_print_status();
#endif

	return 0;
}
