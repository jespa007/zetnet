#include "zetnet.h"

#define BUFFER_SIZE 1024


int main(int argc, char *argv[])
{
	char url[MAX_PATH]={"api.opentopodata.org"};
	int port=443;
	char buffer[BUFFER_SIZE];

	ZN_Init();

	// perform http request...
	SOCKET 	socket = ZN_TcpUtils_NewSocketClient(url,port);



	if(socket != INVALID_SOCKET){

		SSL *ssl = ZN_SSL_New(socket,url);

		// SNI (Server Name Indication).
		if (ZN_SSL_Connect(ssl)) {


			if (ZN_SSL_VerifyHost(ssl, url)) {

				//SSL_get_error()

				const char *request =
					"GET /v1/srtm90m?locations=41.4827,0.2864 HTTP/1.1\r\n"
					"Host: api.opentopodata.org\r\n"
					"User-Agent: zn-client/1.0\r\n"
					"Connection: close\r\n\r\n";


				ZN_SSL_Write(ssl, (uint8_t*)request, strlen(request)); // write(fd, char[]*, len);


				while(1){//ZN_TcpUtils_ReceiveBytes(socket, (uint8_t *)buffer, BUFFER_SIZE - 1) != 0){
					int n = ZN_SSL_Read(ssl, (uint8_t*)buffer, BUFFER_SIZE - 1);
					if (n <= 0) break;

					buffer[n] = '\0';
					printf("%s", buffer);
				}
			}else{
					fprintf(stderr, "Hostname verification failed\n");
			}


		}else{
			fprintf(stderr, "Cannot connect ssl\n");
		}




		ZN_SSL_Close(ssl);

		ZN_TcpUtils_CloseSocket(&socket);
	}
	ZN_DeInit();

	return 0;
}
