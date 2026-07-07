#include "zetnet.h"

#define ZG_ARRAY_CSTR char *

typedef struct{
	ZN_Array *array;
}ZN_ArrayCStrData;


void ZN_ArrayCstr_DestructItem(void *_data) {
    char *data = _data;

    if (data) {
        ZN_FREE(data);
    }
}

ZN_ArrayCStr *ZN_ArrayCstr_New(void)
{
    ZN_ArrayCStr *array = ZN_NEW(ZN_ArrayCStr);
    ZN_ArrayCStrData *data = ZN_NEW(ZN_ArrayCStrData);

    if (!array || !data) {
        ZN_FREE(data);
        ZN_FREE(array);
        return NULL;
    }

    data->array = ZN_ARRAY_NEW(ZG_ARRAY_CSTR);
    data->array->destructor_item = ZN_ArrayCstr_DestructItem;

    array->data = data;

    return array;
}

bool ZN_ArrayCstr_Push(ZN_ArrayCStr *_this, const char *_str)
{
    if (ZN_THIS_DATA_NULL(_this) || !_str) return false;

    ZN_ArrayCStrData *data = _this->data;
    char *copy = ZN_CStr_New(_str);

    if (!copy) {
    	return false;
    }

    return ZN_ARRAY_PUSH(ZG_ARRAY_CSTR, data->array, copy);
}

const char *ZN_ArrayCstr_Get(ZN_ArrayCStr *_this, size_t index)
{
    ZN_ArrayCStrData *data;
    char **slot;

    if (ZN_THIS_DATA_NULL(_this)) {
        return NULL;
    }

    data = _this->data;

    slot = ZN_ARRAY_GET(ZG_ARRAY_CSTR, data->array, index);
    if (!slot) {
        return NULL;
    }

    return *slot;
}


const char **ZN_ArrayCstr_GetData(ZN_ArrayCStr *_this)
{
    if (ZN_THIS_DATA_NULL(_this)) return NULL;

    ZN_ArrayCStrData *data = _this->data;
    return (const char **)ZN_ARRAY_GET_DATA(ZG_ARRAY_CSTR, data->array);
}

size_t ZN_ArrayCstr_Count(ZN_ArrayCStr *_this)
{
    if (ZN_THIS_DATA_NULL(_this)) return 0;

    ZN_ArrayCStrData *data = _this->data;
    return ZN_Array_Count(data->array);
}

void				ZN_ArrayCstr_Delete(ZN_ArrayCStr	* _this){
	if(ZN_THIS_DATA_NULL(_this)) return;

	ZN_ArrayCStrData* data = _this->data;
	ZN_Array_Delete(data->array);
	ZN_FREE(data);
	ZN_FREE(_this);
}

