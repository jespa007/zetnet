#ifndef __ZN_ARRAY_CSTR__
#define __ZN_ARRAY_CSTR__


typedef struct ZN_ArrayCStr ZN_ArrayCStr;
struct ZN_ArrayCStr{
	void *data;
};

ZN_ArrayCStr	*	ZN_ArrayCstr_New(void);
bool				ZN_ArrayCstr_Push(ZN_ArrayCStr	* _this, const char *_str);
const char	**		ZN_ArrayCstr_GetData(ZN_ArrayCStr	* _this);
const char 	*		ZN_ArrayCstr_Get(ZN_ArrayCStr *_this, size_t index);
size_t				ZN_ArrayCstr_Count(ZN_ArrayCStr *_this);
void				ZN_ArrayCstr_Delete(ZN_ArrayCStr	* _this);


#endif
