#ifndef __ZNPATH_H__
#define __ZNPATH_H__

#ifndef MAX_PATH
#define MAX_PATH		4096
#endif

bool  ZNPath_GetDirectory(char *dst,const char * _absolute_filename);
void  ZNPath_GetFilename(char *dst,const char * _absolute_filename);



#endif
