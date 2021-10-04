#include "zetnet.h"

bool zn_url_is_non_symbol(char c)
{
    if(c == '\0') return 1; //we want to write null regardless
    int c_int = (int)c;
    return (c_int >= 48 && c_int <= 57) || (c_int >= 65 && c_int <= 90) || (c_int >= 97 && c_int <= 122);
}

char * zn_url_escape(const char * input)
{
	int end = strlen(input);
	size_t final_size = (end * 3) + 1;
	char *working = ZN_MALLOC(final_size * sizeof(char)), *output = working;

	while(*input)
	{
		const char c = *input;
		if(c < 0)
		{
			input++;
		}
		else if(zn_url_is_non_symbol(c))
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


char * zn_url_unescape(const char * input)
{
	int input_length = strlen(input);

		size_t output_length = (input_length + 1) * sizeof(char);
		char *working = ZN_MALLOC(output_length), *output = working;

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




