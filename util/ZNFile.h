#ifndef __ZNFILE_H__
#define __ZNFILE_H__

bool 		ZNFile_Exists(const char * filename);
uint8_t * 	ZNFile_Read(const char * filename, size_t * buffer_size);
int  		ZNFile_Length(const char * filename);



#endif
