#ifndef __ZN_HTTP_KEY_VALUE_H__
#define __ZN_HTTP_KEY_VALUE_H__

#define ZN_ARRAY_HTTP_KEY_VALUE ZN_HttpKeyValue

#define ZN_ARRAY_HTTP_KEY_VALUE_NEW() \
    ZN_ARRAY_NEW(ZN_ARRAY_HTTP_KEY_VALUE)

#define ZN_ARRAY_HTTP_KEY_VALUE_PUSH(_array, _data) \
    ZN_ARRAY_PUSH(ZN_ARRAY_HTTP_KEY_VALUE, _array, _data)

#define ZN_ARRAY_HTTP_KEY_VALUE_GET(_array, _pos) \
    ZN_ARRAY_GET(ZN_ARRAY_HTTP_KEY_VALUE, _array, _pos)

#define ZN_ARRAY_HTTP_KEY_VALUE_GET_DATA(_array) \
    ZN_ARRAY_GET_DATA(ZN_ARRAY_HTTP_KEY_VALUE, _array)


typedef struct ZN_HttpKeyValue ZN_HttpKeyValue;

struct ZN_HttpKeyValue {
    char *key;
    char *value;
};

//ZN_HttpKeyValue * ZN_HttpParamValue_New(char *name, char *value);
ZN_Array 	*	ZN_HttpKeyValueArray_New(void);
bool 			ZN_HttpKeyValueArray_Push(ZN_Array *array, const char *key, const char *value);

#endif
