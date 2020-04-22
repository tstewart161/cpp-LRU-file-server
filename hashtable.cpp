#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "linked-list.h"
#include "hashtable.h"

#define DEFAULT_SIZE 128
#define DEFAULT_GROW_FACTOR 2

// Hash table entry
struct htent {
    void *key;
    int key_size;
    int hashed_key;
    void *data;
};

// Used to cleanup the linked lists
struct foreach_callback_payload {
	void *arg;
	void (*f)(void *, void *);
};

/**
 * Change the entry count, maintain load metrics
 */
void add_entry_count(struct hashtable *ht, int d) {
    ht->num_entries += d;
    ht->load = (float)ht->num_entries / ht->size;
}

/**
 * Default modulo hashing function
 */
int default_hashf(void *data, int data_size, int bucket_count) {
    const int R = 31; // Small prime
    int h = 0;
    unsigned char *p = data;

    for (int i = 0; i < data_size; i++) {
        h = (R * h + p[i]) % bucket_count;
    }

    return h;
}


struct hashtable *hashtable_create(int size, int (*hashf)(void *, int, int)) {
    if (size < 1) {
        size = DEFAULT_SIZE;
    }

    if (hashf == NULL) {
        hashf = default_hashf;
    }

    struct hashtable *ht = malloc(sizeof *ht);

    if (ht == NULL) return NULL;

    ht->size = size;
    ht->num_entries = 0;
    ht->load = 0;
    ht->bucket = malloc(size * sizeof(struct linked_list *));
    ht->hashf = hashf;

    for (int i = 0; i < size; i++) {
        ht->bucket[i] = linked_list_create();
    }

    return ht;
}


void htent_free(void *htent, void *arg) {
	(void)arg;
	free(htent);
}


void hashtable_destroy(struct hashtable *ht) {
    for (int i = 0; i < ht->size; i++) {
        struct linked_list *linked_list = ht->bucket[i];

		linked_list_foreach(linked_list, htent_free, NULL);
        linked_list_destroy(linked_list);
    }

    free(ht);
}


void *hashtable_put(struct hashtable *ht, char *key, void *data) {
    return hashtable_put_bin(ht, key, strlen(key), data);
}


void *hashtable_put_bin(struct hashtable *ht, void *key, int key_size, void *data) {
    int index = ht->hashf(key, key_size, ht->size);

    struct linked_list *linked_list = ht->bucket[index];

    struct htent *ent = malloc(sizeof *ent);
    ent->key = malloc(key_size);
    memcpy(ent->key, key, key_size);
    ent->key_size = key_size;
    ent->hashed_key = index;
    ent->data = data;

    if (linked_list_append(linked_list, ent) == NULL) {
        free(ent->key);
        free(ent);
        return NULL;
    }

    add_entry_count(ht, +1);

    return data;
}


int htcmp(void *a, void *b) {
    struct htent *entA = a, *entB = b;

    int size_diff = entB->key_size - entA->key_size;

    if (size_diff) {
        return size_diff;
    }

    return memcmp(entA->key, entB->key, entA->key_size);
}


void *hashtable_get(struct hashtable *ht, char *key) {
    return hashtable_get_bin(ht, key, strlen(key));
}


void *hashtable_get_bin(struct hashtable *ht, void *key, int key_size) {
    int index = ht->hashf(key, key_size, ht->size);

    struct linked_list *linked_list = ht->bucket[index];

    struct htent cmpent;
    cmpent.key = key;
    cmpent.key_size = key_size;

    struct htent *n = linked_list_find(linked_list, &cmpent, htcmp);

    if (n == NULL) { return NULL; }

    return n->data;
}


void *hashtable_delete(struct hashtable *ht, char *key) {
    return hashtable_delete_bin(ht, key, strlen(key));
}


void *hashtable_delete_bin(struct hashtable *ht, void *key, int key_size) {
    int index = ht->hashf(key, key_size, ht->size);

    struct linked_list *linked_list = ht->bucket[index];

    struct htent cmpent;
    cmpent.key = key;
    cmpent.key_size = key_size;

    struct htent *ent = linked_list_delete(linked_list, &cmpent, htcmp);

	if (ent == NULL) {
		return NULL;
	}

	void *data = ent->data;

	free(ent);

    add_entry_count(ht, -1);

	return data;
}


void foreach_callback(void *vent, void *vpayload) {
	struct htent *ent = vent;
	struct foreach_callback_payload *payload = vpayload;

	payload->f(ent->data, payload->arg);
}


void hashtable_foreach(struct hashtable *ht, void (*f)(void *, void *), void *arg) {
	struct foreach_callback_payload payload;

	payload.f = f;
	payload.arg = arg;

	for (int i = 0; i < ht->size; i++) {
		struct linked_list *linked_list = ht->bucket[i];

		linked_list_foreach(linked_list, foreach_callback, &payload);
	}
}