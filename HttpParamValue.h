#ifndef __HTTP_PARAM_VALUE_H__
#define __HTTP_PARAM_VALUE_H__

typedef struct HttpParamValue HttpParamValue;

struct HttpParamValue
{
	char name[256];
	char value[256];
};

#endif
