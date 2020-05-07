#include "zetnet.h"

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



