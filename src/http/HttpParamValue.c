#include "zetnet.h"

HttpParamValue * HttpParamValue_New(char *name, char *value){
	HttpParamValue * http_param=ZN_MALLOC(sizeof(HttpParamValue));
	strcpy(http_param->name,name);
	strcpy(http_param->value,value);
	return http_param;
}
