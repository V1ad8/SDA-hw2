/*
 * Copyright (c) 2024, <>
 */

#include <stdio.h>
#include <string.h>
#include "lru_cache.h"
#include "utils.h"

lru_cache *init_lru_cache(unsigned int cache_capacity)
{
	// Declare and allocate memory for the cache
	lru_cache *cache = malloc(sizeof(*cache));
	DIE(!cache, "malloc cache");

	// Initialize the cache's hashtable
	cache->ht = ht_create(cache_capacity, hash_string, compare_strings);

	// Initialize the cache's linked list
	cache->order = ll_create(sizeof(void *));

	// Return the cache
	return cache;
}

bool lru_cache_is_full(lru_cache *cache)
{
	// Check if the cache is full
	return cache->ht->hmax == cache->ht->size;
}

void free_lru_cache(lru_cache **cache)
{
	// Free the cache's hashtable and linked list
	ht_free((*cache)->ht);
	ll_free_info(&(*cache)->order);

	// Free the cache
	free(*cache);
	*cache = NULL;
}

bool lru_cache_put(lru_cache *cache, void *key, void *value, void **evicted_key)
{
	if (ht_has_key(cache->ht, key)) {
		// Update existing key's value and move it to the end of the linked list
		ll_node_t *node = move_node_to_end(cache->order, key);

		free(((info_t *)node->data)->value);
		((info_t *)node->data)->value = strdup(value);

		ht_put(cache->ht, key, sizeof(key), node, sizeof(node));

		return false;
	} else if (lru_cache_is_full(cache)) {
		// Evict the least recently used key
		ll_node_t *node = ll_remove_nth_node(cache->order, 0);

		*evicted_key = strdup(((info_t *)node->data)->key);
		ht_remove_entry(cache->ht, *evicted_key);

		free(((info_t *)node->data)->key);
		free(((info_t *)node->data)->value);
		free(node->data);
		free(node);
	}

	// Create a new info_t struct to store the key-value pair
	info_t *info = malloc(sizeof(info_t));
	info->key = strdup(key);
	info->value = strdup(value);

	// Add the new node to the end of the linked list
	ll_node_t *node =
		ll_add_nth_node_info(cache->order, cache->order->size, info);

	// Free the temporary key and value strings
	free(info->key);
	free(info->value);
	free(info);

	// Add the key-value pair to the hashtable
	ht_put(cache->ht, key, strlen(key) + 1, node, sizeof(node));
	return true;
}

void *lru_cache_get(lru_cache *cache, void *key)
{
	if (!ht_has_key(cache->ht, key))
		return NULL;

	// Get the node corresponding to the key from the hashtable
	ll_node_t *node = ht_get(cache->ht, key);
	char *value = strdup(((info_t *)node->data)->value);

	// Move the node to the end of the linked list to mark it as most recently used
	ht_remove_entry(cache->ht, key);
	ll_node_t *new_node = move_node_to_end(cache->order, key);
	ht_put(cache->ht, key, strlen(key) + 1, new_node, sizeof(new_node));

	return value;
}

void lru_cache_remove(lru_cache *cache, void *key)
{
	if (!ht_has_key(cache->ht, key))
		return;

	// Get the node corresponding to the key from the hashtable
	ll_node_t *node = ht_get(cache->ht, key);

	// Remove the node from the linked list and the key from the hashtable
	ll_remove_node(cache->order, node);
	ht_remove_entry(cache->ht, key);
}
