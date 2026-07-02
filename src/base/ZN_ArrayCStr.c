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

ZN_ArrayCStr	*	ZN_ArrayCstr_New(void){
	ZN_ArrayCStr	* array = ZN_NEW(ZN_ArrayCStr);
	ZN_ArrayCStrData* data = ZN_NEW(ZN_ArrayCStrData);

	data->array = ZN_ARRAY_NEW(ZG_ARRAY_CSTR);
	data->array->destructor_item = ZN_ArrayCstr_DestructItem;

	return array;
}

void				ZN_ArrayCstr_Push(ZN_ArrayCStr	* _this, const char *_str){
	if(ZN_THIS_DATA_NULL(_this)) return;

	ZN_ArrayCStrData* data = _this->data;
	ZN_ARRAY_PUSH(ZG_ARRAY_CSTR, data->array, _str);
}

void				ZN_ArrayCstr_Delete(ZN_ArrayCStr	* _this){
	if(ZN_THIS_DATA_NULL(_this)) return;

	ZN_ArrayCStrData* data = _this->data;
	ZN_Array_Delete(data->array);
	ZN_FREE(data);
	ZN_FREE(_this);
}

