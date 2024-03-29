#include "zetnet.h"


void * zn_malloc(size_t size,const char *file, int line){
	void * p=NULL;
#ifdef __MEMMANAGER__
	p=MEMMGR_malloc(size,file,line);
#else
	ZN_UNUSUED_PARAM(file);
	ZN_UNUSUED_PARAM(line);
	p=malloc(size);
#endif

	if(p==NULL){
		fprintf(stderr,"\nCannot allocate %lu\n",(unsigned long)size);
		return NULL;
	}

	memset(p,0,size);

	return p;
}


