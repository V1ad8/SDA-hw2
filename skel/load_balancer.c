/*
 * Copyright (c) 2024, <>
 */

#include "load_balancer.h"
#include "server.h"

unsigned int get_server(load_balancer *main, unsigned int hash)
{
	unsigned int slot = 0;

	for (ll_node_t *curr = main->servers->head; curr; curr = curr->next) {
		if (hash > main->hash_function_servers(&((server *)curr->data)->id)) {
			slot++;
		} else {
			return slot;
		}
	}

	return 0;
}

load_balancer *init_load_balancer(bool enable_vnodes)
{
	load_balancer *main = calloc(1, sizeof(*main));
	DIE(!main, "calloc main");

	main->hash_function_servers = hash_uint;
	main->hash_function_docs = hash_string;

	main->servers = ll_create(sizeof(server));
	DIE(!main->servers, "ll_create");

	main->enable_vnodes = enable_vnodes;

	return main;
}

void loader_add_server(load_balancer *main, int server_id, int cache_size)
{
	unsigned int s_hash = main->hash_function_servers(&server_id);
	unsigned int slot = ll_get_size(main->servers) - get_server(main, s_hash);

	server *s = init_server(cache_size);
	s->id = server_id;

	if (!ll_get_size(main->servers)) {
		ll_add_nth_node(main->servers, 0, s);

		free(s);
		return;
	}

	unsigned int prev_slot = slot ? slot - 1 : ll_get_size(main->servers) - 1;
	server *prev_s = ll_get_nth_node(main->servers, prev_slot)->data;

	execute_queue(prev_s);

	for (unsigned int b = 0; b < prev_s->db->hmax; b++) {
		for (ll_node_t *curr = prev_s->db->buckets[b]->head; curr;
			 curr = curr->next) {
			unsigned int hash =
				main->hash_function_docs(((info_t *)curr->data)->key);

			if (hash > s_hash) {
				ht_put(s->db, ((info_t *)curr->data)->key,
					   strlen(((info_t *)curr->data)->key) + 1,
					   ((info_t *)curr->data)->value,
					   strlen(((info_t *)curr->data)->value) + 1);

				ht_remove_entry(prev_s->db, ((info_t *)curr->data)->key);
				lru_cache_remove(prev_s->cache, ((info_t *)curr->data)->key);
			}
		}
	}

	ll_add_nth_node(main->servers, slot, s);

	free(s);
}

void loader_remove_server(load_balancer *main, int server_id)
{
	/* TODO */
}

response *loader_forward_request(load_balancer *main, request *req)
{
	unsigned int hash = main->hash_function_docs(req->doc_name);

	unsigned int slot = get_server(main, hash);

	server *s = ll_get_nth_node(main->servers, slot)->data;

	return server_handle_request(s, req);
}

void free_load_balancer(load_balancer **main)
{
	for (ll_node_t *curr = (*main)->servers->head; curr; curr = curr->next) {
		free_server((server **)&curr->data);
	}

	ll_free(&(*main)->servers);

	free(*main);
	*main = NULL;
}
