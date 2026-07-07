#include "zetnet.h"

void ZN_HttpKeyValue_DestructItem(void *_data)
{
    ZN_HttpKeyValue *data = (ZN_HttpKeyValue *)_data;

    if (!data) {
        return;
    }

    if (data->key) {
        ZN_FREE(data->key);
        data->key = NULL;
    }

    if (data->value) {
        ZN_FREE(data->value);
        data->value = NULL;
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

bool ZN_HttpKeyValueArray_Push(ZN_Array *_this, const char *key, const char *value) {
    ZN_HttpKeyValue kv;

    kv.key = ZN_CStr_New(key ? key : "");
    kv.value = ZN_CStr_New(value ? value : "");

    if (!kv.key || !kv.value) {
        if (kv.key) ZN_FREE(kv.key);
        if (kv.value) ZN_FREE(kv.value);
        return false;
    }

    ZN_ARRAY_HTTP_KEY_VALUE_PUSH(_this, kv);

    return true;
}

bool ZN_HttpKeyValue_PushN(
    ZN_Array *_array,
    const char *_key,
    size_t _key_len,
    const char *_value
)
{
    ZN_HttpKeyValue item;

    if (!_array || !_key || !_value || _key_len == 0) {
        return false;
    }

    memset(&item, 0, sizeof(item));

    item.key = ZN_CStr_NewLen(_key, _key_len);
    if (!item.key) {
        return false;
    }

    item.value = ZN_CStr_New(_value);
    if (!item.value) {
        ZN_FREE(item.key);
        return false;
    }

    if (!ZN_ARRAY_PUSH(ZN_ARRAY_HTTP_KEY_VALUE, _array, item)) {
        ZN_HttpKeyValue_DestructItem(&item);
        return false;
    }

    return true;
}
