#ifndef __ZN_TCP_UTILS_H__
#define __ZN_TCP_UTILS_H__

#define  ZN_TCP_MESSAGE_LENGTH 	16384

SOCKET 	ZN_TcpUtils_NewSocketClient(int port);
SOCKET 	ZN_TcpUtils_NewSocketServer(int port);
int  	ZN_TcpUtils_ReceiveBytes(SOCKET  sock,  uint8_t  *buf);
int  	ZN_TcpUtils_SendBytes(SOCKET  sock,  uint8_t  *buf,  uint32_t  len);
void 	ZN_TcpUtils_CloseSocket(SOCKET *sock);

#endif
