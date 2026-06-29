#include "zetnet.h"

#define BUFFER_SIZE 1024
#define REQUEST_SIZE 2048


int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Usage: zn_fetch <url>\n");
        return 1;
    }

    ZN_Url url;
    if (!ZN_Url_Parse(argv[1], &url)) {
        fprintf(stderr, "Invalid URL\n");
        return 1;
    }

    ZN_Init();

    ZN_Connection conn;
    if (ZN_Connection_Open(&conn, &url)) {

		char request[REQUEST_SIZE];
		ZN_HttpRequest_BuildGet(request, sizeof(request), &url);

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

    return 0;
}
