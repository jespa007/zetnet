#ifndef __ZN_FILE_H__
#define __ZN_FILE_H__

bool 		zn_file_exists(const char * filename);
uint8_t * 	zn_file_read(const char * filename, size_t * buffer_size);
int  		zn_file_length(const char * filename);



#endif
