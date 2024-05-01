
/*
 * Copyright (c) 2024, <>
 */

#include "server.h"

static response *server_edit_document(server *s, char *doc_name,
									  char *doc_content)
{
	// Check if the server, document name and content are valid
	if (!s || !doc_name || !doc_content)
		return NULL;

	// Allocate memory for the response from the server
	response *res = calloc(1, sizeof(*res));
	DIE(!res, "calloc response");

	// Allocate memory for the response
	res->server_response = malloc(MAX_RESPONSE_LENGTH);
	DIE(!res->server_response, "malloc response");

	// Allocate memory for the log message
	res->server_log = malloc(MAX_LOG_LENGTH);
	DIE(!res->server_log, "malloc log");

	// Set the server's id
	res->server_id = s->id;

	// Check if the document is in the cache
	if (ht_has_key(s->cache->ht, doc_name)) {
		// Get the corresponding response and log messages
		sprintf(res->server_response, MSG_B, doc_name);
		sprintf(res->server_log, LOG_HIT, doc_name);

		// Update the document's content in the cache and the database
		lru_cache_put(s->cache, doc_name, doc_content, NULL);
		ht_put(s->db, doc_name, strlen(doc_name) + 1, doc_content,
			   strlen(doc_content) + 1);

		// Return the response
		return res;
	}

	// Save the evicted key and check if the cache is full
	void *evicted_key = NULL;
	bool full = lru_cache_is_full(s->cache);

	// Check if the document is in the database and get a corresponding response
	if (ht_has_key(s->db, doc_name)) {
		sprintf(res->server_response, MSG_B, doc_name);
	} else {
		sprintf(res->server_response, MSG_C, doc_name);
	}

	// Update the document's content in the cache and the database
	lru_cache_put(s->cache, doc_name, doc_content, &evicted_key);
	ht_put(s->db, doc_name, strlen(doc_name) + 1, doc_content,
		   strlen(doc_content) + 1);

	// Get the corresponding log message
	if (full) {
		sprintf(res->server_log, LOG_EVICT, doc_name, (char *)evicted_key);
	} else {
		sprintf(res->server_log, LOG_MISS, doc_name);
	}

	// Free the evicted key and return the response
	free(evicted_key);
	return res;
}

static response *server_get_document(server *s, char *doc_name)
{
	// Check if the server and document name are valid
	if (!s || !doc_name)
		return NULL;

	// Allocate memory for the response from the server
	response *res = calloc(1, sizeof(*res));
	DIE(!res, "calloc response");

	// Allocate memory for the response
	res->server_response = malloc(MAX_RESPONSE_LENGTH);
	DIE(!res->server_response, "malloc response");

	// Allocate memory for the log message
	res->server_log = malloc(MAX_LOG_LENGTH);
	DIE(!res->server_log, "malloc log");

	// Set the server's id
	res->server_id = s->id;

	// Check if the document is in the cache
	if (ht_has_key(s->cache->ht, doc_name)) {
		// Get the document's content from the cache
		char *doc_content = lru_cache_get(s->cache, doc_name);

		// Get the corresponding response and log messages
		strcpy(res->server_response, doc_content);
		sprintf(res->server_log, LOG_HIT, doc_name);

		// Free the document's content and return the response
		free(doc_content);
		return res;
	}

	// Check if the document is in the database
	if (!ht_has_key(s->db, doc_name)) {
		// Get the corresponding response and log messages
		free(res->server_response);
		res->server_response = NULL;
		sprintf(res->server_log, LOG_FAULT, doc_name);

		// Return the response
		return res;
	}

	// Save the evicted key and check if the cache is full
	void *evicted_key = NULL;
	bool full = lru_cache_is_full(s->cache);

	// Get the document's content from the database
	char *doc_content = strdup(ht_get(s->db, doc_name));

	// Update the document's content in the cache
	lru_cache_put(s->cache, doc_name, doc_content, &evicted_key);

	// Get the corresponding response
	strcpy(res->server_response, doc_content);

	// Get the corresponding log message
	if (full) {
		sprintf(res->server_log, LOG_EVICT, doc_name, (char *)evicted_key);
	} else {
		sprintf(res->server_log, LOG_MISS, doc_name);
	}

	// Free the document's content and evicted key
	free(doc_content);
	free(evicted_key);

	// Return the response
	return res;
}

server *init_server(unsigned int cache_size)
{
	// Check if the cache size is valid
	if (cache_size == 0)
		return NULL;

	// Allocate memory for the server
	server *s = calloc(1, sizeof(*s));
	DIE(!s, "calloc server");

	// Initialize the server's fields
	s->cache = init_lru_cache(cache_size);
	s->tasks = q_create(sizeof(request), TASK_QUEUE_SIZE);
	s->db = ht_create(cache_size * 2, hash_string, compare_strings);

	// Return the server
	return s;
}

void execute_queue(server *s)
{
	// Check if the server is valid
	if (!s)
		return;

	// Execute all the tasks in the queue
	while (!q_is_empty(s->tasks)) {
		// Get the first task from the queue
		request *task = (request *)q_front(s->tasks);

		// Execute the task and get the corresponding response
		response *res =
			server_edit_document(s, task->doc_name, task->doc_content);

		// Print the response
		PRINT_RESPONSE(res);

		// Remove the task from the queue
		q_dequeue_request(s->tasks);
	}
}

response *server_handle_request(server *s, request *req)
{
	// Check if the server and request are valid
	if (!s || !req || (req->type != GET_DOCUMENT && req->type != EDIT_DOCUMENT))
		return NULL;

	// Handle the edit document request
	if (req->type == EDIT_DOCUMENT) {
		// Add the request to the server's task queue
		q_enqueue_request(s->tasks, (void *)req);

		// Allocate memory for the response from the server
		response *res = calloc(1, sizeof(*res));
		DIE(!res, "calloc response");

		// Allocate memory for the response
		res->server_response = malloc(MAX_RESPONSE_LENGTH);
		DIE(!res->server_response, "malloc response");

		// Allocate memory for the log message
		res->server_log = malloc(MAX_LOG_LENGTH);
		DIE(!res->server_log, "malloc log");

		// Set the server's id
		res->server_id = s->id;

		// Get the corresponding response and log messages
		sprintf(res->server_response, MSG_A, "EDIT", req->doc_name);
		sprintf(res->server_log, LOG_LAZY_EXEC, s->tasks->size);

		// Return the response
		return res;
	}

	// Handle the get document request

	// Execute all the tasks in the queue
	execute_queue(s);

	// Edit the document and return the response
	return server_get_document(s, req->doc_name);
}

void free_server(server **s)
{
	// Check if the server is valid
	if (!s || !(*s))
		return;

	// Free the server's fields
	free_lru_cache(&(*s)->cache);
	q_free_request((*s)->tasks);
	ht_free((*s)->db);

	// Free the server
	free(*s);
	*s = NULL;
}