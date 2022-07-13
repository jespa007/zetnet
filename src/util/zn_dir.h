#ifndef __ZN_DIR_H__
#define __ZN_DIR_H__


bool 		zn_dir_set_working_path(const char * path);
char 	* 	zn_dir_get_cwd(void);

bool	 	zn_dir_exists(const char * filename);
zn_list	* 	zn_dir_list_files(const char * folder, const char * filter, bool recursive);


#endif
