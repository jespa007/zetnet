#ifndef __ZN_ARRAY__
#define __ZN_ARRAY__

#define	ZN_ARRAY_NEW(_type)								ZN_Array_New(#_type,sizeof(_type))
#define	ZN_ARRAY_SET(_type,_array,_pos, _data)			ZN_Array_Set(_array,#_type,sizeof(_type),_pos, &_data)
#define	ZN_ARRAY_PUSH(_type,_array, _data)				ZN_Array_Push(_array,#_type,sizeof(_type),&_data)
#define	ZN_ARRAY_GET_DATA(_type,_array)					(_type *)ZN_Array_GetData(_array,#_type,sizeof(_type))
#define	ZN_ARRAY_GET(_type,_array,_pos)					(_type *)ZN_Array_Get(_array,#_type,sizeof(_type),_pos)
#define ZN_ARRAY_FOR(type, arr, i) \
    for(size_t i = 0; i < ZN_Array_Count(arr); i++)


typedef struct ZN_Array ZN_Array;
typedef struct ZN_ArrayData ZN_ArrayData;

struct ZN_Array{
	ZN_ArrayData	*data;
	void	(*destructor_item)(void *_item);
};


// Member
ZN_Array	*	ZN_Array_New(const char * _name, size_t _size);
void		*	ZN_Array_GetData(ZN_Array *_this, const char * _name, size_t _data_size);
void		*	ZN_Array_Get(ZN_Array *_this, const char * _name, size_t _data_size, size_t _pos);
void			ZN_Array_Set(ZN_Array *_this, const char * _name, size_t _data_size, size_t _pos, void *_data );
void			ZN_Array_Erase(ZN_Array *_this, const char * _name, size_t _data_size, size_t _pos);
void			ZN_Array_InsertAt(ZN_Array *_this, const char * _name, size_t _data_size, size_t _pos, void *_data );

void			ZN_Array_Push(ZN_Array *_this, const char * _name, size_t _data_size, void *_data);
size_t 			ZN_Array_Count(ZN_Array *_this);
void			ZN_Array_Clear(ZN_Array *_this);
void			ZN_Array_Delete(ZN_Array *_this);




#endif
