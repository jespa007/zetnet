#include "zetnet.h"

#define MAX_ELEMENTS_VECTOR	16000

bool	ZN_List_AddSlot(ZN_List *v){
	if (v->_size == 0) {
		v->_size = 10;
		v->items = malloc(sizeof(void*) * v->_size);
	}

	// condition to increase v->items:
	// last slot exhausted
	if (v->_size == v->count) {
		if((v->_size+10) >= MAX_ELEMENTS_VECTOR){
			fprintf(stderr,"\nMax elements vector");
			return false;
		}
		v->_size += 10;
		v->items = realloc(v->items, sizeof(void*) * v->_size);
	}

	v->count++;
	return true;
}

ZN_List * ZN_List_New(){
	ZN_List *v=ZN_NEW(ZN_List);
	return v;
}



void ZN_List_Set(ZN_List *v, uint16_t idx, void *e){
	if (idx >= v->count) {
		fprintf(stderr,"\nidx out of bounds");
		return;
	}
	v->items[idx] = e;
}

size_t ZN_List_Count(ZN_List *v){
	return v->count;
}

void *ZN_List_Get(ZN_List *v, uint16_t idx){
	if (idx >= v->count) {
		fprintf(stderr,"\nidx out of bounds");
		return NULL;
	}

	return v->items[idx];
}

void ZN_List_Erase(ZN_List *_this, uint16_t idx){
	if (idx >= _this->count) {
		fprintf(stderr,"\nidx out of bounds");
		return;
	}

	_this->items[idx] = NULL;

	for (int i = idx; i < (_this->count-1); i++) {
		_this->items[i] = _this->items[i+1];
	}

	_this->items[_this->count-1] = NULL;
	_this->count--;
}

void ZN_List_Add(ZN_List *v, void *e){
	if(ZN_List_AddSlot(v)){
		v->items[v->count-1] = e; // add element to end list...
	}
}

void ZN_List_Concat(ZN_List *_this, ZN_List *list){
	if(_this != NULL && list) {
		for(unsigned i=0; i <  list->count; i++){
			ZN_List_Add(_this, list->items[i]);
		}
	}

}

void 		ZN_List_Insert(ZN_List *v, uint16_t idx, void *e){
	if(idx > (v->count+1)){
		fprintf(stderr,"\nidx should be 0 to %i",v->count+1);
		return;
	}

	if(ZN_List_AddSlot(v)){
		// 1. move all elements...
		for(int i=v->count-1;i>idx;i--){
			v->items[i]=v->items[i-1];
		}
		// 2. Assign element
		v->items[idx]=e;
	}
}

void 		ZN_List_Clear(ZN_List *_this){
	if(_this->items!=NULL){
		ZN_FREE(_this->items);
	}
	memset(_this,0,sizeof(ZN_List));
}

void ZN_List_Delete(ZN_List *_this){
	if(_this->items!=NULL){
		ZN_FREE(_this->items);
	}
	ZN_FREE(_this);
}

void ZN_List_DeleteAndFreeAllItems(ZN_List *_this){
	if(_this!=NULL){
		for(unsigned i=0; i < _this->count; i++){
			ZN_FREE(_this->items[i]);
		}
		ZN_List_Delete(_this);
	}
}

