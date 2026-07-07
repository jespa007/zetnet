#include "zetnet.h"


const char *ZN_SSL_GetBackendVersion(void)
{
#ifdef __WITH_SSL__
    return OpenSSL_version(OPENSSL_VERSION);
#else
    return "SSL disabled";
#endif
}

const char *ZN_SSL_GetBackendName(void)
{
#ifdef __WITH_SSL__
    return "OpenSSL";
#else
    return "None";
#endif
}

#ifdef ZN_WITH_SSL
#include "res/cert/cacert.pem.c"

SSL_CTX * ZN_GetSSLContext(void);

bool ZN_SSL_LoadEmbeddedCA(SSL_CTX *ctx) {
    if (!ctx) return false;

    BIO *bio = BIO_new_mem_buf(cacert_pem, cacert_pem_len);
    if (!bio) return false;

    X509_STORE *store = SSL_CTX_get_cert_store(ctx);
    if (!store) {
        BIO_free(bio);
        return false;
    }

    bool parsed_any = false;

    while (1) {
        X509 *cert = PEM_read_bio_X509(bio, NULL, 0, NULL);
        if (!cert) break;

        parsed_any = true;
        if (X509_STORE_add_cert(store, cert) != 1) {
            unsigned long err = ERR_peek_last_error();
            int reason = ERR_GET_REASON(err);

            /* Duplicate certificates are harmless when the OS/default store
             * already contains the same CA. Clear the OpenSSL error queue so
             * it does not leak into later diagnostics. */
            if (reason == X509_R_CERT_ALREADY_IN_HASH_TABLE) {
                ERR_clear_error();
            }
        }

        X509_free(cert);
    }

    ERR_clear_error();
    BIO_free(bio);
    return parsed_any;
}

bool ZN_SSL_VerifyHost(SSL *ssl, const char *hostname) {
    if (!ssl || !hostname) return false;

    X509 *cert = SSL_get_peer_certificate(ssl);
    if (!cert) return false;

    int ok = X509_check_host(cert, hostname, 0, 0, NULL);
    X509_free(cert);

    return ok == 1;
}

SSL* ZN_SSL_New(SOCKET sock, const char *hostname) {
    if (!hostname) return NULL;

    SSL *ssl = SSL_new(ZN_GetSSLContext());
    if (!ssl) {
        fprintf(stderr, "SSL_new failed\n");
        ERR_print_errors_fp(stderr);
        return NULL;
    }

#ifdef _WIN32
    if (sock > INT_MAX) {
        fprintf(stderr, "SOCKET value too large for SSL_set_fd\n");
        SSL_free(ssl);
        return NULL;
    }

    if (SSL_set_fd(ssl, (int)sock) != 1) {
#else
    if (SSL_set_fd(ssl, sock) != 1) {
#endif
        fprintf(stderr, "SSL_set_fd failed\n");
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        return NULL;
    }

    if (SSL_set_tlsext_host_name(ssl, hostname) != 1) {
        fprintf(stderr, "SSL_set_tlsext_host_name failed\n");
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        return NULL;
    }

    if (SSL_set1_host(ssl, hostname) != 1) {
        fprintf(stderr, "SSL_set1_host failed\n");
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        return NULL;
    }

    return ssl;
}

bool ZN_SSL_Connect(SSL *ssl) {
    if (!ssl) return false;

    int rc = SSL_connect(ssl);
    if (rc <= 0) {
        int ssl_error = SSL_get_error(ssl, rc);
        switch (ssl_error) {
            case SSL_ERROR_WANT_READ:
            case SSL_ERROR_WANT_WRITE:
                fprintf(stderr, "SSL_connect failed: WANT_READ/WANT_WRITE\n");
                break;
            default:
                fprintf(stderr, "SSL_connect failed: %s (%d)\n",
                        ERR_error_string(ERR_get_error(), NULL), ssl_error);
                break;
        }

        return false;
    }

    long verify_result = SSL_get_verify_result(ssl);
    if (verify_result != X509_V_OK) {
        fprintf(stderr, "SSL certificate verification failed: %s\n",
                X509_verify_cert_error_string(verify_result));
        return false;
    }

    return true;
}

int ZN_SSL_Write(SSL *ssl, const uint8_t *data, int len) {
    if (!ssl) return -1;
    return SSL_write(ssl, data, len);
}

int ZN_SSL_Read(SSL *ssl, uint8_t *buffer, int len) {
    if (!ssl) return -1;
    return SSL_read(ssl, buffer, len);
}

void ZN_SSL_Close(SSL *ssl) {
    if (!ssl) return;

    if (SSL_is_init_finished(ssl)) {
        SSL_shutdown(ssl);
    }

    SSL_free(ssl);
}
#endif
