#include "queue.h"
#include "../server.h"

queue_t *q_create(unsigned int data_size, unsigned int max_size)
{
	queue_t *q = calloc(1, sizeof(*q));
	DIE(!q, "calloc queue failed");

	q->data_size = data_size;
	q->max_size = max_size;

	q->buff = malloc(max_size * sizeof(*q->buff));
	DIE(!q->buff, "malloc buffer failed");

	return q;
}

unsigned int q_get_size(queue_t *q)
{
	return !q ? 0 : q->size;
}

unsigned int q_is_empty(queue_t *q)
{
	return !q ? 1 : !q->size;
}

void *q_front(queue_t *q)
{
	if (!q || !q->size)
		return NULL;

	return q->buff[q->read_idx];
}

int q_dequeue(queue_t *q)
{
	if (!q || !q->size)
		return 0;

	free(q->buff[q->read_idx]);

	q->read_idx = (q->read_idx + 1) % q->max_size;
	--q->size;
	return 1;
}

int q_enqueue(queue_t *q, void *new_data)
{
	void *data;
	if (!q || q->size == q->max_size)
		return 0;

	data = malloc(q->data_size);
	DIE(!data, "malloc data failed");
	memcpy(data, new_data, q->data_size);

	q->buff[q->write_idx] = data;
	q->write_idx = (q->write_idx + 1) % q->max_size;
	++q->size;

	return 1;
}

void q_clear(queue_t *q)
{
	unsigned int i;
	if (!q || !q->size)
		return;

	for (i = q->read_idx; i != q->write_idx; i = (i + 1) % q->max_size)
		free(q->buff[i]);

	q->read_idx = 0;
	q->write_idx = 0;
	q->size = 0;
}

void q_free(queue_t *q)
{
	if (!q)
		return;

	q_clear(q);
	free(q->buff);
	free(q);
}

void q_free_request(queue_t *q)
{
	if (!q)
		return;

	unsigned int i;

	for (i = q->read_idx; i != q->write_idx; i = (i + 1) % q->max_size) {
		free(((request *)q->buff[i])->doc_name);
		free(((request *)q->buff[i])->doc_content);
		free(q->buff[i]);
	}

	q->read_idx = 0;
	q->write_idx = 0;
	q->size = 0;

	free(q->buff);
	free(q);
}
