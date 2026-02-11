#include "zetnet.h"

#define BUFFER_SIZE 1024
#define DEFAULT_PORT 80

int main(int argc, char *argv[])
{
	char url[MAX_PATH]={""};
	int port=DEFAULT_PORT;
	bool send_same_site_attribute=false;
	bool error=false;

	for(int i=1; i < argc && !error; i++){
		ZN_List *l=ZN_String_Split(argv[i],'=');
		switch(l->count){
		case 1:
			if(strcmp(l->items[0],"--send_same_site_attribute")==0){
				send_same_site_attribute=true;
			}else{
				fprintf(stderr,"unknow option %s\n",(const char *)l->items[0]);
				error=true;
			}
			break;
		case 2:
			if(strcmp(l->items[0],"--port")==0){
				error=!ZN_String_ToInt(&port,l->items[1],10);
			}else if(strcmp(l->items[0],"--url")==0){
				strcpy(url,(const char *)l->items[1]);
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

	// perform http request...
	SOCKET 	socket = ZN_TcpUtils_NewSocketClient(url,port);
	char buffer[BUFFER_SIZE];

	ZN_TcpUtils_SendBytes(socket, (uint8_t*)"GET /\r\n", strlen("GET /\r\n")); // write(fd, char[]*, len);


	while(ZN_TcpUtils_ReceiveBytes(socket, (uint8_t *)buffer, BUFFER_SIZE - 1) != 0){
			fprintf(stderr, "%s", buffer);
			memset(buffer,0, BUFFER_SIZE);
		}
	ZN_DeInit();

	return 0;
}
