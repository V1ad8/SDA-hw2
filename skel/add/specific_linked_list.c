/*
 * Copyright (c) 2024, <Ungureanu Vlad-Marin> <<2004uvm@gmail.com>>
 */

#include "specific_linked_list.h"

ll_node_t *ll_add_nth_node_info(ll_list_t *list, unsigned int n,
								const info_t *new_data)
{
	// Almost identical to ll_add_nth_node, but with a different data type
	// and a different way of copying the data

	ll_node_t *prev_node;

	if (!list)
		return NULL;

	if (n > list->size)
		return NULL;

	// Allocate memory for the new node
	ll_node_t *new_node = calloc(1, sizeof(*new_node));
	DIE(!new_node, "calloc node failed");

	// Allocate memory for the data
	new_node->data = malloc(sizeof(info_t));
	DIE(!new_node->data, "malloc data failed");

	// Copy the key
	((info_t *)new_node->data)->key = strdup(new_data->key);
	DIE(!((info_t *)new_node->data)->key, "strdup key failed");

	// Copy the value
	((info_t *)new_node->data)->value = strdup(new_data->value);
	DIE(!((info_t *)new_node->data)->value, "strdup value failed");

	if (n == 0) {
		new_node->next = list->head;
		if (list->head)
			list->head->prev = new_node;
		list->head = new_node;
		if (!list->tail)
			list->tail = new_node;
	} else if (n == list->size) {
		new_node->prev = list->tail;
		list->tail->next = new_node;
		list->tail = new_node;
	} else {
		prev_node = ll_get_nth_node(list, n - 1);
		new_node->prev = prev_node;
		new_node->next = prev_node->next;
		prev_node->next->prev = new_node;
		prev_node->next = new_node;
	}

	++list->size;

	return new_node;
}

ll_node_t *move_node_to_end(ll_list_t *list, void *key)
{
	// Declare a node to iterate through the list
	ll_node_t *node;

	// Find the node with the given key and remove it from the list
	for (node = list->head; node; node = node->next) {
		if (strcmp(((info_t *)node->data)->key, key) == 0) {
			ll_node_t *prev = node->prev;
			ll_node_t *next = node->next;
			if (prev) {
				prev->next = next;
			} else {
				list->head = next;
			}
			if (next) {
				next->prev = prev;
			} else {
				list->tail = prev;
			}
			list->size--;

			break;
		}
	}

	// If the node was not found, return NULL
	if (!node)
		return NULL;

	// Add the node to the end of the list
	ll_node_t *new_node = ll_add_nth_node_info(list, list->size, node->data);

	// Free the old node
	free(((info_t *)node->data)->key);
	free(((info_t *)node->data)->value);
	free(node->data);
	free(node);

	// Return the new node
	return new_node;
}

void ll_free_info(ll_list_t **pp_list)
{
	// Almost identical to ll_free, but with a different data type
	// and a different way of freeing the data

	ll_node_t *node;

	if (!pp_list || !*pp_list)
		return;

	while ((*pp_list)->size) {
		node = ll_remove_nth_node(*pp_list, 0);

		// Free the key, value, data, and node
		free(((info_t *)node->data)->key);
		free(((info_t *)node->data)->value);
		free(node->data);
		free(node);
	}

	free(*pp_list);
	*pp_list = NULL;
}

ll_node_t *ll_get_node(ll_list_t *list, void *key)
{
	// Declare a node to iterate through the list
	ll_node_t *node;

	// Find the node with the given key
	for (node = list->head; node; node = node->next) {
		if (strcmp(((info_t *)node->data)->key, key) == 0) {
			return node;
		}
	}

	// If the node was not found, return NULL
	return NULL;
}
