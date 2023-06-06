#ifndef __ZN_HTTP_PARAM_VALUE_H__
#define __ZN_HTTP_PARAM_VALUE_H__

typedef struct ZN_HttpParamValue ZN_HttpParamValue;

struct ZN_HttpParamValue
{
	char name[256];
	char value[256];
};

ZN_HttpParamValue * ZN_HttpParamValue_New(char *name, char *value);

#endif
