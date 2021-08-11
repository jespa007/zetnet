#include "zetnet.h"

bool zn_dir_exists(const char * filename){
	int status;
	struct stat st_buf;
	//int ret_stat;

	status = stat (filename, &st_buf);
	if (status != 0) {
		return false;
	}

	return S_ISDIR (st_buf.st_mode) != 0;
}

zn_list * zn_dir_list_files_builtin(const char * folder, zn_list *list_attribs, bool recursive){
	zn_list *list_file=zn_list_new();
	bool ok=false;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (folder)) != NULL) {
	  while ((ent = readdir (dir)) != NULL) {

		  if((strcmp(ent->d_name,".")!=0) && (strcmp(ent->d_name,"..")!=0)){ // invalid dirs...

				  char data[MAX_PATH]={0};
				  sprintf(data,"%s/%s",folder,ent->d_name);
				  if(zn_dir_exists(data)){

					  if(recursive){
						  zn_list * r = zn_dir_list_files_builtin(data,list_attribs,true);
						  // add all resulting item elements to current list...
						  zn_list_concat(list_file,r);

						  // r is not used any more, so delete it...
						  zn_list_delete(r);
					  }
				  }
				  else{
					  ok=false;

					  for(unsigned i = 0; i < list_attribs->count && !ok; i++){

						  if((strcmp((char *)list_attribs->items[i],"*")==0) || zn_str_ends_with(ent->d_name,(char *)list_attribs->items[i])) {
							  char *filename=ZN_MALLOC(strlen(data)+1); // 1 end string

							  strcpy(filename,data);
							  zn_list_add(list_file,filename);
							  ok=true;
						  }
					  }
				  }
			  }
	  }
	  closedir (dir);
	} else {
		fprintf(stderr,"could not open directory %s\n",folder);
	}

	return list_file;
}

zn_list * zn_dir_list_files(const char * folder, const char * filter, bool recursive){
	zn_list * list_file=NULL;
	zn_list * list_attribs = zn_str_split(filter==NULL?"*":filter, '|');

	for(unsigned i = 0; i < list_attribs->count; i++){
		zn_str_erase_char((char *)list_attribs->items[i],' ');
		if(strcmp((char *)list_attribs->items[i],"*")!=0){
			zn_str_erase_char((char *)list_attribs->items[i],'*');
		}
	}

	list_file=zn_dir_list_files_builtin(folder,list_attribs,recursive);

	zn_list_delete_and_free_all_items(list_attribs);

	return list_file;

//	zn_dir_exists("/tmp");
}

bool zn_dir_set_working_path(const char * path) {

	if(chdir(path)==-1) {
		fprintf(stderr,"Cannot change working path to %s\n", path);
		return false;
	}
	return true;
}

char * zn_dir_get_cwd(){
	static char cwd[MAX_PATH]={0};
#ifdef __GNUC__
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		 perror("getcwd() error");
#else
	#ifdef _WIN32
	GetModuleFileName( NULL, cwd, MAX_PATH );
	#else
		#error "getCwd not supported!"
	#endif
#endif

	return cwd;
}
