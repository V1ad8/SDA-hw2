/*
 * Copyright (c) 2024, <>
 */

#ifndef SERVER_H
#define SERVER_H

#include "constants.h"
#include "lru_cache.h"
#include "add/queue.h"

#define TASK_QUEUE_SIZE 1000
#define MAX_LOG_LENGTH 1000
#define MAX_RESPONSE_LENGTH 4096

typedef struct server {
	// Server ID
	int id;

	// Cache for the server
	lru_cache *cache;

	// Task queue for the server
	queue_t *tasks;

	// Database for the server
	hashtable_t *db;
} server;

typedef struct request {
	// The type of the request: EDIT or GET
	request_type type;

	// The name of the document
	char *doc_name;

	// The content of the document
	char *doc_content;
} request;

typedef struct response {
	// The log message
	char *server_log;

	// The response message
	char *server_response;

	// The server ID
	int server_id;
} response;

/**
 * @brief Initializes a server with the given cache size.
 * 
 * @param cache_size: The size of the cache.
 * 
 * @return server*: The initialized server.
 */
server *init_server(unsigned int cache_size);

/**
 * @brief Executes all the tasks in the server's queue.
 * 
 * @param s: The server.
 */
void execute_queue(server *s);

/**
 * @brief Should deallocate completely the memory used by server,
 *     taking care of deallocating the elements in the queue, if any,
 *     without executing the tasks
 */
void free_server(server **s);

/**
 * server_handle_request() - Receives a request from the load balancer
 *      and processes it according to the request type
 * 
 * @param s: Server which processes the request.
 * @param req: Request to be processed.
 * 
 * @return response*: Response of the requested operation, which will
 *      then be printed in main.
 * 
 * @brief Based on the type of request, should call the appropriate
 *     solver, and should execute the tasks from queue if needed (in
 *     this case, after executing each task, PRINT_RESPONSE should
 *     be called).
 */
response *server_handle_request(server *s, request *req);

#endif /* SERVER_H */
