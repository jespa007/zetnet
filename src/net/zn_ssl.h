#ifndef __ZN_SSL__
#define __ZN_SSL__


bool ZN_SSL_VerifyHost(SSL *ssl, const char *hostname);
bool ZN_SSL_LoadEmbeddedCA(SSL_CTX *ctx);

SSL* ZN_SSL_New(SOCKET sock, const char * hostname);

bool ZN_SSL_Connect(SSL *ssl);

int ZN_SSL_Write(SSL *ssl, const uint8_t *data, int len);

int ZN_SSL_Read(SSL *ssl, uint8_t *buffer, int len);

void ZN_SSL_Close(SSL *ssl);

#endif
