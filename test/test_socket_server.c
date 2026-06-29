#include "../src/zetnet.h"


#define DEFAULT_PORT 27015

bool GestMessage(ZN_TcpServer * _tcp_server,ZN_TcpServerClient * _client, uint8_t * _buffer, size_t _buffer_len, void * _user_data){

	ZN_UNUSED_2PARAMS(_tcp_server, _user_data);

	bool ok = true;

	int bytes_to_send = _buffer_len;
	int offset = 0;
	int bytes_sent = 0;

	printf("sending echo\n");

	do{
		bytes_sent = ZN_TcpSocket_SendBytes(_client->socket,_buffer+offset,bytes_to_send);

		if(bytes_sent != ZN_ERROR){
			bytes_to_send-=bytes_sent;
			offset+=bytes_sent;
		}

	}while(bytes_sent != ZN_ERROR && bytes_to_send > 0);

	printf("done\n");

	return ok;
}

int main(int argc, char *argv[])
{
	int port=DEFAULT_PORT;
	char *host = NULL;
	bool error=false;

	for(int i=1; i < argc && !error; i++){
		ZN_List *l=ZN_CStr_Split(argv[i],'=');
		switch(l->count){
		case 1:
			fprintf(stderr,"unknow option %s\n",(const char *)l->items[0]);
			error=true;
			break;
		case 2:
			if(strcmp(l->items[0],"--host")==0){
				if(host == NULL){
					host=strdup(l->items[1]);
				}
			}else if(strcmp(l->items[0],"--port")==0){
				error=!ZN_CStr_ToInt(&port,l->items[1],10);
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

	if(ZN_TcpServer_Start(tcp_server,host==NULL?"127.0.0.1":host,port)){

		printf("press any key to stop ...\n");
/*#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
		Sleep( 1000 );
#else
		usleep( 1000000 );
#endif*/
		getchar();
	}

	if(host != NULL){
		ZN_FREE(host);
	}

	ZN_TcpServer_Delete(tcp_server);

	ZN_DeInit();

	return 0;
}
