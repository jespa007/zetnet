#include "zetnet.h"

#define BUFFER_SIZE  1024
#define REQUEST_SIZE 4096

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

#define FETCH_ARG_ERROR() \
    do {                  \
        ok = false;       \
        break;            \
    } while (0)

int main(int argc, char **argv)
{
    ZN_HttpRequest *request = NULL;
    ZN_Url parsed_url;
    ZN_Connection conn;

    const char *url = NULL;
    const char *output_filename = NULL;

    bool request_help = false;
    bool ok = true;
    int return_out = 0;

    memset(&parsed_url, 0, sizeof(parsed_url));
    memset(&conn, 0, sizeof(conn));

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

    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];

        if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
            request_help = true;
            break;
        }

        if (strcmp(arg, "-H") == 0 || strcmp(arg, "--header") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Option %s requires a header value\n", arg);
                FETCH_ARG_ERROR();
            }

            i++;

            if (!fetch_add_header(request, argv[i])) {
                FETCH_ARG_ERROR();
            }

            continue;
        }

        if (strcmp(arg, "-o") == 0 || strcmp(arg, "--output") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Option %s requires a filename\n", arg);
                FETCH_ARG_ERROR();
            }

            i++;
            output_filename = argv[i];
            continue;
        }

        if (arg[0] == '-') {
            fprintf(stderr, "Unknown option: %s\n", arg);
            FETCH_ARG_ERROR();
        }

        if (url) {
            fprintf(stderr, "Only one URL is allowed\n");
            FETCH_ARG_ERROR();
        }

        url = arg;
    }

    if (request_help) {
        fetch_print_usage(argv[0]);
        goto fetch_exit;
    }

    if (!ok) {
        return_out = 1;
        goto fetch_exit;
    }

    if (!url) {
        fprintf(stderr, "Missing URL\n");
        fetch_print_usage(argv[0]);
        return_out = 1;
        goto fetch_exit;
    }

    if (strlen(url) >= sizeof(request->url)) {
        fprintf(stderr, "URL too long\n");
        return_out = 1;
        goto fetch_exit;
    }

    if (!ZN_Url_Parse(url, &parsed_url)) {
        fprintf(stderr, "Invalid URL: %s\n", url);
        return_out = 1;
        goto fetch_exit;
    }

    strcpy(request->url, url);

    /*
        Depending on your ZN_HttpRequest_Build() implementation,
        you may also need to fill these fields manually.
    */
    if (strlen(parsed_url.host) < sizeof(request->host)) {
        strcpy(request->host, parsed_url.host);
    } else {
        fprintf(stderr, "Host too long\n");
        return_out = 1;
        goto fetch_exit;
    }

    if (!ZN_Connection_Open(&conn, &parsed_url)) {
        fprintf(stderr, "Connection failed\n");
        return_out = 1;
        goto fetch_exit;
    }

    {
        char http_request[REQUEST_SIZE];

        if (!ZN_HttpRequest_Build(http_request, sizeof(http_request), request)) {
            fprintf(stderr, "Failed to build HTTP request\n");
            return_out = 1;
            goto fetch_exit;
        }

        if (ZN_Connection_Write(&conn, (uint8_t *)http_request, strlen(http_request)) <= 0) {
            fprintf(stderr, "Failed to send HTTP request\n");
            return_out = 1;
            goto fetch_exit;
        }
    }

    {
        char buffer[BUFFER_SIZE];
        FILE *output = stdout;

        if (output_filename) {
            output = fopen(output_filename, "wb");
            if (!output) {
                fprintf(stderr, "Cannot open output file: %s\n", output_filename);
                return_out = 1;
                goto fetch_exit;
            }
        }

        while (1) {
            ssize_t n = ZN_Connection_Read(&conn, (uint8_t *)buffer, sizeof(buffer));

            if (n < 0) {
                fprintf(stderr, "Read error\n");
                return_out = 1;
                break;
            }

            if (n == 0) {
                break;
            }

            fwrite(buffer, 1, (size_t)n, output);
        }

        if (output_filename && output) {
            fclose(output);
        }
    }

fetch_exit:

    ZN_Connection_Close(&conn);

    if (request) {
        ZN_HttpRequest_Delete(request);
    }

    ZN_DeInit();

    return return_out;
}
