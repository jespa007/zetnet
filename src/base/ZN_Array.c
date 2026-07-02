#include "zetnet.h"

#define ZN_MAX_ELEMENTS_ARRAY	16000

struct ZN_ArrayData{
	uint8_t	*ptr_data;
	size_t type_size;
	const char * type_name;
	size_t count;
	size_t capacity;
};


//--------------
// ARRAY MEMBERS

ZN_Array *	ZN_Array_New(const char *_type_name, size_t _type_size){

	ZN_Array *array=NULL;


	array = ZN_NEW(ZN_Array);
	ZN_ArrayData *data = ZN_NEW(ZN_ArrayData);
	data->type_size = _type_size;
	data->type_name = _type_name;

	array->data = data;

	return array;
}

void ZN_Array_CheckType(ZN_Array *_this, const char * _name, size_t _type_size){

	if(!_this){
		ZN_LOG_ERRORF("ZN_Array_CheckType : container is NULL");
	}


	ZN_ArrayData * data = _this->data;
	bool same_size = _type_size == data->type_size;


	if(!same_size){
		ZN_LOG_ERROR("ZN_Array_CheckType : Element has not the same SIZE TYPE as its container (container type : %s . element type : %s)",data->type_name,_name);
	}

	bool same_name = true;

#ifdef __DEBUG__
	same_name = strcmp(_name,data->type_name) == 0;
	if(!same_name){
		ZN_LOG_ERROR("ZN_Array_CheckType : Element has not the same NAME TYPE as its container (container type : %s . element type : %s)",data->type_name,_name);
	}

#endif

	assert(same_size && same_name && _this);//,"ZN_Array_CheckType not same type or same name");
}

bool 		ZN_Array_AddSlot(ZN_Array *_this){
	ZN_ArrayData * data = _this->data;

	if (data->capacity == 0) {
		data->capacity = 10;
		data->ptr_data = ZN_MALLOC(data->type_size * data->capacity);
		memset(data->ptr_data, 0, data->type_size * data->capacity);
	}

	// condition to increase v->items:
	// last slot exhausted
	if (data->capacity == data->count) {
		size_t new_capacity = data->capacity + (data->capacity >> 1);
		if(new_capacity >= ZN_MAX_ELEMENTS_ARRAY){
			ZN_LOG_ERRORF("ZN_Array_AddSlot : Max elements array");
			return false;
		}
		data->capacity = new_capacity;

		void *tmp = realloc(data->ptr_data, data->type_size * data->capacity);
		if (!tmp) {
		    ZN_LOG_ERRORF("ZN_Array_AddSlot : Out of memory");
		    return false;
		}
		data->ptr_data = tmp;
	}

	data->count++;
	return true;
}

static inline uint8_t *ZN_Array_PtrAt(ZN_Array *_this, size_t pos) {
	ZN_ArrayData * data = _this->data;
    return data->ptr_data + data->type_size * pos;
}
/*
uint8_t 	*ZN_Array_PtrAt(ZN_Array *_this, size_t _pos){
	ZN_ArrayData * data = _this->data;

	if(_pos >= data->count){
		ZN_LOG_ERRORF("ZN_Array_PtrAt : '_pos' out of bounds");
		return NULL;
	}

	return data->ptr_data + data->type_size * _pos;
}*/

void  ZN_Array_Set(ZN_Array *_this, const char * _type_name, size_t _type_size, size_t _pos, void * _data){
	ZN_Array_CheckType(_this,_type_name,_type_size);

	ZN_ArrayData * data = _this->data;

	if (_pos >= data->count) {
		ZN_LOG_ERRORF("idx out of bounds");
		return;
	}

	uint8_t * item_ptr = ZN_Array_PtrAt(_this, _pos);

	if(_this->destructor_item){
		_this->destructor_item(item_ptr);
	}

	// copy data
	memcpy(item_ptr,_data,data->type_size);
}

