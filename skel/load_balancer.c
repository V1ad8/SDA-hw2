/*
 * Copyright (c) 2024, <>
 */

#include "load_balancer.h"
#include "server.h"

unsigned int get_server(load_balancer *main, unsigned int hash)
{
	unsigned int slot = 0;

	for (ll_node_t *curr = main->servers->head; curr; curr = curr->next) {
		if (hash <= main->hash_function_servers(&((server *)curr->data)->id))
			return slot;

		slot++;
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
	unsigned int slot = get_server(main, s_hash);

	if (ll_get_size(main->servers))
		if (s_hash > main->hash_function_servers(
						 &((server *)main->servers->tail->data)->id))
			slot = ll_get_size(main->servers) - slot;

	server *s = init_server(cache_size);
	s->id = server_id;

	if (!ll_get_size(main->servers)) {
		ll_add_nth_node(main->servers, 0, s);

		free(s);
		return;
	}

	unsigned int next_slot = slot % ll_get_size(main->servers);
	server *next_s = ll_get_nth_node(main->servers, next_slot)->data;

	execute_queue(next_s);

	for (unsigned int b = 0; b < next_s->db->hmax; b++) {
		ll_node_t *curr = next_s->db->buckets[b]->head;

		while (curr) {
			unsigned int hash =
				main->hash_function_docs(((info_t *)curr->data)->key);

			if (hash < s_hash) {
				ht_put(s->db, ((info_t *)curr->data)->key,
					   strlen(((info_t *)curr->data)->key) + 1,
					   ((info_t *)curr->data)->value,
					   strlen(((info_t *)curr->data)->value) + 1);

				// lru_cache_remove(next_s->cache, ((info_t *)curr->data)->key);
				ht_remove_entry(next_s->db, ((info_t *)curr->data)->key);

				b--;
				break;
			}

			curr = curr->next;
		}
	}

	ll_add_nth_node(main->servers, slot, s);
	free(s);
}

void loader_remove_server(load_balancer *main, int server_id)
{
	unsigned int s_hash = main->hash_function_servers(&server_id);
	unsigned int slot = get_server(main, s_hash);
	unsigned int next_slot = (slot + 1) % ll_get_size(main->servers);

	server *s = ll_get_nth_node(main->servers, slot)->data;

	if (slot == ll_get_size(main->servers)) {
		free_server(&s);
		free(ll_remove_nth_node(main->servers, slot));

		return;
	}

	server *next_s = ll_get_nth_node(main->servers, next_slot)->data;

	execute_queue(s);

	for (unsigned int b = 0; b < s->db->hmax; b++) {
		for (ll_node_t *curr = s->db->buckets[b]->head; curr;
			 curr = curr->next) {
			ht_put(next_s->db, ((info_t *)curr->data)->key,
				   strlen(((info_t *)curr->data)->key) + 1,
				   ((info_t *)curr->data)->value,
				   strlen(((info_t *)curr->data)->value) + 1);
		}
	}

	free_server(&s);
	free(ll_remove_nth_node(main->servers, slot));
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
