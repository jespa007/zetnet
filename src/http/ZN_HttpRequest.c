#include "zetnet.h"

#ifdef _WIN32
    #define ZN_STRCASECMP _stricmp
#else
    #define ZN_STRCASECMP strcasecmp
#endif

static size_t ZN_Appendf(
    char *out,
    size_t size,
    size_t used,
    const char *fmt,
    ...
) {
    va_list args;
    int written;

    if (!out || size == 0 || used >= size) {
        return size;
    }

    va_start(args, fmt);
    written = vsnprintf(out + used, size - used, fmt, args);
    va_end(args);

    if (written < 0) {
        return size;
    }

    if ((size_t)written >= size - used) {
        out[size - 1] = '\0';
        return size;
    }

    return used + (size_t)written;
}

static bool ZN_CopyStr(char *dst, size_t dst_size, const char *src)
{
    size_t len;

    if (!dst || dst_size == 0 || !src) {
        return false;
    }

    len = strlen(src);

    if (len + 1 > dst_size) {
        dst[0] = '\0';
        return false;
    }

    memcpy(dst, src, len + 1);
    return true;
}

static bool ZN_CopyStrN(char *dst, size_t dst_size, const char *src, size_t len)
{
    if (!dst || dst_size == 0 || !src) {
        return false;
    }

    if (len + 1 > dst_size) {
        dst[0] = '\0';
        return false;
    }

    memcpy(dst, src, len);
    dst[len] = '\0';

    return true;
}

static char *ZN_TrimLeft(char *s)
{
    if (!s) {
        return NULL;
    }

    while (*s == ' ' || *s == '\t') {
        s++;
    }

    return s;
}

static void ZN_TrimRightInPlace(char *s)
{
    size_t len;

    if (!s) {
        return;
    }

    len = strlen(s);

    while (len > 0 && (s[len - 1] == ' ' || s[len - 1] == '\t')) {
        s[len - 1] = '\0';
        len--;
    }
}

static void ZN_TrimInPlace(char *s)
{
    char *left;

    if (!s) {
        return;
    }

    left = ZN_TrimLeft(s);

    if (left != s) {
        memmove(s, left, strlen(left) + 1);
    }

    ZN_TrimRightInPlace(s);
}

static const char *ZN_HttpRequest_MimeFromPath(const char *path, bool *is_binary)
{
    const char *ext;

    if (is_binary) {
        *is_binary = false;
    }

    if (!path) {
        return "text/html";
    }

    ext = strrchr(path, '.');
    if (!ext) {
        return "text/html";
    }

    if (strcmp(ext, ".png") == 0) {
        if (is_binary) *is_binary = true;
        return "image/png";
    }

    if (strcmp(ext, ".css") == 0) {
        return "text/css";
    }

    if (strcmp(ext, ".js") == 0 ||
        strcmp(ext, ".cjs") == 0 ||
        strcmp(ext, ".mjs") == 0) {
        return "text/javascript";
    }

    if (strcmp(ext, ".json") == 0) {
        return "application/json";
    }

    if (strcmp(ext, ".gltf") == 0) {
        return "model/gltf+json";
    }

    if (strcmp(ext, ".pdf") == 0) {
        if (is_binary) *is_binary = true;
        return "application/pdf";
    }

    if (strcmp(ext, ".wasm") == 0) {
        if (is_binary) *is_binary = true;
        return "application/wasm";
    }

    if (strcmp(ext, ".svg") == 0) {
        return "image/svg+xml";
    }

    if (strcmp(ext, ".eot") == 0 ||
        strcmp(ext, ".ttf") == 0 ||
        strcmp(ext, ".woff") == 0 ||
        strcmp(ext, ".woff2") == 0) {
        if (is_binary) *is_binary = true;
        return "application/octet-stream";
    }

    return "text/html";
}

static bool ZN_HttpRequest_HasHeader(
    const ZN_HttpRequest *req,
    const char *name
) {
    size_t count;
    ZN_HttpKeyValue *headers;

    if (!req || !req->headers || !name) {
        return false;
    }

    count = ZN_Array_Count(req->headers);
    headers = ZN_ARRAY_HTTP_KEY_VALUE_GET_DATA(req->headers);

    if (!headers) {
        return false;
    }

    for (size_t i = 0; i < count; i++) {
        if (headers[i].key && ZN_STRCASECMP(headers[i].key, name) == 0) {
            return true;
        }
    }

    return false;
}

