/*
 * Copyright (c) 2024, <Ungureanu Vlad-Marin> <<2004uvm@gmail.com>>
 */

#ifndef SPECIFIC_LINKED_LIST_H
#define SPECIFIC_LINKED_LIST_H

#include "linked_list.h"

ll_node_t *ll_add_nth_node_info(ll_list_t *list, unsigned int n,
								const info_t *new_data);

ll_node_t *move_node_to_end(ll_list_t *list, void *key);

void ll_free_info(ll_list_t **pp_list);

ll_node_t *ll_get_node(ll_list_t *list, void *key);

#endif /* SPECIFIC_LINKED_LIST_H */
