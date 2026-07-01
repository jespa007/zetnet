#ifndef __ZN_FILE_H__
#define __ZN_FILE_H__

bool 		ZN_File_Exists(const char * filename);
uint8_t * 	ZN_File_Read(const char * filename, size_t * buffer_size);
int  		ZN_File_Length(const char * filename);



#endif
