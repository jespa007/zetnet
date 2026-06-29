#ifndef __ZN_CONNECTION_H__
#define __ZN_CONNECTION_H__

typedef struct ZN_Connection ZN_Connection;

struct ZN_Connection{
    SOCKET socket;
#ifdef __WITH_SSL__
    SSL   *ssl;
#endif
    bool   use_ssl;
};

bool	ZN_Connection_Open(ZN_Connection *conn, const ZN_Url *url);
int 	ZN_Connection_Write(ZN_Connection *c, const uint8_t *data, int len);
int		ZN_Connection_Read(ZN_Connection *c, uint8_t *buf, int len);
void	ZN_Connection_Close(ZN_Connection *c);

#endif
