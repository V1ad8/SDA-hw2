/*
 * Copyright (c) 2024, <Ungureanu Vlad-Marin> <<2004uvm@gmail.com>>
 */

#include "specific_queue.h"

int q_dequeue_request(queue_t *q)
{
	// Almost identical to q_dequeue, but with a different data type
	// and a different way of freeing the data

	if (!q || !q->size)
		return 0;

	// Free the doc_name, doc_content, and request
	free(((request *)q->buff[q->read_idx])->doc_name);
	free(((request *)q->buff[q->read_idx])->doc_content);
	free(q->buff[q->read_idx]);

	q->read_idx = (q->read_idx + 1) % q->max_size;
	--q->size;
	return 1;
}

int q_enqueue_request(queue_t *q, void *req)
{
	// Almost identical to q_enqueue, but with a different data type
	// and a different way of copying the data

	if (!q || q->size == q->max_size)
		return 0;

	// Allocate memory for the new request
	request *new_req = malloc(sizeof(*new_req));
	DIE(!new_req, "malloc request failed");

	// Copy the doc_name
	new_req->doc_name = strdup(((request *)req)->doc_name);
	DIE(!new_req->doc_name, "strdup doc_name failed");

	// Copy the doc_content
	new_req->doc_content = strdup(((request *)req)->doc_content);
	DIE(!new_req->doc_content, "strdup doc_content failed");

	// Copy the type
	new_req->type = ((request *)req)->type;

	q->buff[q->write_idx] = (void *)new_req;
	q->write_idx = (q->write_idx + 1) % q->max_size;
	++q->size;

	return 1;
}
