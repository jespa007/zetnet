#ifndef __ZETNET_H__
#define __ZETNET_H__

#include <stdio.h>
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
#endif

// include socket platform
#ifdef _WIN32

	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <windows.h>


	#define ioctl ioctlsocket
	#define bzero ZeroMemory
#else // linux ?
	typedef  int SOCKET;
	#define INVALID_SOCKET 	-1
	#define SOCKET_ERROR	-1

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



#ifdef __MEMMANAGER__
#include "memmgr.h"
#endif


//----------------------------
// START ZETNET PROTOTYPES

#define ZETNET_VERSION_MAJOR	1
#define ZETNET_VERSION_MINOR	3
#define ZETNET_VERSION_PATCH	0


#ifdef  __cplusplus
extern "C" {
#endif

#define ZN_UNUSUED_PARAM(x) ((void)x)

#include "TcpUtils.h"
#include "TcpServer.h"

#include "util/zn_util.h"
#include "http/zn_http.h"

bool ZetNet_Init(void);
void ZetNet_DeInit(void);


#ifdef  __cplusplus
}
#endif

#endif
