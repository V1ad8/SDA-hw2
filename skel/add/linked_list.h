// add a guard to this header file

#ifndef ADD_LINKED_LIST_H
#define ADD_LINKED_LIST_H

typedef struct linked_list_t linked_list_t;
typedef struct ll_node_t ll_node_t;

typedef struct ll_node_t {
	void *data;
	ll_node_t *next;
} ll_node_t;

typedef struct linked_list_t {
    ll_node_t *head;
    unsigned int data_size;
    unsigned int size;
} linked_list_t;

linked_list_t *ll_create(unsigned int data_size);

ll_node_t *get_nth_node(linked_list_t *list, unsigned int n);

void ll_add_nth_node(linked_list_t *list, unsigned int n, const void *new_data);

ll_node_t *ll_remove_nth_node(linked_list_t *list, unsigned int n);

unsigned int ll_get_size(linked_list_t *list);

void ll_free(linked_list_t **pp_list);

# endif /* ADD_LINKED_LIST_H */