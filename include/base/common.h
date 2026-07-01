#ifndef __ZN_COMMON_H__
#define __ZN_COMMON_H__


#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <memory.h>
#include <pthread.h>
#include <limits.h> /* LONG_MAX */

#ifdef __GNUC__
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdarg.h>
#endif

// include socket platform
#ifdef _WIN32

	// FD_SIZE of windows is 64 -> redefine to 4096
	#define  FD_SETSIZE 4096
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <windows.h>

	#define ioctl ioctlsocket
	#define bzero ZeroMemory

#else // linux ?
	typedef  int SOCKET;

	#define INVALID_SOCKET	-1
	#define	SOCKET_ERROR	-1

	//#define addrinfo sockaddr_in

	#ifdef __GNUC__
		#include <arpa/inet.h>
		#include <sys/select.h>
		#include <sys/socket.h>
		#include <netinet/in.h>
		#include <sys/ioctl.h>
		#include <sys/types.h>
 	  	#include <netdb.h>
	#endif
#endif


#include "config.h"


#ifdef __WITH_SSL__
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>

#endif



#ifdef __MEMMANAGER__
#include "memmgr.h"
#endif

#define ZN_UNUSUED_PARAM(x) ((void)x)
#define ZN_UNUSED_2PARAMS(_param1, param2) ((void)(_param1),(void)(param2))


#endif
