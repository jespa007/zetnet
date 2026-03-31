#include "@zn_net.h"

SSL_CTX * ZN_GetSSLContext(void);

bool ZN_SSL_VerifyHost(SSL *ssl, const char *hostname) {
    X509 *cert = SSL_get_peer_certificate(ssl);
    if (!cert) return false;

    int ok = X509_check_host(cert, hostname, 0, 0, NULL);
    X509_free(cert);

    return ok == 1;
}

// new & connect ?
SSL* ZN_SSL_New(SOCKET sock, const char * hostname) {
    SSL *ssl = SSL_new(ZN_GetSSLContext());
    SSL_set_fd(ssl, sock);

    // set SNI
    SSL_set_tlsext_host_name(ssl, hostname);

    return ssl;
}

bool ZN_SSL_Connect(SSL *ssl) {
    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        return false;
    }
    return true;
}

int ZN_SSL_Write(SSL *ssl, const uint8_t *data, int len) {
    return SSL_write(ssl, data, len);
}

int ZN_SSL_Read(SSL *ssl, uint8_t *buffer, int len) {
    return SSL_read(ssl, buffer, len);
}

void ZN_SSL_Close(SSL *ssl) {
    if (!ssl) return;

    if (SSL_is_init_finished(ssl)) {
        SSL_shutdown(ssl);
    }

    SSL_free(ssl);
}
