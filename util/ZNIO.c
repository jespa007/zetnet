#include "zetnet.h"

#ifndef MAX_PATH
#define MAX_PATH		4096
#endif

bool ZNIO_FileExists(const char * filename) {

	if(filename == NULL) return false;

#ifdef _WIN32
	FILE *fp = fopen(filename,"r");

	if(fp == NULL) return false;

	fclose(fp);

	return true;
#else

	if(is_directory(m_file)) return false;


	struct stat buffer;
	  return (stat (m_file.c_str(), &buffer) == 0);
#endif

}

uint8_t * ZNIO_ReadFile(const char * filename,size_t * buffer_size){


	int  file_length, readed_elements;
	FILE  *fp;

	if((fp  =  fopen(filename,"rb"))  !=  NULL)
	{
		if((file_length = ZNIO_GetFileLength(filename)) != -1) {

			*buffer_size = file_length+1; // +1 is for string end

			uint8_t *buffer = (uint8_t *)malloc(*buffer_size);
			memset(buffer,0,*buffer_size );

			readed_elements = fread(buffer, 1, file_length, fp);

			if(readed_elements != file_length) {
				fprintf(stderr,"number elements doesn't match with length file (%s)\n",filename);
				free(buffer);
				return NULL;
			}

			fclose(fp);

			return buffer;
		}
		else  fprintf(stderr,"I can't read file \"%s\"\n",filename);
	}
	else  fprintf(stderr,"I can't open file \"%s\"\n",filename);


	return NULL;
}


int  ZNIO_GetFileLength(const char * filename)
{

	int  ini,  end;
	FILE  *fp;

	if((fp  =  fopen(filename,"rb"))  !=  NULL)
	{

		fseek(fp,  0,SEEK_SET);
		ini  =  ftell(fp);
		fseek(fp,  0,SEEK_END);
		end  =  ftell(fp);
		fclose(fp);

		return  (end  -  ini);
	}

	return    -1;
}

int  		ZNIO_GetFileLengthFromFile(FILE * file)
{

	int  ini,  end;
	FILE  *fp = file;

	if(fp != NULL)
	{
		long current = ftell(fp);

		fseek(fp,  0,SEEK_SET);
		ini  =  ftell(fp);
		fseek(fp,  0,SEEK_END);
		end  =  ftell(fp);

		fseek(fp,  current,SEEK_SET);
		return  (end  -  ini);
	}

	return    -1;
}

bool ZNIO_SetWorkPath(const char * path) {

	if(chdir(path)==-1) {
		fprintf(stderr,"Cannot change working path to %s\n", path);
		return false;
	}
	return true;
}

char * ZNIO_GetCwd(){
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

bool ZNIO_IsDirectory(const char * filename){
	int status;
	struct stat st_buf;
	//int ret_stat;

	status = stat (filename, &st_buf);
	if (status != 0) {
		return false;
	}

	return S_ISDIR (st_buf.st_mode) != 0;
}

ZNList * ZNIO_GetFilesBuiltIn(const char * folder, ZNList *list_attribs, bool recursive){
	ZNList *list_file=ZNList_New();
	bool ok=false;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (folder)) != NULL) {
	  while ((ent = readdir (dir)) != NULL) {

		  if((strcmp(ent->d_name,".")!=0) && (strcmp(ent->d_name,"..")!=0)){ // invalid dirs...

				  char data[MAX_PATH]={0};
				  sprintf(data,"%s/%s",folder,ent->d_name);
				  if(ZNIO_IsDirectory(data)){

					  if(recursive){
						  ZNList * r = ZNIO_GetFilesBuiltIn(data,list_attribs,true);
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
							  char *filename=malloc(strlen(data)+1); // 1 end string
							  memset(filename,0,strlen(data)+1);
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

ZNList * ZNIO_GetFiles(const char * folder, const char * filter, bool recursive){
	ZNList * list_file=NULL;
	ZNList * list_attribs = ZNString_Split(filter==NULL?"*":filter, '|');

	for(unsigned i = 0; i < list_attribs->count; i++){
		ZNString_Remove((char *)list_attribs->items[i],' ');
		if(strcmp((char *)list_attribs->items[i],"*")!=0){
			ZNString_Remove((char *)list_attribs->items[i],'*');
		}
	}

	list_file=ZNIO_GetFilesBuiltIn(folder,list_attribs,recursive);

	return list_file;

//	ZNIO_IsDirectory("/tmp");


}

