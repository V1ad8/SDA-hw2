#ifndef ADD_QUEUE_H
#define ADD_QUEUE_H

#include "../utils.h"

typedef struct queue_t {
	unsigned int max_size;
	unsigned int size;
	unsigned int data_size;
	unsigned int read_idx;
	unsigned int write_idx;
	void **buff;
} queue_t;

queue_t *q_create(unsigned int data_size, unsigned int max_size);

unsigned int q_get_size(queue_t *q);

unsigned int q_is_empty(queue_t *q);

void *q_front(queue_t *q);

int q_dequeue(queue_t *q);

int q_enqueue(queue_t *q, void *new_data);

int q_dequeue_request(queue_t *q);

int q_enqueue_request(queue_t *q, void *req);

void q_clear(queue_t *q);

void q_free(queue_t *q);

void q_free_request(queue_t *q);

#endif /* ADD_QUEUE_H */