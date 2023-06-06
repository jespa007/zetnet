#include "zetnet.h"

#include "ZN_TcpServer.c"
#include "ZN_TcpUtils.c"

bool ZetNet_Init(void){
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

void ZetNet_DeInit(void){
#ifdef _WIN32
	WSACleanup();
#endif
}
