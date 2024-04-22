#ifndef ADD_DOUBLY_LINKED_LIST_H
#define ADD_DOUBLY_LINKED_LIST_H

typedef struct dll_node_t {
	void *data;
	struct  dll_node_t *next, *prev;
} dll_node_t;

typedef struct dll_list_t {
	dll_node_t *head, *tail;
	unsigned int data_size;
	unsigned int size;
} dll_list_t;

dll_list_t *dll_create(unsigned int data_size);

dll_node_t *dll_get_nth_node(dll_list_t *list, unsigned int n);

dll_node_t *dll_create_node(const void *new_data, unsigned int data_size);

dll_node_t *dll_add_nth_node(dll_list_t *list, unsigned int n,
							 const void *new_data);

dll_node_t *dll_remove_nth_node(dll_list_t *list, unsigned int n);

dll_node_t *move_node_to_end(dll_list_t *list, void *data);

dll_node_t *dll_remove_node(dll_list_t *list, dll_node_t *node);

unsigned int dll_get_size(dll_list_t *list);

void dll_free(dll_list_t **pp_list);

#endif /* ADD_DOUBLY_LINKED_LIST_H */