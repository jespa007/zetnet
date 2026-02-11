#include "zetnet.h"


#define HTTP_SERVER_DEFAULT_PORT 8081

int main(int argc, char *argv[])
{
	char path[MAX_PATH]={"."};
	int port=HTTP_SERVER_DEFAULT_PORT;
	bool send_same_site_attribute=false;
	bool error=false;

	for(int i=1; i < argc && !error; i++){
		ZN_List *l=ZN_String_Split(argv[i],'=');
		switch(l->count){
		case 1:
			if(strcmp(l->items[0],"--send_same_site_attribute")==0){
				send_same_site_attribute=true;
			}else{
				fprintf(stderr,"unknow option %s\n",(const char *)l->items[0]);
				error=true;
			}
			break;
		case 2:
			if(strcmp(l->items[0],"--port")==0){
				error=!ZN_String_ToInt(&port,l->items[1],10);
			}else if(strcmp(l->items[0],"--path")==0){
				strcpy(path,(const char *)l->items[1]);
			}else{
				fprintf(stderr,"unknow option %s\n",(const char *)l->items[0]);
				error=true;
			}
			break;
		}

		ZN_List_DeleteAndFreeAllItems(l);
	}

	if(error){
		return -1;
	}

	ZN_Init();



	ZN_DeInit();

	return 0;
}
