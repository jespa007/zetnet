#ifndef __ZN_PATH_H__
#define __ZN_PATH_H__

#ifndef MAX_PATH
#define MAX_PATH		4096
#endif

bool  ZN_Path_GetDirectoryName(char *dst, size_t dst_size,const char * _absolute_filename);
bool  ZN_Path_GetFileName(char *dst, size_t dst_size, const char *absolute_filename);



#endif
