#include "zetnet.h"



#define DEFAULT_PORT 27015

// MAY BE YES! DO MAX BLOCK LENGTH TO AVOID MALLOCS/FREE A LOT OF TIME!
bool GestMessage(ZN_TcpServer * _tcp_server,ZN_TcpServerClient * _client, uint8_t * _buffer, size_t _buffer_len, void * _user_data){

	ZN_UNUSED_2PARAMS(_tcp_server, _user_data);

	bool ok = true;


	size_t send_len = _buffer_len+1;

	// do echo...
	size_t send_bytes = ZN_TcpUtils_SendBytes(_client->socket,_buffer,_buffer_len);

	if(send_bytes<send_len){
		fprintf(stderr, "Error sending bytes send_bytes %i < buffer_len %i\n",(int)send_bytes,(int)_buffer_len);
		ok = false;
	}

	return ok;
}

int main(int argc, char *argv[])
{
	int port=DEFAULT_PORT;
	bool error=false;

	for(int i=1; i < argc && !error; i++){
		ZN_List *l=ZN_String_Split(argv[i],'=');
		switch(l->count){
		case 1:
			fprintf(stderr,"unknow option %s\n",(const char *)l->items[0]);
			error=true;
			break;
		case 2:
			if(strcmp(l->items[0],"--port")==0){
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

	ZN_TcpServerOnGestMessage on_gest_message=(ZN_TcpServerOnGestMessage){
		.callback_function=GestMessage
	};

	ZN_TcpServer *tcp_server=ZN_TcpServer_New(on_gest_message);

	if(ZN_TcpServer_Start(tcp_server,port)){

		printf("press any key to stop ...\n");
		getchar();
	}


	ZN_DeInit();

	return 0;
}
