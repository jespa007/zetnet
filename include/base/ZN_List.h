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

ZN_DLL_EXPORT ZN_List  *	ZN_List_New(void);

ZN_DLL_EXPORT void 		ZN_List_Set(ZN_List *_this, uint16_t idx, void *e);
ZN_DLL_EXPORT void	*	ZN_List_Get(ZN_List *_this, uint16_t idx);
ZN_DLL_EXPORT void 		ZN_List_Erase(ZN_List *_this, uint16_t idx);
/**
 * Insert item at the end
 */
ZN_DLL_EXPORT void 		ZN_List_Add(ZN_List *_this, void *e);
ZN_DLL_EXPORT void 		ZN_List_Concat(ZN_List *_this, ZN_List *list);
/**
 * Insert item at position idx.
 */
ZN_DLL_EXPORT void 		ZN_List_Insert(ZN_List *_this, uint16_t idx, void *e);


ZN_DLL_EXPORT void 		ZN_List_Clear(ZN_List *_this);

ZN_DLL_EXPORT size_t 		ZN_List_Count(ZN_List *_this);
ZN_DLL_EXPORT void 		ZN_List_Delete(ZN_List *_this);

/**
 * Deletes list and its elements.
 */
ZN_DLL_EXPORT void 		ZN_List_DeleteAndFreeAllItems(ZN_List *v);



#endif
