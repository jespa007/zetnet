#ifndef __ZNPATH_H__
#define __ZNPATH_H__

#ifndef MAX_PATH
#define MAX_PATH		4096
#endif

bool  zn_path_get_directory_name(char *dst,const char * _absolute_filename);
void  zn_path_get_file_name(char *dst,const char * _absolute_filename);



#endif
