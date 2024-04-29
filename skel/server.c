
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
	response *res = calloc(1, sizeof(*res));
	DIE(!res, "calloc response");

	res->server_response = malloc(MAX_RESPONSE_LENGTH);
	DIE(!res->server_response, "malloc response");

	res->server_log = malloc(MAX_LOG_LENGTH);
	DIE(!res->server_log, "malloc log");

	res->server_id = s->id;

	if (ht_has_key(s->cache->ht, doc_name)) {
		sprintf(res->server_response, MSG_B, doc_name);
		sprintf(res->server_log, LOG_HIT, doc_name);

		lru_cache_put(s->cache, doc_name, doc_content, NULL);
		ht_put(s->db, doc_name, strlen(doc_name) + 1, doc_content,
			   strlen(doc_content) + 1);

		return res;
	}

	void *evicted_key = NULL;
	bool full = lru_cache_is_full(s->cache);

	if (ht_has_key(s->db, doc_name)) {
		sprintf(res->server_response, MSG_B, doc_name);
	} else {
		sprintf(res->server_response, MSG_C, doc_name);
	}

	lru_cache_put(s->cache, doc_name, doc_content, &evicted_key);
	ht_put(s->db, doc_name, strlen(doc_name) + 1, doc_content,
		   strlen(doc_content) + 1);

	if (full) {
		sprintf(res->server_log, LOG_EVICT, doc_name, (char *)evicted_key);
	} else {
		sprintf(res->server_log, LOG_MISS, doc_name);
	}

	free(evicted_key);
	return res;
}

static response *server_get_document(server *s, char *doc_name)
{
	response *res = calloc(1, sizeof(*res));
	DIE(!res, "calloc response");

	res->server_response = malloc(MAX_RESPONSE_LENGTH);
	DIE(!res->server_response, "malloc response");

	res->server_log = malloc(MAX_LOG_LENGTH);
	DIE(!res->server_log, "malloc log");

	res->server_id = s->id;

	if (ht_has_key(s->cache->ht, doc_name)) {
		char *doc_content = lru_cache_get(s->cache, doc_name);

		strcpy(res->server_response, doc_content);
		sprintf(res->server_log, LOG_HIT, doc_name);

		free(doc_content);
		return res;
	}

	if (!ht_has_key(s->db, doc_name)) {
		free(res->server_response);
		res->server_response = NULL;

		sprintf(res->server_log, LOG_FAULT, doc_name);

		return res;
	}

	void *evicted_key = NULL;
	bool full = lru_cache_is_full(s->cache);

	char *doc_content = strdup(ht_get(s->db, doc_name));

	strcpy(res->server_response, doc_content);
	lru_cache_put(s->cache, doc_name, doc_content, &evicted_key);

	free(doc_content);

	if (full) {
		sprintf(res->server_log, LOG_EVICT, doc_name, (char *)evicted_key);
	} else {
		sprintf(res->server_log, LOG_MISS, doc_name);
	}

	free(evicted_key);
	return res;
}

server *init_server(unsigned int cache_size)
{
	server *s = calloc(1, sizeof(*s));
	DIE(!s, "calloc server");

	s->cache = init_lru_cache(cache_size);
	s->tasks = q_create(sizeof(request), TASK_QUEUE_SIZE);
	s->db = ht_create(
		cache_size * 2, hash_string,
		compare_strings); // Allocate memory for the hashtable with the correct size

	return s;
}

response *server_handle_request(server *s, request *req)
{
	if (!s || !req || (req->type != GET_DOCUMENT && req->type != EDIT_DOCUMENT))
		return NULL;

	if (req->type == EDIT_DOCUMENT) {
		q_enqueue_request(s->tasks, (void *)req);

		response *res = calloc(1, sizeof(*res));
		DIE(!res, "calloc response");

		res->server_response = malloc(MAX_RESPONSE_LENGTH);
		DIE(!res->server_response, "malloc response");

		res->server_log = malloc(MAX_LOG_LENGTH);
		DIE(!res->server_log, "malloc log");

		res->server_id = s->id;

		sprintf(res->server_response, MSG_A, "EDIT", req->doc_name);
		sprintf(res->server_log, LOG_LAZY_EXEC, s->tasks->size);

		return res;
	}

	while (!q_is_empty(s->tasks)) {
		request *task = (request *)q_front(s->tasks);

		response *res =
			server_edit_document(s, task->doc_name, task->doc_content);

		PRINT_RESPONSE(res);

		q_dequeue_request(s->tasks);
	}

	return server_get_document(s, req->doc_name);
}

void free_server(server **s)
{
	if (!s || !(*s))
		return;

	free_lru_cache(&(*s)->cache);
	q_free_request((*s)->tasks);
	ht_free((*s)->db);

	free(*s);
	*s = NULL;
}