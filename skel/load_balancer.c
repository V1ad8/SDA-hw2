/*
 * Copyright (c) 2024, <>
 */

#include "load_balancer.h"
#include "server.h"

/*
 * get_server() - Get the server that should handle the request.
 * 
 * @main: The main load balancer.
 * @hash: The hash of the document.
 * 
 * @return unsigned int - The slot where the server is placed.
 * 
 * @brief The function will return the slot where the server is placed
 * based on the hash of the document.
*/
static unsigned int get_server(load_balancer *main, unsigned int hash)
{
	// Initialize slot to 0
	unsigned int slot = 0;

	// Look for the server with the hash bigger than the given hash
	for (ll_node_t *curr = main->servers->head; curr; curr = curr->next) {
		if (hash <= main->hash_function_servers(&((server *)curr->data)->id))
			return slot;

		slot++;
	}

	// If no server is found, return the first slot
	return 0;
}

load_balancer *init_load_balancer(bool enable_vnodes)
{
	// Allocate memory for the main load balancer
	load_balancer *main = calloc(1, sizeof(*main));
	DIE(!main, "calloc main");

	// Initialize the hash functions
	main->hash_function_servers = hash_uint;
	main->hash_function_docs = hash_string;

	// Initialize the servers list
	main->servers = ll_create(sizeof(server));
	DIE(!main->servers, "ll_create");

	// Set vnodes
	main->enable_vnodes = enable_vnodes;

	// Return the main load balancer
	return main;
}

void loader_add_server(load_balancer *main, int server_id, int cache_size)
{
	// Get the hash of the server and the slot where it should be placed
	unsigned int s_hash = main->hash_function_servers(&server_id);
	unsigned int slot = get_server(main, s_hash);

	// Initialize the server and set its id
	server *s = init_server(cache_size);
	s->id = server_id;

	// If the server list is empty, add the server at the beginning
	if (!ll_get_size(main->servers)) {
		// Add the server to the list
		ll_add_nth_node(main->servers, 0, s);

		// Free the server and return
		free(s);
		return;
	}

	// Check if the server should be placed at the end of the list or at th beginning; only for servers placed between the last one and the first one
	if (s_hash >
		main->hash_function_servers(&((server *)main->servers->tail->data)->id))
		slot = ll_get_size(main->servers) - slot;

	// Get the next server
	unsigned int next_slot = slot % ll_get_size(main->servers);
	server *next_s = ll_get_nth_node(main->servers, next_slot)->data;

	// Execute the tasks in the queue of the next server
	execute_queue(next_s);

	// Iterate through the keys of the next server and move them to the current server
	for (unsigned int b = 0; b < next_s->db->hmax; b++) {
		ll_node_t *curr = next_s->db->buckets[b]->head;

		while (curr) {
			// Get the hash of the key
			unsigned int hash =
				main->hash_function_docs(((info_t *)curr->data)->key);

			// Check if the key should be moved to the current server
			if (hash < s_hash) {
				// Add the key to the current server
				ht_put(s->db, ((info_t *)curr->data)->key,
					   strlen(((info_t *)curr->data)->key) + 1,
					   ((info_t *)curr->data)->value,
					   strlen(((info_t *)curr->data)->value) + 1);

				// Remove the key from the next server's cache and database
				lru_cache_remove(next_s->cache, ((info_t *)curr->data)->key);
				ht_remove_entry(next_s->db, ((info_t *)curr->data)->key);

				// Start from the beginning of the list again
				curr = next_s->db->buckets[b]->head;
				break;
			}

			// Move to the next key
			curr = curr->next;
		}
	}

	// Add the server to the list and free the server
	ll_add_nth_node(main->servers, slot, s);
	free(s);
}

void loader_remove_server(load_balancer *main, int server_id)
{
	// Get the hash of the server and the slot where it should be placed
	unsigned int s_hash = main->hash_function_servers(&server_id);
	unsigned int slot = get_server(main, s_hash);
	server *s = ll_get_nth_node(main->servers, slot)->data;

	// If there is only one server, remove it and return
	if (ll_get_size(main->servers) == 1) {
		// Free the server and the node from the list of servers
		free_server(&s);
		free(ll_remove_nth_node(main->servers, slot));

		return;
	}

	// Get the next server
	unsigned int next_slot = (slot + 1) % ll_get_size(main->servers);
	server *next_s = ll_get_nth_node(main->servers, next_slot)->data;

	// Execute the tasks in the queue of the current server
	execute_queue(s);

	// Iterate through the keys of the current server and move them to the next server
	for (unsigned int b = 0; b < s->db->hmax; b++) {
		for (ll_node_t *curr = s->db->buckets[b]->head; curr;
			 curr = curr->next) {
			ht_put(next_s->db, ((info_t *)curr->data)->key,
				   strlen(((info_t *)curr->data)->key) + 1,
				   ((info_t *)curr->data)->value,
				   strlen(((info_t *)curr->data)->value) + 1);
		}
	}

	// Free the server and the node from the list of servers
	free_server(&s);
	free(ll_remove_nth_node(main->servers, slot));
}

// Helper function to print the servers; used for debugging
void print_servers(load_balancer *main)
{
	for (ll_node_t *curr = main->servers->head; curr; curr = curr->next) {
		server *s = curr->data;

		printf("Server %d\t\t\t\t\t\t - %x\n", s->id,
			   main->hash_function_servers(&s->id));

		for (unsigned int b = 0; b < s->db->hmax; b++) {
			for (ll_node_t *curr = s->db->buckets[b]->head; curr;
				 curr = curr->next) {
				printf("\t%s - %x\n", ((info_t *)curr->data)->key,
					   main->hash_function_docs(((info_t *)curr->data)->key));
			}
		}
	}
}

response *loader_forward_request(load_balancer *main, request *req)
{
	// Get the hash of the document and the slot where it should be placed
	unsigned int hash = main->hash_function_docs(req->doc_name);
	unsigned int slot = get_server(main, hash);

	// Get the server that should handle the request
	server *s = ll_get_nth_node(main->servers, slot)->data;

	// Forward the request to the server
	return server_handle_request(s, req);
}

void free_load_balancer(load_balancer **main)
{
	// Free the servers from the list of servers
	for (ll_node_t *curr = (*main)->servers->head; curr; curr = curr->next) {
		free_server((server **)&curr->data);
	}

	// Free the list of servers
	ll_free(&(*main)->servers);

	// Free the main load balancer
	free(*main);
	*main = NULL;
}
