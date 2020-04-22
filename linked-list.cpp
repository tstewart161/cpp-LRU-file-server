#include <stdlib.h>
#include "linked-list.h"

struct linked_list_node {
	void *data;
	struct linked_list_node *next;
};

struct linked_list *linked_list_create(void) {
    return calloc(1, sizeof(struct linked_list));
}

// Does not deallocate data in each node.
void linked_list_destroy(struct linked_list *linked_list) {
    struct linked_list_node *n = linked_list->head, *next;
    
    while (n != NULL) {
        next = n->next;
        free(n);

        n = next;
    }
    free(linked_list);
}

void *linked_list_insert(struct linked_list *linked_list, void *data) {
	struct linked_list_node *n = calloc(1, sizeof *n);

	if (n == NULL) {
		return NULL;
	}

	n->data = data;
	n->next = linked_list->head;
	linked_list->head = n;

	linked_list->count++;

	return data;
}

void *linked_list_append(struct linked_list *linked_list, void *data) {
	struct linked_list_node *tail = linked_list->head;

	// If list is empty, just insert
	if (tail == NULL) {
		return linked_list_insert(linked_list, data);
	}

	struct linked_list_node *n = calloc(1, sizeof *n);

	if (n == NULL) {
		return NULL;
	}

	while (tail->next != NULL) {
		tail = tail->next;
	}

	n->data = data;
	tail->next = n;

	linked_list->count++;

	return data;
}


void *linked_list_head(struct linked_list *linked_list) {
	if (linked_list->head == NULL) {
		return NULL;
	}

	return linked_list->head->data;
}


void *linked_list_tail(struct linked_list *linked_list) {
	struct linked_list_node *n = linked_list->head;

	if (n == NULL) {
		return NULL;
	}

	while (n->next != NULL) {
		n = n->next;
	}

	return n->data;
}


void *linked_list_find(struct linked_list *linked_list, void *data, int (*cmpfn)(void *, void *)) {
	struct linked_list_node *n = linked_list->head;

	if (n == NULL) {
		return NULL;
	}

	while (n != NULL) {
		if (cmpfn(data, n->data) == 0) {
			break;
		}

		n = n->next;
	}

	if (n == NULL) {
		return NULL;
	}

	return n->data;
}


void *linked_list_delete(struct linked_list *linked_list, void *data, int (*cmpfn)(void *, void *)) {
	struct linked_list_node *n = linked_list->head, *prev = NULL;

	while (n != NULL) {
		if (cmpfn(data, n->data) == 0) {

			void *data = n->data;

			if (prev == NULL) {
				// Free the head
				linked_list->head = n->next;
				free(n);

			} else {
				// Free the non-head
				prev->next = n->next;
				free(n);
			}

			linked_list->count--;

			return data;
		}

		prev = n;
		n = n->next;
	}

	return NULL;
}


int linked_list_count(struct linked_list *linked_list) {
	return linked_list->count;
}


void linked_list_foreach(struct linked_list *linked_list, void (*f)(void *, void *), void *arg) {
	struct linked_list_node *p = linked_list->head, *next;

	while (p != NULL) {
		next = p->next;
		f(p->data, arg);
		p = next;
	}
}


void **linked_list_array_get(struct linked_list *linked_list) {
	if (linked_list->head == NULL) {
		return NULL;
	}

	void **a = malloc(sizeof *a * linked_list->count + 1);

	struct linked_list_node *n;
	int i;

	for (i = 0, n = linked_list->head; n != NULL; i++, n = n->next) {
		a[i] = n->data;
	}

	a[i] = NULL;

	return a;
}


void linked_list_array_free(void **a) {
	free(a);
}