static bool ZN_HttpRequest_AddParamLine(
    ZN_HttpRequest *request,
    const char *param_line
) {
    const char *eq;
    size_t key_len;

    if (!request || !param_line || param_line[0] == '\0') {
        return false;
    }

    eq = strchr(param_line, '=');
    if (!eq) {
        return false;
    }

    key_len = (size_t)(eq - param_line);
    if (key_len == 0) {
        return false;
    }

    if (!request->params) {
        request->params = ZN_HttpKeyValueArray_New();

        if (!request->params) {
            return false;
        }
    }

    return ZN_HttpKeyValue_PushN(
        request->params,
        param_line,
        key_len,
        eq + 1
    );
}

static void ZN_HttpRequest_ParseParams(
    ZN_HttpRequest *request,
    const char *params_text
) {
    const char *start;
    const char *amp;

    if (!request || !params_text || params_text[0] == '\0') {
        return;
    }

    start = params_text;

    while (*start) {
        char param_buf[4096];
        size_t len;

        amp = strchr(start, '&');

        if (amp) {
            len = (size_t)(amp - start);
        } else {
            len = strlen(start);
        }

        if (len > 0 && len < sizeof(param_buf)) {
            if (ZN_CopyStrN(param_buf, sizeof(param_buf), start, len)) {
                char *decoded = ZN_Url_Unescape(param_buf);

                if (decoded) {
                    ZN_HttpRequest_AddParamLine(request, decoded);
                    ZN_FREE(decoded);
                } else {
                    ZN_HttpRequest_AddParamLine(request, param_buf);
                }
            }
        }

        if (!amp) {
            break;
        }

        start = amp + 1;
    }
}

bool ZN_HttpRequest_AddHeaderN(
    ZN_HttpRequest *_request,
    const char *_header_line,
    size_t _key_len,
    const char *_value_start
) {
    if (!_request || !_header_line || !_value_start || _key_len == 0) {
        return false;
    }

    if (!_request->headers) {
        _request->headers = ZN_HttpKeyValueArray_New();

        if (!_request->headers) {
            return false;
        }
    }

    return ZN_HttpKeyValue_PushN(
        _request->headers,
        _header_line,
        _key_len,
        _value_start
    );
}

bool ZN_HttpRequest_AddHeader(
    ZN_HttpRequest *_request,
    const char *_key,
    const char *_value
) {
    if (!_request || !_key || !_value || _key[0] == '\0') {
        return false;
    }

    return ZN_HttpRequest_AddHeaderN(
        _request,
        _key,
        strlen(_key),
        _value
    );
}

bool ZN_HttpRequest_AddHeaderLine(
    ZN_HttpRequest *request,
    const char *header_line
) {
    const char *colon;
    const char *value_start;
    size_t key_len;

    if (!request || !header_line || header_line[0] == '\0') {
        return false;
    }

    if (strchr(header_line, '\r') || strchr(header_line, '\n')) {
        return false;
    }

    colon = strchr(header_line, ':');
    if (!colon) {
        return false;
    }

    key_len = (size_t)(colon - header_line);
    if (key_len == 0) {
        return false;
    }

    value_start = colon + 1;

    while (*value_start == ' ' || *value_start == '\t') {
        value_start++;
    }

    return ZN_HttpRequest_AddHeaderN(
        request,
        header_line,
        key_len,
        value_start
    );
}

void ZN_HttpRequest_InitGetFromUrl(
    ZN_HttpRequest *req,
    const ZN_Url *url
) {
    if (!req || !url) {
        return;
    }

    memset(req, 0, sizeof(*req));

    snprintf(req->type, sizeof(req->type), "GET");
    snprintf(req->host, sizeof(req->host), "%s", url->host);

    if (url->path[0]) {
        snprintf(req->url, sizeof(req->url), "%s", url->path);
    } else {
        snprintf(req->url, sizeof(req->url), "/");
    }

    req->mime = "*/*";
    req->is_binary = false;
    req->params = NULL;
    req->headers = ZN_HttpKeyValueArray_New();
}

ZN_HttpRequest *ZN_HttpRequest_NewEmpty(void)
{
    ZN_HttpRequest *request = ZN_NEW(ZN_HttpRequest);

    if (!request) {
        return NULL;
    }

    request->params = NULL;
    request->headers = ZN_HttpKeyValueArray_New();

    if (!request->headers) {
        ZN_FREE(request);
        return NULL;
    }

    return request;
}

