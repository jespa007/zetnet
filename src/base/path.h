#ifndef __ZN_PATH_H__
#define __ZN_PATH_H__

#ifndef MAX_PATH
#define MAX_PATH		4096
#endif

bool  ZN_Path_GetDirectoryName(char *dst,const char * _absolute_filename);
void  ZN_Path_GetFileName(char *dst,const char * _absolute_filename);



#endif
