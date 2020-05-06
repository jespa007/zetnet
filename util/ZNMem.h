#ifndef __ZNMEM_H__
#define __ZNMEM_H__

#define ZN_MALLOC(s) 	ZNMem_malloc(s,__FILE__,__LINE__)
#define ZN_FREE(s) 		free(s)



void * 	ZNMem_malloc(size_t size,const char *file, int line);


#endif
