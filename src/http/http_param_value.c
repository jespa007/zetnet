#include "zetnet.h"

ZN_HttpParamValue * ZN_HttpParamValue_New(char *name, char *value){
	ZN_HttpParamValue * http_param=ZN_NEW(ZN_HttpParamValue);
	strcpy(http_param->name,name);
	strcpy(http_param->value,value);
	return http_param;
}
