#pragma once




//#include "SDL2/SDL.h"
//#include "SDL2/SDL_net.h"
#include <thread>

#include <time.h>
#include <string>
#include <sstream>
#include <vector>
#include <memory.h>





#ifdef __GNUC__
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#endif

// include socket platform
#ifdef _WIN32

		#include <winsock2.h>
		#include <ws2tcpip.h>
		#include <windows.h>
		#pragma comment (lib, "Ws2_32.lib")

		#define ioctl ioctlsocket
		#define bzero ZeroMemory
#else

	typedef SOCKET  int;
	#define INVALID_SOCKET -1

	#define addrinfo sockaddr_in



	#ifdef __GNUC__
		#include <sys/socket.h>
		#include <netinet/in.h>
		#include <sys/ioctl.h>
	#endif
#endif


#ifndef MAX_PATH
#define MAX_PATH		4096
#endif

using std::string;
using std::vector;


#include "dir.h"
#include "CUri.h"
#include "CBuffer.h"
//#include "CStringUtils.h"
#include "CIO_Utils.h"
//#include "CThread.h"
#include "CNetTcp.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpHandleClient.h"

class HttpServer:public CNetTcp
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

