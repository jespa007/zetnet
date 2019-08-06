#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <thread>
#include <time.h>
#include <string>
#include <sstream>
#include <vector>
#include <memory.h>


#ifdef __GNUC__

struct _finddata_t
 {
       // File location storage location. It stores an unsigned unit that represents the attributes of the file. File attributes are represented by bits, mainly the following: _A_ARCH (archive), _A_HIDDEN (hidden), _A_NORMAL (normal), _A_RDONLY (read only), _A_SUBDIR (folder), _A_SYSTEM (system)
       unsigned attrib;
               // The time_t here is a variable type (long integer? Equivalent to long int?), used to store the time, we do not care about it for the time being, as long as we know that this time_create variable is used to store the file creation time.
       time_t time_create;
               //The last time the file was accessed
       time_t time_access;
               // The last time the file was modified
       time_t time_write;
               //The size of the file. Here _fsize_t should be equivalent to unsigned integer, indicating the number of bytes in the file
       _fsize_t size;
               // File name of the file. Here _MAX_FNAME is a constant macro, which is defined in the <stdlib.h> header file, indicating the maximum length of the file name.
       char name[_MAX_FNAME];
 };
	#include <unistd.h>
	#include <fcntl.h>
 	#include <sys/types.h>
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



#define ZETNET_MAJOR_VERSION 1
#define ZETNET_MINOR_VERSION 0
#define ZETNET_PATCH_VERSION 0



#include "string.h"
#include "io.h"
#include "path.h"
#include "url.h"



#include "CServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpHandleClient.h"
#include "HttpServer.h"
