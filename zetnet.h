#pragma once

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
#include <errno.h>
#endif

// include socket platform
#ifdef _WIN32

		#include <winsock2.h>
		#include <ws2tcpip.h>
		#include <windows.h>


		#define ioctl ioctlsocket
		#define bzero ZeroMemory
#else

	typedef  int SOCKET;
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



#include "string_utils.h"
#include "io_utils.h"
#include "path_utils.h"
#include "time_utils.h"


#include "CUri.h"
#include "CServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpHandleClient.h"
#include "HttpServer.h"
