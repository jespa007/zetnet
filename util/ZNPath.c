#include "zetnet.h"

#ifndef MAX_FILENAME_LENGTH
#define MAX_FILENAME_LENGTH 100
#endif

#ifndef MAX_PATH_LENGTH
#define MAX_PATH_LENGTH 2096
#endif

#ifndef MAX_ABSOLUTE_FILENAME_LENGTH
#define MAX_ABSOLUTE_FILENAME_LENGTH (MAX_FILENAME_LENGTH+MAX_PATH_LENGTH)
#endif

bool ZNPath_GetDirectory(char *ss, const char * _absolute_filename) {
	//static char ss[MAX_ABSOLUTE_FILENAME_LENGTH]={0};
	char * found=strrchr(_absolute_filename,'\\'); // locate the last ocurrence of '\\'
	if(found==NULL){
		found=strrchr(_absolute_filename,'/'); // locate the last ocurrence of '/'
	}

	if(found != NULL){
		size_t len = found-_absolute_filename;
		if(len < MAX_ABSOLUTE_FILENAME_LENGTH && len > 0){
			memcpy(ss,_absolute_filename,len);
			return true;
		}
		else{
			fprintf(stderr,"Reached MAX_ABSOLUTE_FILENAME_LENGTH or 0 ('%s':%i)",_absolute_filename,len);
		}
	}
	else{
		fprintf(stderr,"Cannot get folder name on '%s'",_absolute_filename);
	}

	return false;
	//return ss;
}

bool ZNPath_GetFilename(char *ss,const char * _absolute_filename) {
	//static char ss[MAX_FILENAME_LENGTH]={0};
	//size_t len=strlen(_absolute_filename);
	char * found=strrchr(_absolute_filename,'\\'); // locate the last ocurrence of '\\'

	if(found==NULL){
		found=strrchr(_absolute_filename,'/'); // locate the last ocurrence of '/'
	}
	if(found == NULL){ // no folder
		strcpy(ss,_absolute_filename);
	}

	if(found!=NULL){
		strcpy(ss,found+1);
		return true;
	}else{
		fprintf(stderr,"cannot get filename from '%s'",_absolute_filename);
	}

	return false;
//	return ss;
}

bool  ZNPath_GetFilenameWithoutExtension(char *ss,const char * _absolute_filename) {
	//static char ss[MAX_FILENAME_LENGTH]={0};
	char filename_aux[MAX_PATH]="";
	char *last_dot_found = NULL;

	size_t len;

	if(ZNPath_GetFilename(filename_aux,_absolute_filename)){

	if(filename_aux == NULL) return NULL;

		last_dot_found=strrchr(filename_aux,'.');

		if(last_dot_found==NULL){
			last_dot_found=(char *)filename_aux+strlen(filename_aux);
		}

		len =last_dot_found-filename_aux;
		if(len<MAX_FILENAME_LENGTH && len > 0){
			memcpy(ss,filename_aux,len);
			return true;
		}else{
			fprintf(stderr,"Reached MAX_FILENAME_LENGTH or 0 ('%s':%i)",filename_aux,len);
		}
	}
	return false;
}



