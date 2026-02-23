#include "zetnet.h"

// sample socket client
// https://learn.microsoft.com/es-es/windows/win32/api/winsock/nf-winsock-recv

#define DEFAULT_PORT 27015

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

	const char *buf = "aaa";
	size_t send_bytes = 0;
	size_t buf_len = strlen(buf);

	do{
		switch(getchar()){
		case 's':
			if((socket = ZN_TcpUtils_NewSocketClient(host,port)) != INVALID_SOCKET){
				if((send_bytes = ZN_TcpUtils_SendBytes(socket,buf,strlen(buf)))< buf_len){
					fprintf(stderr,"buffer length %i < send bytes %i ",buf_len,send_bytes);
				}

				if((receive_bytes = ZN_TcpUtils_ReceiveBytes(socket,buf))< buf_len){
					fprintf(stderr,"buffer length %i < send bytes %i ",buf_len,send_bytes);
				}

				ZN_TcpUtils_CloseSocket(socket);
			}else{
				fprintf(stderr,"Invalid socket\n");
			}
			break;
		case 'q':
			exit = true;
			break;
		}
	}while(!exit);


	if(host != NULL){
		ZN_FREE(host);
	}

	ZN_DeInit();

	return 0;
}
