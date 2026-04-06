#include "../src/zetnet.h"

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

	printf("\nTCP Socket client test ver x.x.x\n");
	printf("\nHost %s:%i\n",host==NULL?"localhost":host,port);
	printf(
		"[o]pen socket\n"
		"[c]lose socket\n"
		"[s]end data\n"
		"[q]uit\n"
	);

	SOCKET socket = INVALID_SOCKET;

	uint8_t buffer[BUFFER_LEN];

	do{

		switch(getchar()){
		case 'o':
			// close socket
			if((socket = ZN_TcpSocket_NewSocketClient(host,port)) != INVALID_SOCKET){
				printf("Socket opened\n");
			}else{
				fprintf(stderr,"Cannot create socket\n");
			}
			break;
		case 'c':
			// close socket
			if(socket != INVALID_SOCKET){
				ZN_TcpSocket_CloseSocket(&socket);
				printf("Socket closed\n");
			}else{
				fprintf(stderr,"Cannot close socket. Socket not open!\n");
			}
			break;
		case 's':
			// open socket to transmit as many bytes as the client want to send
			if(socket != INVALID_SOCKET){
				printf("\nEnter data to send : ");
				fflush(stdout);
				memset(buffer,0,BUFFER_LEN);
				// read characters until press enter
				if(read(STDIN_FILENO, buffer, BUFFER_LEN-1) > 0){
					printf("\nsending response (%s)...\n",buffer);
					int bytes_to_send = (int)strlen((const char *)buffer);
					int offset = 0;
					int bytes_sent = 0;

					do{
						bytes_sent = ZN_TcpSocket_SendBytes(socket,buffer+offset,bytes_to_send);

						if(bytes_sent != ZN_ERROR){
							bytes_to_send-=bytes_sent;
							offset+=bytes_sent;
						}

					}while(bytes_sent != ZN_ERROR && bytes_to_send > 0);
				}

				// try to receive the bytes send (echo)
				printf("\nawaitting response ...\n");
				int received_bytes = 0;
				do{
					// there's timeout ?
					int received_bytes = ZN_TcpSocket_ReceiveBytes(socket,buffer,BUFFER_LEN);

					for(int r = 0; r < received_bytes; r++){
						putc(buffer[r],stdout);
					}

				}while(received_bytes != ZN_ERROR && received_bytes > 0);

			}else{
				fprintf(stderr,"You must open socket first before send data\n");
			}
			break;
		case 'q':
			exit = true;
			break;
		}
	}while(!exit);

	if(socket != INVALID_SOCKET){
		ZN_TcpSocket_CloseSocket(&socket);
	}

	if(host != NULL){
		ZN_FREE(host);
	}

	ZN_DeInit();

	return 0;
}
