#ifndef __ZNPATH_H__
#define __ZNPATH_H__


bool  ZNPath_GetDirectory(char *dst,const char * _absolute_filename);
bool  ZNPath_GetFilename(char *dst,const char * _absolute_filename);
bool  ZNPath_GetFilenameWithoutExtension(char *dst,const char * _absolute_filename);


#endif
