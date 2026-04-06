#include "zetnet.h"


bool ZN_Connection_Open(ZN_Connection *conn, const ZN_Url *url) {
    memset(conn, 0, sizeof(*conn));

    conn->socket = ZN_TcpSocket_NewSocketClient(url->host, url->port);
    if (conn->socket == INVALID_SOCKET) {
    	return false;
    }

#ifdef __WITH_SSL__

    conn->use_ssl = (strcmp(url->scheme, "https") == 0);

    if (conn->use_ssl) {

        conn->ssl = ZN_SSL_New(conn->socket, url->host);

        if (!ZN_SSL_Connect(conn->ssl)) return false;

        if (!ZN_SSL_VerifyHost(conn->ssl, url->host)) {
            fprintf(stderr, "Hostname verification failed\n");
            return false;
        }
    }
#endif

    return true;
}

int ZN_Connection_Write(ZN_Connection *c, const uint8_t *data, int len) {
#ifdef __WITH_SSL__
    if (c->use_ssl) {
    	return ZN_SSL_Write(c->ssl, data, len);
    }
#endif
    return ZN_TcpSocket_SendBytes(c->socket, data, len);
}

int ZN_Connection_Read(ZN_Connection *c, uint8_t *buf, int len) {
#ifdef __WITH_SSL__
    if (c->use_ssl) return ZN_SSL_Read(c->ssl, buf, len);
#endif
    return ZN_TcpSocket_ReceiveBytes(c->socket, buf, len);
}

void ZN_Connection_Close(ZN_Connection *c) {
#ifdef __WITH_SSL__
    if (c->use_ssl && c->ssl) {
        ZN_SSL_Close(c->ssl);
    }
#endif

    if (c->socket != INVALID_SOCKET) {
        ZN_TcpSocket_CloseSocket(&c->socket);
    }
}
