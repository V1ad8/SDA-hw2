#ifndef ADD_LINKED_LIST_H
#define ADD_LINKED_LIST_H

typedef struct ll_node_t {
	void *data;
	struct ll_node_t *next;
} ll_node_t;

typedef struct ll_list_t {
	ll_node_t *head;
	unsigned int data_size;
	unsigned int size;
} ll_list_t;

ll_list_t *ll_create(unsigned int data_size);

ll_node_t *ll_get_nth_node(ll_list_t *list, unsigned int n);

ll_node_t *ll_create_node(const void *new_data, unsigned int data_size);

void ll_add_nth_node(ll_list_t *list, unsigned int n, const void *new_data);

ll_node_t *ll_remove_nth_node(ll_list_t *list, unsigned int n);

unsigned int ll_get_size(ll_list_t *list);

void ll_free(ll_list_t **pp_list);

#endif /* ADD_LINKED_LIST_H */