ZN_HttpRequest *ZN_HttpRequest_New(
    char *_type,
    char *_url,
    char *_host,
    char *_referer,
    const char *_mime,
    bool _is_binary,
    char *_content_type,
    ZN_Array *_params
) {
    ZN_HttpRequest *http_request = ZN_HttpRequest_NewEmpty();

    if (!http_request) {
        return NULL;
    }

    if (_type && !ZN_CopyStr(http_request->type, sizeof(http_request->type), _type)) {
        ZN_HttpRequest_Delete(http_request);
        return NULL;
    }

    if (_url && !ZN_CopyStr(http_request->url, sizeof(http_request->url), _url)) {
        ZN_HttpRequest_Delete(http_request);
        return NULL;
    }

    if (_host && !ZN_CopyStr(http_request->host, sizeof(http_request->host), _host)) {
        ZN_HttpRequest_Delete(http_request);
        return NULL;
    }

    if (_referer && !ZN_CopyStr(http_request->referer, sizeof(http_request->referer), _referer)) {
        ZN_HttpRequest_Delete(http_request);
        return NULL;
    }

    if (_content_type && !ZN_CopyStr(http_request->content_type, sizeof(http_request->content_type), _content_type)) {
        ZN_HttpRequest_Delete(http_request);
        return NULL;
    }

    http_request->mime = _mime;
    http_request->is_binary = _is_binary;
    http_request->params = _params;

    return http_request;
}

bool ZN_HttpRequest_Build(
    char *out,
    size_t size,
    const ZN_HttpRequest *req
) {
    const char *method;
    const char *path;
    size_t used = 0;

    if (!out || size == 0 || !req) {
        return false;
    }

    out[0] = '\0';

    method = req->type[0] ? req->type : "GET";
    path = req->url[0] ? req->url : "/";

#define ZN_HTTP_APPEND(...)                                      \
    do {                                                        \
        used = ZN_Appendf(out, size, used, __VA_ARGS__);         \
        if (used >= size) {                                     \
            return false;                                       \
        }                                                       \
    } while (0)

    ZN_HTTP_APPEND("%s %s HTTP/1.1\r\n", method, path);

    if (req->host[0] && !ZN_HttpRequest_HasHeader(req, "Host")) {
        ZN_HTTP_APPEND("Host: %s\r\n", req->host);
    }

    if (!ZN_HttpRequest_HasHeader(req, "User-Agent")) {
        ZN_HTTP_APPEND("User-Agent: zetnet/1.0\r\n");
    }

    if (!ZN_HttpRequest_HasHeader(req, "Accept")) {
        if (req->mime) {
            ZN_HTTP_APPEND("Accept: %s\r\n", req->mime);
        } else {
            ZN_HTTP_APPEND("Accept: */*\r\n");
        }
    }

    if (req->referer[0] && !ZN_HttpRequest_HasHeader(req, "Referer")) {
        ZN_HTTP_APPEND("Referer: %s\r\n", req->referer);
    }

    if (req->content_type[0] && !ZN_HttpRequest_HasHeader(req, "Content-Type")) {
        ZN_HTTP_APPEND("Content-Type: %s\r\n", req->content_type);
    }

    if (req->headers) {
        size_t count = ZN_Array_Count(req->headers);
        ZN_HttpKeyValue *headers = ZN_ARRAY_HTTP_KEY_VALUE_GET_DATA(req->headers);

        if (headers) {
            for (size_t i = 0; i < count; i++) {
                ZN_HttpKeyValue *header = &headers[i];

                if (header->key && header->key[0] && header->value) {
                    ZN_HTTP_APPEND("%s: %s\r\n", header->key, header->value);
                }
            }
        }
    }

    if (!ZN_HttpRequest_HasHeader(req, "Connection")) {
        ZN_HTTP_APPEND("Connection: close\r\n");
    }

    ZN_HTTP_APPEND("\r\n");

#undef ZN_HTTP_APPEND

    return true;
}

