#include "zetnet.h"

void ZN_HttpKeyValue_DestructItem(void *_data) {
    ZN_HttpKeyValue *data = _data;

    if (data->key) {
        ZN_FREE(data->key);
    }

    if (data->value) {
        ZN_FREE(data->value);
    }
}

/*
ZN_HttpKeyValue * ZN_HttpParamValue_New(char *name, char *value){
	ZN_HttpKeyValue * http_param=ZN_NEW(ZN_HttpKeyValue);
	strcpy(http_param->name,name);
	strcpy(http_param->value,value);
	return http_param;
}*/

/*ZN_HttpKeyValue ZN_HttpKeyValue_New(const char *key, const char *value) {
    ZN_HttpKeyValue kv;

    kv.key = ZN_CStr_Dup(key ? key : "");
    kv.value = ZN_CStr_Dup(value ? value : "");

    return kv;
}*/
ZN_Array *ZN_HttpKeyValueArray_New(void) {
    ZN_Array *array = ZN_ARRAY_HTTP_KEY_VALUE_NEW();

    if (!array) {
        return NULL;
    }

    array->destructor_item = ZN_HttpKeyValue_DestructItem;

    return array;
}

bool ZN_HttpKeyValueArray_Push(ZN_Array *array, const char *key, const char *value) {
    ZN_HttpKeyValue kv;

    kv.key = ZN_CStr_New(key ? key : "");
    kv.value = ZN_CStr_New(value ? value : "");

    if (!kv.key || !kv.value) {
        if (kv.key) ZN_FREE(kv.key);
        if (kv.value) ZN_FREE(kv.value);
        return false;
    }

    ZN_ARRAY_HTTP_KEY_VALUE_PUSH(array, kv);

    return true;
}
