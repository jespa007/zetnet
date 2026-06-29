#ifndef __ZN_LIST_H__
#define __ZN_LIST_H__

typedef struct ZN_List ZN_List;

struct ZN_List{
	//public
	void** items;
	uint16_t count; //number of items

	//private
	uint16_t _size; // size vector (user count for iterate through items)
};

ZN_List  	*	ZN_List_New(void);

void 		ZN_List_Set(ZN_List *_this, uint16_t idx, void *e);
void	*	ZN_List_Get(ZN_List *_this, uint16_t idx);
void 		ZN_List_Erase(ZN_List *_this, uint16_t idx);
/**
 * Insert item at the end
 */
void 		ZN_List_Add(ZN_List *_this, void *e);
void 		ZN_List_Concat(ZN_List *_this, ZN_List *list);
/**
 * Insert item at position idx.
 */
void 		ZN_List_Insert(ZN_List *_this, uint16_t idx, void *e);


void 		ZN_List_Clear(ZN_List *_this);

size_t 		ZN_List_Count(ZN_List *_this);
void 		ZN_List_Delete(ZN_List *_this);

/**
 * Deletes list and its elements.
 */
void 		ZN_List_DeleteAndFreeAllItems(ZN_List *v);



#endif
