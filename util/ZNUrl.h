#ifndef __ZNURL__
#define __ZNURL__

char * ZNURL_Escape(const char * s, const unsigned char mask); // mask=0xff
char * ZNURL_Unescape(const char * s);

#endif
