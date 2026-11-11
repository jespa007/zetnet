#include "zetnet.h"

bool ZN_Url_is_non_symbol(char c)
{
    if(c == '\0') return 1; //we want to write null regardless
    int c_int = (int)c;
    return (c_int >= 48 && c_int <= 57) || (c_int >= 65 && c_int <= 90) || (c_int >= 97 && c_int <= 122);
}

static bool ZN_CopyBounded(char *dst, size_t dst_size, const char *src)
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

static bool ZN_CopyBoundedN(char *dst, size_t dst_size, const char *src, size_t len)
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

static bool ZN_Url_IsUnreserved(unsigned char c)
{
    if (c >= 'A' && c <= 'Z') return true;
    if (c >= 'a' && c <= 'z') return true;
    if (c >= '0' && c <= '9') return true;

    return c == '-' || c == '_' || c == '.' || c == '~';
}

char *ZN_Url_Escape(const char *input)
{
    static const char hex[] = "0123456789ABCDEF";

    const unsigned char *src;
    char *output;
    char *dst;
    size_t output_len = 0;

    if (!input) {
        return NULL;
    }

    src = (const unsigned char *)input;

    while (*src) {
        if (ZN_Url_IsUnreserved(*src)) {
            output_len += 1;
        } else {
            output_len += 3;
        }

        src++;
    }

    output = ZN_MALLOC(output_len + 1);
    if (!output) {
        return NULL;
    }

    src = (const unsigned char *)input;
    dst = output;

    while (*src) {
        unsigned char c = *src;

        if (ZN_Url_IsUnreserved(c)) {
            *dst++ = (char)c;
        } else {
            *dst++ = '%';
            *dst++ = hex[(c >> 4) & 0x0F];
            *dst++ = hex[c & 0x0F];
        }

        src++;
    }

    *dst = '\0';

    return output;
}


static int ZN_Url_HexValue(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;

    return -1;
}

char *ZN_Url_Unescape(const char *input)
{
    const char *src;
    char *output;
    char *dst;

    if (!input) {
        return NULL;
    }

    output = ZN_MALLOC(strlen(input) + 1);
    if (!output) {
        return NULL;
    }

    src = input;
    dst = output;

    while (*src) {
        if (*src == '%') {
            int hi;
            int lo;

            if (!src[1] || !src[2]) {
                ZN_FREE(output);
                return NULL;
            }

            hi = ZN_Url_HexValue(src[1]);
            lo = ZN_Url_HexValue(src[2]);

            if (hi < 0 || lo < 0) {
                ZN_FREE(output);
                return NULL;
            }

            *dst++ = (char)((hi << 4) | lo);
            src += 3;
        } else {
            *dst++ = *src++;
        }
    }

    *dst = '\0';

    return output;
}

bool ZN_Url_Parse(const char *url_str, ZN_Url *out)
{
    bool result = false;

    const char *scheme_end = NULL;
    const char *rest = NULL;
    const char *path_start = NULL;
    const char *host_start = NULL;
    const char *host_end = NULL;
    const char *port_start = NULL;

    size_t scheme_len = 0;
    size_t host_len = 0;

    if (!url_str || !out) {
        return false;
    }

    memset(out, 0, sizeof(ZN_Url));

    /*
        Required format:
            scheme://host[:port][/path]
    */
    scheme_end = strstr(url_str, "://");
    if (!scheme_end) {
        return false;
    }

    scheme_len = (size_t)(scheme_end - url_str);

    if (scheme_len == 0) {
        return false;
    }

    if (!ZN_CopyBoundedN(out->scheme, sizeof(out->scheme), url_str, scheme_len)) {
        return false;
    }

    rest = scheme_end + 3;

    if (*rest == '\0') {
        return false;
    }

    path_start = strchr(rest, '/');

    if (path_start) {
        if (!ZN_CopyBounded(out->path, sizeof(out->path), path_start)) {
            return false;
        }

        host_end = path_start;
    } else {
        if (!ZN_CopyBounded(out->path, sizeof(out->path), "/")) {
            return false;
        }

        host_end = rest + strlen(rest);
    }

    host_start = rest;

    /*
        Basic IPv6 support:
            http://[::1]:8080/path
            https://[2001:db8::1]/
    */
    if (*host_start == '[') {
        const char *ipv6_end = strchr(host_start, ']');

        if (!ipv6_end || ipv6_end >= host_end) {
            return false;
        }

        host_len = (size_t)(ipv6_end - host_start + 1);

        if (!ZN_CopyBoundedN(out->host, sizeof(out->host), host_start, host_len)) {
            return false;
        }

        if ((ipv6_end + 1) < host_end) {
            if (ipv6_end[1] != ':') {
                return false;
            }

            port_start = ipv6_end + 2;
        }
    } else {
        const char *colon = NULL;

        for (colon = host_start; colon < host_end; colon++) {
            if (*colon == ':') {
                break;
            }
        }

        if (colon < host_end) {
            host_len = (size_t)(colon - host_start);
            port_start = colon + 1;
        } else {
            host_len = (size_t)(host_end - host_start);
        }

        if (host_len == 0) {
            return false;
        }

        if (!ZN_CopyBoundedN(out->host, sizeof(out->host), host_start, host_len)) {
            return false;
        }
    }

    if (port_start) {
        char port_buf[16];
        size_t port_len = (size_t)(host_end - port_start);

        if (port_len == 0 || port_len >= sizeof(port_buf)) {
            return false;
        }

        if (!ZN_CopyBoundedN(port_buf, sizeof(port_buf), port_start, port_len)) {
            return false;
        }

        if (!ZN_CStr_ToInt(&out->port, port_buf, 10)) {
            return false;
        }

        if (out->port <= 0 || out->port > 65535) {
            return false;
        }
    } else {
        if (strcmp(out->scheme, "https") == 0 || strcmp(out->scheme, "wss") == 0) {
            out->port = 443;
        } else if (strcmp(out->scheme, "http") == 0 || strcmp(out->scheme, "ws") == 0) {
            out->port = 80;
        } else {
            /*
                Unknown scheme.
                You can either fail here or default to 80.
                I prefer fail.
            */
            return false;
        }
    }

    result = true;

    return result;
}



