#include "../src_old/zetnet.h"

#include "zn_tcp_server.c"
#include "zn_tcp_server.c"
#include "zn_tcp_socket.c"

bool ZN_Init(void){
#ifdef _WIN32
	WSADATA wsaData;
	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		fprintf(stderr,"\nWSAStartup failed with error: %d\n", iResult);
		return false;
	}
#endif

	return true;
}

void ZN_DeInit(void){
#ifdef _WIN32
	WSACleanup();
#endif
}
