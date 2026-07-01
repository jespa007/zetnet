#include "zetnet.h"

#define BUFFER_SIZE 1024
#define REQUEST_SIZE 2048


int main(int argc, char *argv[]) {

	bool error = false;
	ZN_HttpRequest req;
	ZN_Url url;
	int return_out = 0;
	//ZN_ArrayCStr *headers = ZN_ArrayCStr_New();

	for(int i=1; i < argc && !error; i++){
		ZN_List *l=ZN_CStr_Split(argv[i],'=');
		switch(l->count){
		default:
			if(strncmp(l->items[0],"http",4) == 0){
				if (!ZN_Url_Parse(l->items[0], &url)) {
				    fprintf(stderr, "Invalid URL\n");
				    error=true;
				}
			}else{
				fprintf(stderr,"unknow option %s\n",(const char *)l->items[0]);
				error=true;
			}
			break;
		case 2:
			/*if(strcmp(l->items[0],"-H")==0){
				ZN_ArrayCStr_Push(headers, l->items[1]);
			}else{
				fprintf(stderr,"unknow option %s\n",(const char *)l->items[0]);
				error=true;
			}*/
			break;
		}

		ZN_List_DeleteAndFreeAllItems(l);
	}

    if (argc < 2) {
        fprintf(stderr, "Usage: fetch <url>\n");
        goto fetch_exit;
        return_out=1;
    }


    ZN_Init();

    ZN_Connection conn;
    if (ZN_Connection_Open(&conn, &url)) {

		char request[REQUEST_SIZE];
		ZN_HttpRequest_Build(request, sizeof(request), &req);

		ZN_Connection_Write(&conn, (uint8_t*)request, strlen(request));

		char buffer[BUFFER_SIZE];

		while (1) {
			int n = ZN_Connection_Read(&conn, (uint8_t*)buffer, sizeof(buffer)-1);
			if (n <= 0) break;

			buffer[n] = '\0';
			printf("%s", buffer);
		}

    }else{
        fprintf(stderr, "Connection failed\n");
    }

	ZN_Connection_Close(&conn);


    ZN_DeInit();

fetch_exit:

    //ZN_ArrayCStr_Delete(headers);

    return return_out;
}
