#ifndef __ZNIO_H__
#define __ZNIO_H__

bool 		ZNIO_FileExists(const char * filename);
uint8_t * 	ZNIO_ReadFile(const char * filename, size_t * buffer_size);
int  		ZNIO_GetFileLength(const char * filename);

bool 		ZNIO_SetWorkPath(const char * path);
char 	* 	ZNIO_GetCwd();
bool	 	ZNIO_IsDirectory(const char * filename);
ZNList 	*	ZNIO_GetFiles(const char * folder, const char * filter, bool recursive);


#endif
