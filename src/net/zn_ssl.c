#include "zetnet.h"
#include "res/cert/cacert.pem.c"

SSL_CTX * ZN_GetSSLContext(void);

bool ZN_SSL_LoadEmbeddedCA(SSL_CTX *ctx) {
    BIO *bio = BIO_new_mem_buf(cacert_pem, cacert_pem_len);
    if (!bio) return false;

    X509_STORE *store = SSL_CTX_get_cert_store(ctx);
    if (!store) return false;

    while (1) {
        X509 *cert = PEM_read_bio_X509(bio, NULL, 0, NULL);
        if (!cert) break;

        X509_STORE_add_cert(store, cert);
        X509_free(cert);
    }

    BIO_free(bio);
    return true;
}

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

    if (!ssl) {
        fprintf(stderr,"SSL_new failed\n");
        ERR_print_errors_fp(stderr);
        return NULL;
    }

    if (SSL_set_fd(ssl, sock) != 1) {
        fprintf(stderr,"SSL_set_fd failed\n");
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        return NULL;
    }

    // set SNI
    if (SSL_set_tlsext_host_name(ssl, hostname) != 1) {
        printf("SSL_set_tlsext_host_name failed\n");
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        return NULL;
    }

    printf("Verify result: %ld\n", SSL_get_verify_result(ssl));


    return ssl;
}

bool ZN_SSL_Connect(SSL *ssl) {
    int ssl_error = SSL_connect(ssl);
	if (ssl_error <= 0) {
        ssl_error = SSL_get_error(ssl, ssl_error);
        switch (ssl_error) {
            case SSL_ERROR_WANT_READ:
            case SSL_ERROR_WANT_WRITE:
            	 fprintf(stderr,"SSL_connect failed : WANT READ/WRITE");
                break;
            default:
                ERR_load_crypto_strings();
                fprintf(stderr,"SSL_connect failed  %s:%d",  ERR_error_string(ERR_get_error(), NULL),ssl_error);
                ERR_free_strings();
                break;
        }

		return false;
	}

	printf("SSL connected successfully\n");

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
