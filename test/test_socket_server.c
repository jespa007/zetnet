#include "zetnet.h"



#define DEFAULT_PORT 27015

// MAY BE YES! DO MAX BLOCK LENGTH TO AVOID MALLOCS/FREE A LOT OF TIME!
// IMAGINE 1000000 conections each one of 1000000 bytes to allocate ?!?

bool GestMessage(ZN_TcpServer * _tcp_server,ZN_SocketClient * _client_socket, uint8_t * _buffer, size_t _buffer_len, void * _user_data){
	bool ok = true;

	const char * echo_msg = "echo message :";
	size_t echo_msg_len = strlen(echo_msg);

	size_t send_len = _buffer_len+echo_msg_len+1;

	// do echo...
	char *msg_reply = ZN_MALLOC(send_len);

	strncat(msg_reply,echo_msg,echo_msg_len);
	strncpy(msg_reply+echo_msg_len,_buffer,_buffer_len);

	size_t send_bytes = ZN_TcpUtils_SendBytes(_client_socket,msg_reply,send_len);

	if(send_bytes<send_len){
		fprintf("Error sending bytes send_bytes %i < buffer_len %i\n",send_bytes,send_len);
	}

	ZN_FREE(msg_reply);

	return false;
}

int main(int argc, char *argv[])
{
	char path[MAX_PATH]={"."};
	int port=DEFAULT_PORT;
	bool send_same_site_attribute=false;
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

	if(ZN_HttpServer_Start(tcp_server,port)){

		printf("press any key to stop ...\n");
		getchar();
	}


	ZN_DeInit();

	return 0;
}
