#include "zetnet.h"

#define BUFFER_SIZE 1024


int main(int argc, char *argv[])
{
	char url[MAX_PATH]={"api.opentopodata.org"};
	int port=443;

	ZN_Init();

	// perform http request...
	SOCKET 	socket = ZN_TcpUtils_NewSocketClient(url,port);
	char buffer[BUFFER_SIZE];

	const char *GET_PARAMS = "/v1/srtm90m?locations=41.4827,0.2864 \r\n";

	ZN_TcpUtils_SendBytes(socket, (uint8_t*)GET_PARAMS, strlen(GET_PARAMS)); // write(fd, char[]*, len);


	while(ZN_TcpUtils_ReceiveBytes(socket, (uint8_t *)buffer, BUFFER_SIZE - 1) != 0){
			fprintf(stderr, "%s", buffer);
			memset(buffer,0, BUFFER_SIZE);
		}
	ZN_DeInit();

	return 0;
}
