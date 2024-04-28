#ifndef ADD_HASHTABLE_H
#define ADD_HASHTABLE_H

#include "linked_list.h"

typedef struct hashtable_t {
	ll_list_t **buckets;
	unsigned int size;
	unsigned int hmax;
	unsigned int (*hash_function)(void *);
	int (*compare_function)(void *, void *);
} hashtable_t;

hashtable_t *ht_create(unsigned int hmax, unsigned int (*hash_function)(void *),
					   int (*compare_function)(void *, void *));

int ht_has_key(hashtable_t *ht, void *key);

void *ht_get(hashtable_t *ht, void *key);

void ht_put(hashtable_t *ht, void *key, unsigned int key_size, void *value,
			unsigned int value_size);

void ht_remove_entry(hashtable_t *ht, void *key);

void ht_free(hashtable_t *ht);

unsigned int ht_get_size(hashtable_t *ht);

unsigned int ht_get_hmax(hashtable_t *ht);

int compare_strings(void *a, void *b);

#endif /* ADD_HASHTABLE_H */