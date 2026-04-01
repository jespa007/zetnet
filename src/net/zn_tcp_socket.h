#ifndef __ZN_TCP_SOCKET_H__
#define __ZN_TCP_SOCKET_H__

#define ZN_ERROR			-1
#define ZN_INVALID_SOCKET 	-1

#ifdef _WIN32
    #define ZN_CHANNEL_RECEIVE	SD_RECEIVE
    #define ZN_CHANNEL_SEND   	SD_SEND
    #define	ZN_CHANNEL_BOTH 	SD_BOTH

    #define ZN_WOULD_BLOCK 		WSAEWOULDBLOCK
    #define ZN_GET_ERROR()		WSAGetLastError()
#else
    #define ZN_CHANNEL_RECEIVE	SHUT_RD
    #define ZN_CHANNEL_SEND   	SHUT_WR
    #define	ZN_CHANNEL_BOTH 	SHUT_RDWR

    #define ZN_WOULD_BLOCK EWOULDBLOCK
    #define ZN_GET_ERROR() errno
#endif

//#define  ZN_TCP_MESSAGE_LENGTH 	16384


SOCKET 	ZN_TcpUtils_NewSocketClient(const char * _host, int port);
SOCKET 	ZN_TcpUtils_NewSocketServer(const char * _host,int port);
int  	ZN_TcpUtils_ReceiveBytes(SOCKET  sock,  uint8_t  *_buf, size_t _buf_len);
int 	ZN_TcpUtils_SendBytes(SOCKET  sock,  const uint8_t  * _buf,  size_t  _buf_len);
void	ZN_TcpUtils_CloseChannel(SOCKET  sock, int channel);
void 	ZN_TcpUtils_CloseSocket(SOCKET *sock);

#endif