ZN_HttpRequest *ZN_HttpRequest_GetRequest(const char *str_request)
{
    ZN_HttpRequest *http_request = NULL;
    char *request = NULL;
    char *headers_end = NULL;
    char *body = NULL;
    char *line = NULL;
    char *next_line = NULL;

    if (!str_request || str_request[0] == '\0') {
        return NULL;
    }

    request = ZN_CStr_New(str_request);
    if (!request) {
        return NULL;
    }

    ZN_CStr_DeleteChar(request, '\r');

    http_request = ZN_HttpRequest_NewEmpty();
    if (!http_request) {
        ZN_FREE(request);
        return NULL;
    }

    headers_end = strstr(request, "\n\n");
    if (headers_end) {
        *headers_end = '\0';
        body = headers_end + 2;
    }

    line = request;
    next_line = strchr(line, '\n');

    if (next_line) {
        *next_line = '\0';
        next_line++;
    }

    /*
        Request line:
            GET /path?x=1 HTTP/1.1
    */
    {
        char *method = line;
        char *path = NULL;
        char *version = NULL;
        char *query = NULL;
        char *decoded_path = NULL;

        path = strchr(method, ' ');
        if (!path) {
            ZN_HttpRequest_Delete(http_request);
            ZN_FREE(request);
            return NULL;
        }

        *path = '\0';
        path++;

        while (*path == ' ') {
            path++;
        }

        version = strchr(path, ' ');
        if (version) {
            *version = '\0';
        }

        if (method[0] == '\0' || path[0] == '\0') {
            ZN_HttpRequest_Delete(http_request);
            ZN_FREE(request);
            return NULL;
        }

        if (!ZN_CopyStr(http_request->type, sizeof(http_request->type), method)) {
            ZN_HttpRequest_Delete(http_request);
            ZN_FREE(request);
            return NULL;
        }

        query = strchr(path, '?');
        if (query) {
            *query = '\0';
            query++;
        }

        decoded_path = ZN_Url_Unescape(path);
        if (decoded_path) {
            if (!ZN_CopyStr(http_request->url, sizeof(http_request->url), decoded_path)) {
                ZN_FREE(decoded_path);
                ZN_HttpRequest_Delete(http_request);
                ZN_FREE(request);
                return NULL;
            }

            ZN_FREE(decoded_path);
        } else {
            if (!ZN_CopyStr(http_request->url, sizeof(http_request->url), path)) {
                ZN_HttpRequest_Delete(http_request);
                ZN_FREE(request);
                return NULL;
            }
        }

        if (http_request->url[0] == '\0') {
            ZN_CopyStr(http_request->url, sizeof(http_request->url), "/");
        }

        http_request->mime = ZN_HttpRequest_MimeFromPath(
            http_request->url,
            &http_request->is_binary
        );

        if (query) {
            ZN_HttpRequest_ParseParams(http_request, query);
        }
    }

    /*
        Header lines.
    */
    line = next_line;

    while (line && *line) {
        char *colon;
        char *value;
        char *line_end;

        line_end = strchr(line, '\n');
        if (line_end) {
            *line_end = '\0';
        }

        if (line[0] == '\0') {
            break;
        }

        colon = strchr(line, ':');
        if (colon) {
            *colon = '\0';

            value = colon + 1;
            value = ZN_TrimLeft(value);
            ZN_TrimRightInPlace(line);
            ZN_TrimRightInPlace(value);

            if (line[0] != '\0') {
                ZN_HttpRequest_AddHeader(http_request, line, value);

                if (ZN_STRCASECMP(line, "Host") == 0) {
                    ZN_CopyStr(http_request->host, sizeof(http_request->host), value);
                } else if (ZN_STRCASECMP(line, "Referer") == 0) {
                    ZN_CopyStr(http_request->referer, sizeof(http_request->referer), value);
                } else if (ZN_STRCASECMP(line, "Content-Type") == 0) {
                    char content_type[ZN_MAX_CONTENT_LEN];
                    char *semi;

                    if (ZN_CopyStr(content_type, sizeof(content_type), value)) {
                        semi = strchr(content_type, ';');
                        if (semi) {
                            *semi = '\0';
                        }

                        ZN_TrimInPlace(content_type);
                        ZN_CopyStr(http_request->content_type,
                                   sizeof(http_request->content_type),
                                   content_type);
                    }
                }
            }
        }

        if (!line_end) {
            break;
        }

        line = line_end + 1;
    }

    /*
        Body params, mostly useful for application/x-www-form-urlencoded.
        This preserves your old behavior of parsing key=value&key2=value2 body data.
    */
    if (body && body[0]) {
        ZN_HttpRequest_ParseParams(http_request, body);
    }

    ZN_FREE(request);

    return http_request;
}

void ZN_HttpRequest_Delete(ZN_HttpRequest *request)
{
    if (!request) {
        return;
    }

    if (request->params) {
        ZN_Array_Delete(request->params);
        request->params = NULL;
    }

    if (request->headers) {
        ZN_Array_Delete(request->headers);
        request->headers = NULL;
    }

    ZN_FREE(request);
}
