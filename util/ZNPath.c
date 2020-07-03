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
			fprintf(stderr,"Reached MAX_ABSOLUTE_FILENAME_LENGTH or 0 ('%s':%lu)",_absolute_filename,(unsigned long)len);
		}
	}
	else{
		fprintf(stderr,"Cannot get folder name on '%s'",_absolute_filename);
	}

	return false;
	//return ss;
}

void ZNPath_GetFilename(char *ss,const char * _absolute_filename) {
	//static char ss[MAX_FILENAME_LENGTH]={0};
	//size_t len=strlen(_absolute_filename);
	char * found1=strrchr(_absolute_filename,'\\'); // locate the last ocurrence of '\\'
	char * found2=strrchr(_absolute_filename,'/'); // locate the last ocurrence of '\\'
	char * found=found1>found2?found1:found2;


	if(found == NULL){ // no folder --> a file!
		strcpy(ss,_absolute_filename);
	}
	else{
		strcpy(ss,found+1);
	}
}





