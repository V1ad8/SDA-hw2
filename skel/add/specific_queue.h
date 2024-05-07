/*
 * Copyright (c) 2024, <Ungureanu Vlad-Marin> <<2004uvm@gmail.com>>
 */

# ifndef SPECIFIC_QUEUE_H
# define SPECIFIC_QUEUE_H

#include "queue.h"
#include "../server.h"

int q_dequeue_request(queue_t *q);

int q_enqueue_request(queue_t *q, void *req);

# endif /* SPECIFIC_QUEUE_H */
