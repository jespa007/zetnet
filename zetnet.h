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


#include "utils/string.h"
#include "utils/io.h"
#include "utils/path.h"
#include "utils/url.h"

#include "CServer.h"
#include "CHttpRequest.h"
#include "CHttpResponse.h"
#include "CHttpHandleClient.h"
#include "CHttpServer.h"


#define ZETNET_MAJOR_VERSION 1
#define ZETNET_MINOR_VERSION 0
#define ZETNET_PATCH_VERSION 0
