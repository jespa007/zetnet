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

		SSL_CTX_set_verify(g_ssl_ctx, SSL_VERIFY_PEER, NULL);
		SSL_CTX_set_default_verify_paths(g_ssl_ctx);
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
