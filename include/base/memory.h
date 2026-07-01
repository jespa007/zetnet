#ifndef __ZN_MEMORY_H__
#define __ZN_MEMORY_H__

#define ZN_MALLOC(s) 		zn_malloc(s,__FILE__,__LINE__)
#define ZN_NEW(s)			zn_malloc(sizeof(s),__FILE__,__LINE__)
#define ZN_NEW_LENGTH(S,N) ((S*)zn_malloc(sizeof(S)*(N),__FILE__,__LINE__))
#define ZN_FREE(s) \
    do { \
        free(s); \
        (s) = NULL; \
    } while (0)



void * 	zn_malloc(size_t size,const char *file, int line);


#endif
