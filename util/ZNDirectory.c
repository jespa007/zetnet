#include "zetnet.h"

bool ZNDirectory_Exists(const char * filename){
	int status;
	struct stat st_buf;
	//int ret_stat;

	status = stat (filename, &st_buf);
	if (status != 0) {
		return false;
	}

	return S_ISDIR (st_buf.st_mode) != 0;
}

ZNList * ZNDirectory_ListFilesBuiltIn(const char * folder, ZNList *list_attribs, bool recursive){
	ZNList *list_file=ZNList_New();
	bool ok=false;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (folder)) != NULL) {
	  while ((ent = readdir (dir)) != NULL) {

		  if((strcmp(ent->d_name,".")!=0) && (strcmp(ent->d_name,"..")!=0)){ // invalid dirs...

				  char data[MAX_PATH]={0};
				  sprintf(data,"%s/%s",folder,ent->d_name);
				  if(ZNDirectory_Exists(data)){

					  if(recursive){
						  ZNList * r = ZNDirectory_ListFilesBuiltIn(data,list_attribs,true);
						  // add all resulting item elements to current list...
						  ZNList_Concat(list_file,r);

						  // r is not used any more, so delete it...
						  ZNList_Delete(r);
					  }
				  }
				  else{
					  ok=false;

					  for(unsigned i = 0; i < list_attribs->count && !ok; i++){

						  if((strcmp((char *)list_attribs->items[i],"*")==0) || ZNString_EndsWith(ent->d_name,(char *)list_attribs->items[i])) {
							  char *filename=ZN_MALLOC(strlen(data)+1); // 1 end string

							  strcpy(filename,data);
							  ZNList_Add(list_file,filename);
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

ZNList * ZNDirectory_ListFiles(const char * folder, const char * filter, bool recursive){
	ZNList * list_file=NULL;
	ZNList * list_attribs = ZNString_Split(filter==NULL?"*":filter, '|');

	for(unsigned i = 0; i < list_attribs->count; i++){
		ZNString_Remove((char *)list_attribs->items[i],' ');
		if(strcmp((char *)list_attribs->items[i],"*")!=0){
			ZNString_Remove((char *)list_attribs->items[i],'*');
		}
	}

	list_file=ZNDirectory_ListFilesBuiltIn(folder,list_attribs,recursive);

	ZNList_DeleteAndFreeAllItems(list_attribs);

	return list_file;

//	ZNDirectory_Exists("/tmp");


}
