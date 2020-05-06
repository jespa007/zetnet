#ifndef __ZNLIST_H__
#define __ZNLIST_H__

typedef struct ZNList ZNList;

struct ZNList{
	//public
	void** items;
	uint16_t count; //number of items

	//private
	uint16_t _size; // size vector (user count for iterate through items)
};

ZNList  	*	ZNList_New(void);

void 		ZNList_Set(ZNList *_this, uint16_t idx, void *e);
void	*	ZNList_Get(ZNList *_this, uint16_t idx);
void 		ZNList_Erase(ZNList *_this, uint16_t idx);
/**
 * Insert item at the end
 */
void 		ZNList_Add(ZNList *_this, void *e);
void 		ZNList_Concat(ZNList *_this, ZNList *list);
/**
 * Insert item at position idx.
 */
void 		ZNList_Insert(ZNList *_this, uint16_t idx, void *e);


void 		ZNList_Clear(ZNList *_this);

size_t 		ZNList_Count(ZNList *_this);
void 		ZNList_Delete(ZNList *_this);

/**
 * Deletes list and its elements.
 */
void 		ZNList_DeleteAndFreeAllItems(ZNList *v);



#endif
