#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

struct linked_list {
	struct linked_list_node *head;
	int count;
};

extern struct linked_list *linked_list_create(void);
extern void linked_list_destroy(struct linked_list *llist);
extern void *linked_list_insert(struct linked_list *llist, void *data);
extern void *linked_list_append(struct linked_list *llist, void *data);
extern void *linked_list_head(struct linked_list *llist);
extern void *linked_list_tail(struct linked_list *llist);
extern void *linked_list_find(struct linked_list *llist, void *data, int (*cmpfn)(void *, void *));
extern void *linked_list_delete(struct linked_list *llist, void *data, int (*cmpfn)(void *, void *));
extern int linked_list_count(struct linked_list *llist);
extern void linked_list_foreach(struct linked_list *llist, void (*f)(void *, void *), void *arg);
extern void **linked_list_array_get(struct linked_list *llist);
extern void linked_list_array_free(void **a);

#endif