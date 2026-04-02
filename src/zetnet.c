#include "zetnet.h"


static SSL_CTX * g_ssl_ctx = NULL;

bool ZN_Init(void){
#ifdef _WIN32
	WSADATA wsaData;
	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		fprintf(stderr,"\nWSAStartup failed with error: %d\n", iResult);
		return false;
	}
#endif

#ifdef __SSL__
    // OpenSSL init
	if(g_ssl_ctx == NULL){
		SSL_library_init();
		SSL_load_error_strings();

		g_ssl_ctx = SSL_CTX_new(TLS_client_method());
		if (!g_ssl_ctx) {
			fprintf(stderr,"SSL_CTX_new failed\n");
			return false;
		}

		if(SSL_CTX_set_default_verify_paths(g_ssl_ctx) == 0){
			fprintf(stderr,"SSL_CTX_set_default_verify_paths failed\n");
			return false;
		}


		if (!ZN_SSL_LoadEmbeddedCA(g_ssl_ctx)) {
		    fprintf(stderr, "Failed to load embedded CA\n");
		    return false;
		}

		/*if (!SSL_CTX_load_verify_locations(g_ssl_ctx, NULL, NULL)) {
		    fprintf(stderr, "Failed to load CA bundle\n");
		    return false;
		}

		---
		BIO *bio = BIO_new_mem_buf(cacert_pem, cacert_pem_len);

		X509_STORE *store = SSL_CTX_get_cert_store(ctx);

		STACK_OF(X509_INFO) *infos = PEM_X509_INFO_read_bio(bio, NULL, NULL, NULL);

		for (int i = 0; i < sk_X509_INFO_num(infos); i++) {
			X509_INFO *it = sk_X509_INFO_value(infos, i);

			if (it->x509) {
				X509_STORE_add_cert(store, it->x509);
			}
		}

		sk_X509_INFO_pop_free(infos, X509_INFO_free);
		BIO_free(bio);
		//---*/

		SSL_CTX_set_verify(g_ssl_ctx, SSL_VERIFY_PEER, NULL);


	}
#endif

	return true;
}

#ifdef __SSL__
SSL_CTX * ZN_GetSSLContext(void){
	return g_ssl_ctx;
}

#endif

void ZN_DeInit(void){
#ifdef __SSL__
	if(g_ssl_ctx != NULL){
		SSL_CTX_free(g_ssl_ctx);
        g_ssl_ctx = NULL;
	}
#endif

#ifdef _WIN32
	WSACleanup();
#endif


}
