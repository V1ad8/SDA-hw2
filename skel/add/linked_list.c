#include "linked_list.h"

ll_list_t *ll_create(unsigned int data_size)
{
	ll_list_t *ll = calloc(1, sizeof(*ll));
	DIE(!ll, "calloc list");

	ll->data_size = data_size;

	return ll;
}

ll_node_t *ll_get_nth_node(ll_list_t *list, unsigned int n)
{
	unsigned int len = list->size - 1;
	unsigned int i;
	ll_node_t *node;

	if (n > len)
		return NULL;

	if (n <= len / 2) {
		node = list->head;
		for (i = 0; i < n; ++i)
			node = node->next;
	} else {
		node = list->tail;
		for (i = len; i > n; --i)
			node = node->prev;
	}

	return node;
}

static ll_node_t *ll_create_node(const void *new_data, unsigned int data_size)
{
	ll_node_t *node = calloc(1, sizeof(*node));
	DIE(!node, "calloc node");

	node->data = malloc(data_size);
	DIE(!node->data, "malloc data");

	memcpy(node->data, new_data, data_size);

	return node;
}

ll_node_t *ll_add_nth_node(ll_list_t *list, unsigned int n,
						   const void *new_data)
{
	ll_node_t *new_node, *prev_node;

	if (!list)
		return NULL;

	if (n > list->size)
		return NULL;

	new_node = ll_create_node(new_data, list->data_size);

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

ll_node_t *ll_remove_nth_node(ll_list_t *list, unsigned int n)
{
	ll_node_t *prev_node, *removed_node;

	if (!list || !list->size)
		return NULL;

	if (n >= list->size)
		return NULL;

	if (n == 0) {
		removed_node = list->head;
		list->head = removed_node->next;
		if (list->head)
			list->head->prev = NULL;
		if (removed_node == list->tail)
			list->tail = NULL;
	} else if (n == list->size - 1) {
		removed_node = list->tail;
		list->tail = removed_node->prev;
		list->tail->next = NULL;
	} else {
		prev_node = ll_get_nth_node(list, n - 1);
		removed_node = prev_node->next;
		prev_node->next = removed_node->next;
		prev_node->next->prev = prev_node;
	}

	--list->size;

	return removed_node;
}

ll_node_t *ll_remove_node(ll_list_t *list, ll_node_t *node)
{
	if (!list || !node)
		return NULL;

	if (node == list->head) {
		list->head = node->next;
		if (list->head)
			list->head->prev = NULL;
	} else {
		if (node->prev)
			node->prev->next = node->next;
	}

	if (node == list->tail) {
		list->tail = node->prev;
		if (list->tail)
			list->tail->next = NULL;
	} else {
		if (node->next)
			node->next->prev = node->prev;
	}

	list->size--;

	return node;
}

unsigned int ll_get_size(ll_list_t *list)
{
	return !list ? 0 : list->size;
}

void ll_free(ll_list_t **pp_list)
{
	ll_node_t *node;

	if (!pp_list || !*pp_list)
		return;

	while ((*pp_list)->size) {
		node = ll_remove_nth_node(*pp_list, 0);
		free(node->data);
		free(node);
	}

	free(*pp_list);
	*pp_list = NULL;
}
