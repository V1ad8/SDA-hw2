/*
 * Copyright (c) 2024, <>
 */

#include <stdio.h>
#include "server.h"
#include "lru_cache.h"

#include "utils.h"

static response *server_edit_document(server *s, char *doc_name,
									  char *doc_content)
{
	/* TODO */
	return NULL;
}

static response *server_get_document(server *s, char *doc_name)
{
	/* TODO */
	return NULL;
}

server *init_server(unsigned int cache_size)
{
	server *s = malloc(sizeof(server *));
	DIE(!s, "malloc server");

	s->cache = init_lru_cache(cache_size);
	s->task_queue = ll_create(sizeof(request *));
	s->server_id = 0;

	return s;
}

response *server_handle_request(server *s, request *req)
{
	/* TODO */
	return NULL;
}

void free_server(server **s)
{
	/* TODO */
}
