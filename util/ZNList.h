#ifndef __ZNLIST_H__
#define __ZNLIST_H__

typedef struct ZNList ZNList;

struct ZNList{
	void** items;
	uint16_t size; // size vector (user count for iterate through items)
	uint16_t count; //number of items
};

ZNList  	*	ZNList_New();

void 		ZNList_Set(ZNList *this, uint16_t idx, void *e);
void	*	ZNList_Get(ZNList *this, uint16_t idx);
void 		ZNList_Erase(ZNList *this, uint16_t idx);
/**
 * Insert item at the end
 */
void 		ZNList_Add(ZNList *this, void *e);
/**
 * Insert item at position idx.
 */
void 		ZNList_Insert(ZNList *this, uint16_t idx, void *e);

void 		ZNList_Clear(ZNList *this);

size_t 		ZNList_Count(ZNList *this);
void 		ZNList_Delete(ZNList *this);

/**
 * Deletes list and its elements.
 */
void 		ZNList_DeleteAndFreeAllItems(ZNList *v);



#endif
