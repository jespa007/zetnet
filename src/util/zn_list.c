#include "zetnet.h"

#define MAX_ELEMENTS_VECTOR	16000

bool	zn_list_add_slot(zn_list *v){
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

zn_list * zn_list_new(){
	zn_list *v=ZN_MALLOC(sizeof(zn_list));
	return v;
}



void zn_list_set(zn_list *v, uint16_t idx, void *e){
	if (idx >= v->count) {
		fprintf(stderr,"idx out of bounds");
		return;
	}
	v->items[idx] = e;
}

size_t zn_list_count(zn_list *v){
	return v->count;
}

void *zn_list_get(zn_list *v, uint16_t idx){
	if (idx >= v->count) {
		fprintf(stderr,"idx out of bounds");
		return NULL;
	}

	return v->items[idx];
}

void zn_list_erase(zn_list *_this, uint16_t idx){
	if (idx >= _this->count) {
		fprintf(stderr,"idx out of bounds");
		return;
	}

	_this->items[idx] = NULL;

	for (int i = idx; i < (_this->count-1); i++) {
		_this->items[i] = _this->items[i+1];
	}

	_this->items[_this->count-1] = NULL;
	_this->count--;
}

void zn_list_add(zn_list *v, void *e){
	if(zn_list_add_slot(v)){
		v->items[v->count-1] = e; // add element to end list...
	}
}

void zn_list_concat(zn_list *_this, zn_list *list){
	if(_this != NULL && list) {
		for(unsigned i=0; i <  list->count; i++){
			zn_list_add(_this, list->items[i]);
		}
	}

}

void 		zn_list_insert(zn_list *v, uint16_t idx, void *e){
	if(idx > (v->count+1)){
		fprintf(stderr,"idx should be 0 to %i",v->count+1);
		return;
	}

	if(zn_list_add_slot(v)){
		// 1. move all elements...
		for(int i=v->count-1;i>idx;i--){
			v->items[i]=v->items[i-1];
		}
		// 2. Assign element
		v->items[idx]=e;
	}
}

void 		zn_list_clear(zn_list *_this){
	if(_this->items!=NULL){
		ZN_FREE(_this->items);
	}
	memset(_this,0,sizeof(zn_list));
}

void zn_list_delete(zn_list *_this){
	if(_this->items!=NULL){
		ZN_FREE(_this->items);
	}
	ZN_FREE(_this);
}

void zn_list_delete_and_free_all_items(zn_list *_this){
	if(_this!=NULL){
		for(unsigned i=0; i < _this->count; i++){
			ZN_FREE(_this->items[i]);
		}
		zn_list_delete(_this);
	}
}

