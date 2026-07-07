#ifndef __ZN_URL__
#define __ZN_URL__

typedef struct {
    char scheme[8];    // "http" / "https"
    char host[256];
    int  port;
    char path[1024];
} ZN_Url;

ZN_DLL_EXPORT bool 		ZN_Url_Parse(const char *url_str, ZN_Url *out);
ZN_DLL_EXPORT char * 	ZN_Url_Escape(const char * s);
ZN_DLL_EXPORT char * 	ZN_Url_Unescape(const char * s);

#endif
