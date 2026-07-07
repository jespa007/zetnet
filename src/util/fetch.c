#include "zetnet.h"

static void fetch_print_usage(const char *app)
{
    fprintf(stderr,
            "Usage:\n"
            "  %s [options] <url>\n"
            "\n"
            "Options:\n"
            "  -H <header>     Add request header, example: -H \"Accept: application/json\"\n"
            "  -o <file>       Write response body to file\n"
            "  -h              Show help\n",
            app ? app : "fetch");
}

static bool fetch_add_header(ZN_HttpRequest *request, const char *header)
{
    if (!request || !header || header[0] == '\0') {
        fprintf(stderr, "Invalid empty header\n");
        return false;
    }

    if (strchr(header, '\r') || strchr(header, '\n')) {
        fprintf(stderr, "Invalid header: header cannot contain CR/LF\n");
        return false;
    }

    if (!strchr(header, ':')) {
        fprintf(stderr, "Invalid header '%s': expected format 'Name: value'\n", header);
        return false;
    }

    return ZN_HttpRequest_AddHeaderLine(request, header);
}

int main(int argc, char **argv)
{
    ZN_HttpRequest *request = NULL;
    const char *url = NULL;
    const char *output_filename = NULL;

    int i;

    if (argc < 2) {
        fetch_print_usage(argv[0]);
        return 1;
    }

    if (!ZN_Init()) {
        fprintf(stderr, "ZN_Init failed\n");
        return 1;
    }

    request = ZN_HttpRequest_NewEmpty();
    if (!request) {
        fprintf(stderr, "Cannot create HTTP request\n");
        ZN_DeInit();
        return 1;
    }

    strcpy(request->type, "GET");

    for (i = 1; i < argc; i++) {
        const char *arg = argv[i];

        if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
            fetch_print_usage(argv[0]);
            ZN_HttpRequest_Delete(request);
            ZN_DeInit();
            return 0;
        }

        if (strcmp(arg, "-H") == 0 || strcmp(arg, "--header") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Option %s requires a header value\n", arg);
                ZN_HttpRequest_Delete(request);
                ZN_DeInit();
                return 1;
            }

            i++;

            if (!fetch_add_header(request, argv[i])) {
                ZN_HttpRequest_Delete(request);
                ZN_DeInit();
                return 1;
            }

            continue;
        }

        if (strcmp(arg, "-o") == 0 || strcmp(arg, "--output") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Option %s requires a filename\n", arg);
                ZN_HttpRequest_Delete(request);
                ZN_DeInit();
                return 1;
            }

            i++;
            output_filename = argv[i];
            continue;
        }

        if (arg[0] == '-') {
            fprintf(stderr, "Unknown option: %s\n", arg);
            ZN_HttpRequest_Delete(request);
            ZN_DeInit();
            return 1;
        }

        if (url) {
            fprintf(stderr, "Only one URL is allowed\n");
            ZN_HttpRequest_Delete(request);
            ZN_DeInit();
            return 1;
        }

        url = arg;
    }

    if (!url) {
        fprintf(stderr, "Missing URL\n");
        fetch_print_usage(argv[0]);
        ZN_HttpRequest_Delete(request);
        ZN_DeInit();
        return 1;
    }

    if (strlen(url) >= sizeof(request->url)) {
        fprintf(stderr, "URL too long\n");
        ZN_HttpRequest_Delete(request);
        ZN_DeInit();
        return 1;
    }

    strcpy(request->url, url);

    /*
        Here call your existing fetch/get function.

        Example names only — adapt to your current API:
            ZN_HttpClient_Fetch(request)
            ZN_HttpRequest_Send(request)
            ZN_HttpClient_Get(...)
    */

    /*
    ZN_HttpResponse *response = ZN_HttpClient_Fetch(request);

    if (!response) {
        fprintf(stderr, "Request failed\n");
        ZN_HttpRequest_Delete(request);
        ZN_DeInit();
        return 1;
    }

    if (output_filename) {
        ZN_File_Write(output_filename, response->body, response->body_size);
    } else {
        fwrite(response->body, 1, response->body_size, stdout);
    }

    ZN_HttpResponse_Delete(response);
    */

    ZN_HttpRequest_Delete(request);
    ZN_DeInit();

    return 0;
}
