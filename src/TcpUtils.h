#ifndef __TCP_UTILS_H__
#define __TCP_UTILS_H__

#define  ZETNET_TCP_MESSAGE_LENGTH 	16384

SOCKET TcpUtils_NewSocketClient(int port);
SOCKET TcpUtils_NewSocketServer(int port);
int  TcpUtils_ReceiveBytes(SOCKET  sock,  uint8_t  *buf);
int  TcpUtils_SendBytes(SOCKET  sock,  uint8_t  *buf,  uint32_t  len);
void TcpUtils_CloseSocket(SOCKET *sock);

#endif
