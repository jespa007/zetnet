// @base.h
// Module entry point for base.
// Controls which source files are part of this module build.
#ifndef __ZN_BASE_H__
#define __ZN_BASE_H__

#if defined(_WIN32) && !defined(__CYGWIN__)
# if defined(BUILD_STATIC_LIBS)
#  define ZN_DLL_EXPORT
# elif defined(BUILD_SHARED_LIBS)
#  define ZN_DLL_EXPORT __declspec(dllexport)
# else
#  define ZN_DLL_EXPORT __declspec(dllimport)
# endif
#elif defined(__OS2__) && defined(__WATCOMC__) && defined(__SW_BD)
#  define ZN_DLL_EXPORT __declspec(dllexport)
#elif (defined(__GNUC__) || defined(__clang__) || defined(__HP_cc)) && defined(ZETGINE_SYM_VISIBILITY)
# define ZN_DLL_EXPORT __attribute__((visibility ("default")))
#elif defined(__SUNPRO_C) && defined(ZETGINE_LDSCOPE_GLOBAL)
# define ZN_DLL_EXPORT __global
#elif defined(EMSCRIPTEN)
# include <emscripten.h>
# define ZN_DLL_EXPORT EMSCRIPTEN_KEEPALIVE
# define ZN_DLL_EXPORT_VAR
#else
# define ZN_DLL_EXPORT
#endif

#if !defined (ZN_DLL_EXPORT_VAR)
#define ZN_DLL_EXPORT_VAR ZN_DLL_EXPORT
#define ZN_EXTERN_EXPORT_VAR extern ZN_DLL_EXPORT_VAR
#endif


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

#include "ZN_Common.h"
#include "ZN_List.h"
#include "ZN_Dir.h"
#include "ZN_File.h"
#include "ZN_Log.h"
#include "ZN_Memory.h"
#include "ZN_Path.h"
#include "ZN_Cstr.h"
#include "ZN_Url.h"
#include "ZN_Array.h"
#include "ZN_ArrayCStr.h"


#endif

