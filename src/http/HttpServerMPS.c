#include "zetnet.h"

#define HTTP_SERVER_MPS_BUFFER	2

static const char * HTTP_SERVER_MPS_HEADER=

    "HTTP/1.0 200 OK\n"
    "Cache-Control: no-cache\n"
	"Cache-Control: private\n"
	"Content-Type: multipart/x-mixed-replace;boundary=--boundary\n\n";

static const unsigned char CRLF[] 		= { 10 };
static const unsigned char EMPTY_LINE[]	= { 10, 10};


typedef struct {
	TcpServer *	tcp_server;
	unsigned long time_delay_ms;
	uint8_t		n_read,n_write;
	size_t		data_len[HTTP_SERVER_MPS_BUFFER];
	uint8_t		*data_ptr[HTTP_SERVER_MPS_BUFFER];
}HttpServerMPSData;


bool HttpServerMPS_OnGestMessage(TcpServer * tcp_server,SocketClient * client_socket, uint8_t *buffer, size_t buffer_len, void *user_data){
	ZN_UNUSUED_PARAM(buffer);
	ZN_UNUSUED_PARAM(buffer_len);

	HttpServerMPS * http_server_mps=user_data;
	HttpServerMPSData *data=http_server_mps->data;

	if(data->n_read != data->n_write){ // time to send data ...

		char *buffer=NULL;
		int buffer_len = 0;
		char *start_buffer=NULL;
		char http_header[128]={0};
		size_t   data_len=data->data_len[data->n_read];
		uint8_t *data_ptr=data->data_ptr[data->n_read];

		sprintf(http_header,
			"--boundary\r\n"
			"Content-Type: image/jpeg\r\n"
			"Content-Length: %lu\r\n\r\n"
	        , (unsigned long)data_len);


		buffer_len=strlen(http_header)+data_len+2;

		start_buffer = buffer = (char *)malloc(buffer_len);
		memset(start_buffer,0,buffer_len);

		memcpy(buffer, http_header, strlen(http_header));
		buffer+=strlen(http_header);
		memcpy(buffer, data_ptr, data_len);
		buffer+=data_len;
		memcpy(buffer, "\n", 1);


		// send header...
		if(!client_socket->streaming_header_sent){
			if(!TcpUtils_SendBytes(client_socket->socket,(uint8_t *)HTTP_SERVER_MPS_HEADER, strlen(HTTP_SERVER_MPS_HEADER))){
#if __DEBUG__
				fprintf(stderr,"HttpServerMPS_TcpServerOnGestMessage:Erasing client (Stream)\n");
#endif
				TcpServer_CloseSocketClient(tcp_server,client_socket);

			}

			client_socket->streaming_header_sent = true;
		}

		// send streaming...
		if(!TcpUtils_SendBytes(client_socket->socket,(uint8_t *)start_buffer, buffer_len)){
#if __DEBUG__
			fprintf(stderr,"gestMessage:Erasing client (Stream)\n");
#endif
			TcpServer_CloseSocketClient(tcp_server,client_socket);
		}


		ZN_FREE(start_buffer);
		ZN_FREE(data->data_ptr[data->n_read]);
		data->data_ptr[data->n_read]=NULL;
		data->n_read = (data->n_read+1)&0x1; // message readed!
	}

	return true;
}



HttpServerMPS *	HttpServerMPS_New(){
	HttpServerMPS * http_server_mps=ZN_MALLOC(sizeof(HttpServerMPS));
	HttpServerMPSData *data=ZN_MALLOC(sizeof(HttpServerMPSData));
	http_server_mps->data=data;

	return http_server_mps;
}

void 				HttpServerMPS_SetTimeDelay(HttpServerMPS 	*_this, unsigned long time_delay_ms ){
	HttpServerMPSData *data=_this->data;
	data->time_delay_ms=time_delay_ms;
}

void 	HttpServerMPS_Start(HttpServerMPS	*_this, int port){
	HttpServerMPSData *data=_this->data;

	if(data->tcp_server==NULL){

		TcpServerOnGestMessage on_gest_message=(TcpServerOnGestMessage){
					.callback_function=HttpServerMPS_OnGestMessage
					,.user_data=_this
				};
		data->tcp_server=TcpServer_New(on_gest_message);
		data->tcp_server->time_delay_ms=data->time_delay_ms;
		TcpServer_Start(data->tcp_server,port);

	}else{
		//print_error("HttpServerMPS already started!");
	}
}

void 	HttpServerMPS_Stop(HttpServerMPS 	*_this){
	HttpServerMPSData *data = _this->data;
	if(data->tcp_server!=NULL){
		TcpServer_Delete(data->tcp_server);
		data->tcp_server=NULL;
	}else{
		//Log_Error("HttpServerMPS not started!");
	}
}

bool				HttpServerMPS_CanWrite(HttpServerMPS	*_this){
	HttpServerMPSData *data=_this->data;
	return ((data->n_write+1)&0x1)!= data->n_read;
}

void				HttpServerMPS_Write(HttpServerMPS	*_this, uint8_t *ptr_src, size_t len){
	HttpServerMPSData *data=_this->data;
	if(HttpServerMPS_CanWrite(_this)){
		void *ptr_dst=ZN_MALLOC(len);
		memcpy(ptr_dst,ptr_src,len);

		data->data_ptr[data->n_write] = ptr_dst;
		data->data_len[data->n_write] = len;

		data->n_write = (data->n_write+1)&0x1;
	}
}



void 			HttpServerMPS_Delete(HttpServerMPS *_this){
	if(_this == NULL) return;

	HttpServerMPSData *data=_this->data;
	TcpServer_Delete(data->tcp_server);

	ZN_FREE(data);
	ZN_FREE(_this);

}
