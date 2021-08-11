#ifndef __ZNLIST_H__
#define __ZNLIST_H__

typedef struct zn_list zn_list;

struct zn_list{
	//public
	void** items;
	uint16_t count; //number of items

	//private
	uint16_t _size; // size vector (user count for iterate through items)
};

zn_list  	*	zn_list_new(void);

void 		zn_list_set(zn_list *_this, uint16_t idx, void *e);
void	*	zn_list_get(zn_list *_this, uint16_t idx);
void 		zn_list_erase(zn_list *_this, uint16_t idx);
/**
 * Insert item at the end
 */
void 		zn_list_add(zn_list *_this, void *e);
void 		zn_list_concat(zn_list *_this, zn_list *list);
/**
 * Insert item at position idx.
 */
void 		zn_list_insert(zn_list *_this, uint16_t idx, void *e);


void 		zn_list_clear(zn_list *_this);

size_t 		zn_list_count(zn_list *_this);
void 		zn_list_delete(zn_list *_this);

/**
 * Deletes list and its elements.
 */
void 		zn_list_delete_and_free_all_items(zn_list *v);



#endif
