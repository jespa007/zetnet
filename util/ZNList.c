#include "zetnet.h"

#define MAX_ELEMENTS_VECTOR	16000

bool	ZNList_AddSlot(ZNList *v){
	if (v->_size == 0) {
		v->_size = 10;
		v->items = ZN_MALLOC(sizeof(void*) * v->_size);
	}

	// condition to increase v->items:
	// last slot exhausted
	if (v->_size == v->count) {
		if((v->_size+10) >= MAX_ELEMENTS_VECTOR){
			fprintf(stderr,"Max elements vector");
			return false;
		}
		v->_size += 10;
		v->items = realloc(v->items, sizeof(void*) * v->_size);
	}

	v->count++;
	return true;
}

ZNList * ZNList_New(){
	ZNList *v=ZN_MALLOC(sizeof(ZNList));
	return v;
}



void ZNList_Set(ZNList *v, uint16_t idx, void *e){
	if (idx >= v->count) {
		fprintf(stderr,"idx out of bounds");
		return;
	}
	v->items[idx] = e;
}

size_t ZNList_Count(ZNList *v){
	return v->count;
}

void *ZNList_Get(ZNList *v, uint16_t idx){
	if (idx >= v->count) {
		fprintf(stderr,"idx out of bounds");
		return NULL;
	}

	return v->items[idx];
}

void ZNList_Erase(ZNList *_this, uint16_t idx){
	if (idx >= _this->count) {
		fprintf(stderr,"idx out of bounds");
		return;
	}

	_this->items[idx] = NULL;

	for (unsigned i = idx; i < (_this->count-1); i++) {
		_this->items[i] = _this->items[i+1];
	}

	_this->items[_this->count-1] = NULL;
	_this->count--;
}

void ZNList_Add(ZNList *v, void *e){
	if(ZNList_AddSlot(v)){
		v->items[v->count-1] = e; // add element to end list...
	}
}

void ZNList_Concat(ZNList *_this, ZNList *list){
	if(_this != NULL && list) {
		for(unsigned i=0; i <  list->count; i++){
			ZNList_Add(_this, list->items[i]);
		}
	}

}

void 		ZNList_Insert(ZNList *v, uint16_t idx, void *e){
	if(idx > (v->count+1)){
		fprintf(stderr,"idx should be 0 to %i",v->count+1);
		return;
	}

	if(ZNList_AddSlot(v)){
		// 1. move all elements...
		for(int i=v->count-1;i>idx;i--){
			v->items[i]=v->items[i-1];
		}
		// 2. Assign element
		v->items[idx]=e;
	}
}

void 		ZNList_Clear(ZNList *_this){
	if(_this->items!=NULL){
		ZN_FREE(_this->items);
	}
	memset(_this,0,sizeof(ZNList));
}

void ZNList_Delete(ZNList *_this){
	if(_this->items!=NULL){
		ZN_FREE(_this->items);
	}
	ZN_FREE(_this);
}

void ZNList_DeleteAndFreeAllItems(ZNList *_this){
	if(_this!=NULL){
		for(unsigned i=0; i < _this->count; i++){
			ZN_FREE(_this->items[i]);
		}
		ZNList_Delete(_this);
	}
}

