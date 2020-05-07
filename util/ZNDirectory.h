#ifndef __ZNDIRECTORY_H__
#define __ZNDIRECTORY_H__




bool	 	ZNDirectory_Exists(const char * filename);
ZNList 	* 	ZNDirectory_ListFiles(const char * folder, const char * filter, bool recursive);

#endif
