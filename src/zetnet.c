#include "zetnet.h"

#include "ZN_TcpServer.c"
#include "ZN_TcpUtils.c"

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
