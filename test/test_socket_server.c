#include "zetnet.h"


#define TCP_SERVER_DEFAULT_PORT 8100

bool GestMessagebool(ZN_TcpServer * tcp_server,ZN_SocketClient * client_socket, uint8_t *buffer, size_t len, void *user_data){

}

int main(int argc, char *argv[])
{
	char path[MAX_PATH]={"."};
	int port=TCP_SERVER_DEFAULT_PORT;
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
			}else if(strcmp(l->items[0],"--path")==0){
				strcpy(path,(const char *)l->items[1]);
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

	ZN_TcpServerOnGestMessage on_gest_message=(ZN_TcpServerOnGestMessage){
				.callback_function=GestMessage
	};

	ZN_TcpServer *tcp_server=ZN_TcpServer_New(on_gest_message);

	ZN_TcpServer_Start(tcp_server,port);

	ZN_DeInit();

	return 0;
}
