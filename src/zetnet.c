#include "zetnet.h"

#ifdef ZN_WITH_SSL
static SSL_CTX * g_ssl_ctx = NULL;
#endif

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

#ifdef ZN_WITH_SSL
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

		SSL_CTX_set_verify(g_ssl_ctx, SSL_VERIFY_PEER, NULL);

	}
#endif

	return true;
}

const char *ZN_GetBuildInfo(void)
{
    static char info[256];

#ifdef ZN_WITH_SSL
    snprintf(info, sizeof(info),
             "ZetNet %d.%d.%d | SSL: %s",
             ZETNET_VERSION_MAJOR,
             ZETNET_VERSION_MINOR,
             ZETNET_VERSION_PATCH,
             OpenSSL_version(OPENSSL_VERSION));
#else
    snprintf(info, sizeof(info),
             "ZetNet %d.%d.%d | SSL: disabled",
             ZETNET_VERSION_MAJOR,
             ZETNET_VERSION_MINOR,
             ZETNET_VERSION_PATCH);
#endif

    return info;
}

#ifdef ZN_WITH_SSL
SSL_CTX * ZN_GetSSLContext(void){
	return g_ssl_ctx;
}

#endif

bool ZN_HasSSL(void)
{
#ifdef __WITH_SSL__
    return true;
#else
    return false;
#endif
}

void ZN_DeInit(void){
#ifdef ZN_WITH_SSL
	if(g_ssl_ctx != NULL){
		SSL_CTX_free(g_ssl_ctx);
        g_ssl_ctx = NULL;
	}
#endif

#ifdef _WIN32
	WSACleanup();
#endif


}
