#ifndef __ZN_LIST_H__
#define __ZN_LIST_H__

typedef struct zn_list zn_list;

struct zn_list{
	//public
	void** items;
	uint16_t count; //number of items

	//private
	uint16_t _size; // size vector (user count for iterate through items)
};

zn_list  	*	ZNList_New(void);

void 		ZNList_Set(zn_list *_this, uint16_t idx, void *e);
void	*	ZNList_Get(zn_list *_this, uint16_t idx);
void 		ZNList_Erase(zn_list *_this, uint16_t idx);
/**
 * Insert item at the end
 */
void 		ZNList_Add(zn_list *_this, void *e);
void 		ZNList_Concat(zn_list *_this, zn_list *list);
/**
 * Insert item at position idx.
 */
void 		ZNList_Insert(zn_list *_this, uint16_t idx, void *e);


void 		ZNList_Clear(zn_list *_this);

size_t 		ZNList_Count(zn_list *_this);
void 		ZNList_Delete(zn_list *_this);

/**
 * Deletes list and its elements.
 */
void 		ZNList_DeleteAndFreeAllItems(zn_list *v);



#endif
