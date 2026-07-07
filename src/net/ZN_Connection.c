#include "zetnet.h"

static bool ZN_Connection_IsSecureScheme(const char *scheme) {
    return scheme != NULL &&
           (strcmp(scheme, "https") == 0 || strcmp(scheme, "wss") == 0);
}

bool ZN_Connection_Open(ZN_Connection *conn, const ZN_Url *url) {
    if (!conn || !url) return false;

    memset(conn, 0, sizeof(*conn));
    conn->socket = INVALID_SOCKET;

#ifndef ZN_WITH_SSL
    if (ZN_Connection_IsSecureScheme(url->scheme)) {
        fprintf(stderr, "ZetNet was compiled without SSL support\n");
        return false;
    }
#endif

    conn->socket = ZN_TcpSocket_NewSocketClient(url->host, url->port);
    if (conn->socket == INVALID_SOCKET) {
        return false;
    }

#ifdef ZN_WITH_SSL
    conn->use_ssl = ZN_Connection_IsSecureScheme(url->scheme);

    if (conn->use_ssl) {
        conn->ssl = ZN_SSL_New(conn->socket, url->host);
        if (!conn->ssl) {
            ZN_Connection_Close(conn);
            return false;
        }

        if (!ZN_SSL_Connect(conn->ssl)) {
            ZN_Connection_Close(conn);
            return false;
        }
    }
#endif

    return true;
}

ssize_t ZN_Connection_Write(ZN_Connection *c, const uint8_t *data, int len) {
    if (!c) return -1;

#ifdef ZN_WITH_SSL
    if (c->use_ssl) {
        return ZN_SSL_Write(c->ssl, data, len);
    }

#endif
    return ZN_TcpSocket_SendBytes(c->socket, data, len);
}

ssize_t ZN_Connection_Read(ZN_Connection *c, uint8_t *buf, int len) {
    if (!c) return -1;

#ifdef ZN_WITH_SSL
    if (c->use_ssl) {
        return ZN_SSL_Read(c->ssl, buf, len);
    }
#endif
    return ZN_TcpSocket_ReceiveBytes(c->socket, buf, len);
}

void ZN_Connection_Close(ZN_Connection *c) {
    if (!c) return;

#ifdef ZN_WITH_SSL
    if (c->use_ssl && c->ssl) {
        ZN_SSL_Close(c->ssl);
        c->ssl = NULL;
        c->use_ssl = false;
    }
#endif

    if (c->socket != INVALID_SOCKET) {
        ZN_TcpSocket_CloseSocket(&c->socket);
    }
}
