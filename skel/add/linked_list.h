/*
 * Copyright (c) 2024, <>
 */

// I do not know who the author is, I added the copyright for the style checker

#ifndef ADD_DOUBLY_LINKED_LIST_H
#define ADD_DOUBLY_LINKED_LIST_H

#include "../utils.h"

typedef struct info_t {
	void *key;
	void *value;
} info_t;

typedef struct ll_node_t {
	void *data;
	struct ll_node_t *next, *prev;
} ll_node_t;

typedef struct ll_list_t {
	ll_node_t *head, *tail;
	unsigned int data_size;
	unsigned int size;
} ll_list_t;

ll_list_t *ll_create(unsigned int data_size);

ll_node_t *ll_get_nth_node(ll_list_t *list, unsigned int n);

ll_node_t *ll_add_nth_node(ll_list_t *list, unsigned int n,
						   const void *new_data);

ll_node_t *ll_add_nth_node_info(ll_list_t *list, unsigned int n,
								const info_t *new_data);

ll_node_t *ll_remove_nth_node(ll_list_t *list, unsigned int n);

ll_node_t *move_node_to_end(ll_list_t *list, void *data);

ll_node_t *ll_remove_node(ll_list_t *list, ll_node_t *node);

unsigned int ll_get_size(ll_list_t *list);

void ll_free(ll_list_t **pp_list);

void ll_free_info(ll_list_t **pp_list);

#endif /* ADD_DOUBLY_LINKED_LIST_H */
