#include "zetnet.h"

// sample socket client
// https://learn.microsoft.com/es-es/windows/win32/api/winsock/nf-winsock-recv

#define DEFAULT_PORT 27015
#define BUFFER_LEN   512
#define CLEAR_BUFFER(buffer) memset(buffer,0,sizeof(buffer))


int main(int argc, char *argv[]){
	int port = DEFAULT_PORT;
	char *host = NULL;
	bool error = false;
	bool exit = false;

	for(int i=1; i < argc && !error; i++){
		ZN_List *l=ZN_String_Split(argv[i],'=');
		switch(l->count){
		default:
			fprintf(stderr,"unknow option %s\n",(const char *)l->items[0]);
			error=true;
			break;
		case 2:
			if(strcmp(l->items[0],"--host")==0){
				if(host == NULL){
					size_t len = strlen(l->items[1])+1;
					host = ZN_MALLOC(len);
					strcpy(host,l->items[1]);
				}
			}else if(strcmp(l->items[0],"--port")==0){
				error=!ZN_String_ToInt(&port,l->items[1],10);
			}else{
				fprintf(stderr,"unknow option %s\n",(const char *)l->items[0]);
				error=true;
			}
			break;
		}

		ZN_List_DeleteAndFreeAllItems(l);
	}

	if(error){
		return -1;
	}

	ZN_Init();

	printf("TCP Socket client test ver x.x.x\n");
	printf("Host t %s:%i\n",host,port);

	SOCKET socket = INVALID_SOCKET;

	uint8_t buffer[BUFFER_LEN];

	do{

		switch(getchar()){
		case 'o':
			// close socket
			if((socket = ZN_TcpUtils_NewSocketClient(host,port)) == INVALID_SOCKET){
				fprintf(stderr,"Cannot create socket\n");
			}
			break;
		case 'c':
			// close socket
			if(socket != INVALID_SOCKET){
				ZN_TcpUtils_CloseSocket(&socket);
			}else{
				fprintf(stderr,"Cannot close socket. Socket not open!\n");
			}
			break;
		case 's':
			// open socket to transmit as many bytes as the client want to send
			if(socket != INVALID_SOCKET){
				// read characters until press enter
				if(read(STDIN_FILENO, buffer, BUFFER_LEN-1) > 0)
					if((send_bytes = ZN_TcpUtils_SendBytes(socket,buf,BUFFER_LEN))< buf_len){
						fprintf(stderr,"buffer length %i < send bytes %i ",buf_len,send_bytes);
					}
				}

					// try to receive the bytes send (echo)
				if(ZN_TcpUtils_ReceiveBytes(socket,buf)){
					fprintf(stderr,"buffer length %i < send bytes %i ",buf_len,send_bytes);
				}

			}else{
				fprintf(stderr,"You must open socket first before send data\n");
			}
		case 'q':
			exit = true;
			break;
		}
	}while(!exit);

	if(socket != INVALID_SOCKET){
		ZN_TcpUtils_CloseSocket(&socket);
	}


	if(host != NULL){
		ZN_FREE(host);
	}

	ZN_DeInit();

	return 0;
}
