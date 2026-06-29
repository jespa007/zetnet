#include "zetnet.h"

bool ZN_Url_is_non_symbol(char c)
{
    if(c == '\0') return 1; //we want to write null regardless
    int c_int = (int)c;
    return (c_int >= 48 && c_int <= 57) || (c_int >= 65 && c_int <= 90) || (c_int >= 97 && c_int <= 122);
}

char * ZN_Url_escape(const char * input)
{
	int end = strlen(input);
	size_t final_size = (end * 3) + 1;
	char *working = malloc(final_size * sizeof(char)), *output = working;

	while(*input)
	{
		const char c = *input;
		if(c < 0)
		{
			input++;
		}
		else if(ZN_Url_is_non_symbol(c))
		{
			*working++ = *input++;
		}
		else
		{
			char encoded[4] = {0};
			snprintf(encoded, 4, "%%%02x", c);

			*working++ = encoded[0];
			*working++ = encoded[1];
			*working++ = encoded[2];
			input++;
		}
	}

	*working = 0; //null term
	return output;
}


char * ZN_Url_Unescape(const char * input)
{
	int input_length = strlen(input);

		size_t output_length = (input_length + 1) * sizeof(char);
		char *working = malloc(output_length), *output = working;

		while(*input)
		{
			if(*input == '%')
			{
				char buffer[3] = { input[1], input[2], 0 };
				*working++ = strtol(buffer, NULL, 16);
				input += 3;
			}
			else
			{
				*working++ = *input++;
			}
		}

		*working = 0; //null terminate
		return output;
}

bool ZN_Url_Parse(const char *url_str, ZN_Url *out) {
    if (!url_str || !out) return false;

    memset(out, 0, sizeof(ZN_Url));

    // 1. Split scheme://rest
    ZN_List *scheme_split = ZN_String_Split(url_str, ':');
    if (!scheme_split || scheme_split->count < 2) return false;

    strcpy(out->scheme, scheme_split->items[0]);

    // skip "//"
    char *rest = strstr(url_str, "://");
    if (!rest) return false;
    rest += 3;

    // 2. Split host[:port]/path
    char *path_start = strchr(rest, '/');

    if (path_start) {
        strcpy(out->path, path_start);
    } else {
        strcpy(out->path, "/");
    }

    char hostport[256] = {0};

    if (path_start) {
        size_t len = path_start - rest;
        strncpy(hostport, rest, len);
        hostport[len] = '\0';
    } else {
        strcpy(hostport, rest);
    }

    // 3. Split host:port
    char *colon = strchr(hostport, ':');

    if (colon) {
        *colon = '\0';
        strcpy(out->host, hostport);

        if (!ZN_String_ToInt(&out->port, colon + 1, 10)) {
            return false;
        }
    } else {
        strcpy(out->host, hostport);

        // default ports
        if (strcmp(out->scheme, "https") == 0) {
            out->port = 443;
        } else {
            out->port = 80;
        }
    }

    ZN_List_DeleteAndFreeAllItems(scheme_split);

    return true;
}