void 	*ZN_Array_GetData(ZN_Array *_this, const char * _type_name, size_t _type_size){
	ZN_Array_CheckType(_this,_type_name,_type_size);
	ZN_ArrayData * data = _this->data;
	return data->ptr_data;
}


void * ZN_Array_Get(ZN_Array *_this, const char * _type_name, size_t _type_size, size_t _pos){
	ZN_Array_CheckType(_this,_type_name,_type_size);
	return ZN_Array_PtrAt(_this,_pos);
}

void 		ZN_Array_InsertAt(ZN_Array *_this, const char * _type_name, size_t _type_size, size_t _pos, void * _element){

	ZN_Array_CheckType(_this,_type_name,_type_size);

	ZN_ArrayData * data = _this->data;
	if (_pos > (data->count)) {
		ZN_LOG_ERROR("ZN_Array_Insert : _pos should be 0 to %i",data->count);
		return;
	}

	if(ZN_Array_AddSlot(_this)){
		uint8_t *base = data->ptr_data;
		uint8_t *dst = base + (_pos + 1) * data->type_size;
		uint8_t *src = base + _pos * data->type_size;

		size_t bytes_to_move = (data->count - 1 - _pos) * data->type_size;

		memmove(dst, src, bytes_to_move);
		memcpy(src, _element, data->type_size);
	}
}

void ZN_Array_Erase(ZN_Array *_this, const char * _type_name, size_t _type_size, size_t _pos){

	ZN_Array_CheckType(_this,_type_name,_type_size);

	ZN_ArrayData * data = _this->data;

	if (_pos >= data->count) {
		ZN_LOG_ERRORF("ZN_Array_Erase : _pos out of bounds");
		return;
	}

	uint8_t *cursor = ZN_Array_PtrAt(_this, _pos);

	if(_this->destructor_item){
		_this->destructor_item(cursor);
	}

	// rellocate elements
	uint8_t *dst = ZN_Array_PtrAt(_this, _pos);
	uint8_t *src = dst + data->type_size;

	size_t bytes_to_move = (data->count - 1 - _pos) * data->type_size;

	memmove(dst, src, bytes_to_move);


	//_this->items[_this->count-1] = NULL;
	data->count--;

	if (data->count < data->capacity / 4 && data->capacity > 10) {
	    size_t new_capacity = data->capacity / 2;
	    void *tmp = realloc(data->ptr_data, new_capacity * data->type_size);
	    if (tmp) {
	        data->ptr_data = tmp;
	        data->capacity = new_capacity;
	    }
	}
}

size_t 		ZN_Array_Count(ZN_Array *_this){
	ZN_ArrayData * data = _this->data;
	return data->count;
}

void		ZN_Array_Push(ZN_Array *_this, const char * _type_name, size_t _type_size, void *_data){

	ZN_Array_CheckType(_this,_type_name,_type_size);

	ZN_ArrayData * data = _this->data;

	if(ZN_Array_AddSlot(_this)){
		uint8_t *ptr = ZN_Array_PtrAt(_this,data->count-1);
		memcpy(ptr,_data,data->type_size);
	}
}

void		ZN_Array_Clear(ZN_Array *_this){
	ZN_ArrayData * data = _this->data;

	if(data){

		if(data->ptr_data != NULL){
			uint8_t *cursor = data->ptr_data;
			if(_this->destructor_item != NULL){
				for(size_t i = 0; i < data->count; i++){
					_this->destructor_item(cursor);
					cursor += data->type_size;
				}
			}

			ZN_FREE(data->ptr_data);
		}

		data->ptr_data = NULL;
		data->capacity=0;
		data->count=0;
	}
}

void 		ZN_Array_Delete(ZN_Array *_this){
	ZN_ArrayData * data = _this->data;

	if(data){
		ZN_Array_Clear(_this);
		ZN_FREE(data);
		data=NULL;
	}

	ZN_FREE(_this);
}

