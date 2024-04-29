/*
 * Copyright (c) 2024, <>
 */

#include "lru_cache.h"

lru_cache *init_lru_cache(unsigned int cache_capacity)
{
	// Check if the cache capacity is valid
	if (cache_capacity == 0)
		return NULL;

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
	// Check if the cache is valid
	if (!cache)
		return false;

	// Check if the cache is full
	return cache->ht->hmax == cache->ht->size;
}

void free_lru_cache(lru_cache **cache)
{
	// Check if the cache is valid
	if (!*cache)
		return;

	// Free the cache's hashtable and linked list
	ht_free((*cache)->ht);
	ll_free_info(&(*cache)->order);

	// Free the cache
	free(*cache);
	*cache = NULL;
}

bool lru_cache_put(lru_cache *cache, void *key, void *value, void **evicted_key)
{
	// Check if the cache, key and value are valid
	if (!cache || !key || !value)
		return false;

	// Update existing key's value and move it to the end of the linked list
	if (ht_has_key(cache->ht, key)) {
		ll_node_t *node = move_node_to_end(cache->order, key);

		// Update the value for the key
		free(((info_t *)node->data)->value);
		((info_t *)node->data)->value = strdup(value);

		// Modify the key-value pair in the hashtable
		ht_put(cache->ht, key, sizeof(key), node, sizeof(node));

		// The key already exists in the cache
		return false;
	} else if (lru_cache_is_full(cache)) {
		// Evict the least recently used key
		ll_node_t *node = ll_remove_nth_node(cache->order, 0);

		// Store the evicted key
		*evicted_key = strdup(((info_t *)node->data)->key);
		ht_remove_entry(cache->ht, *evicted_key);

		// Free the memory allocated for the evicted key and value
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
	// Check if the cache and key are valid
	if (!cache || !key)
		return NULL;

	// Check if the key exists in the cache
	if (!ht_has_key(cache->ht, key))
		return NULL;

	// Get the node corresponding to the key from the hashtable
	ll_node_t *node = ht_get(cache->ht, key);

	// Duplicate the value associated with the key
	char *value = strdup(((info_t *)node->data)->value);

	// Move the node to the end of the linked list to mark it as most recently used
	ll_node_t *new_node = move_node_to_end(cache->order, key);

	// Update the key-value pair in the hashtable
	ht_remove_entry(cache->ht, key);
	ht_put(cache->ht, key, strlen(key) + 1, new_node, sizeof(new_node));

	// Return the value associated with the key
	return value;
}

void lru_cache_remove(lru_cache *cache, void *key)
{
	// Check if the cache and key are valid
	if (!cache || !key)
		return;

	// Check if the key exists in the cache
	if (!ht_has_key(cache->ht, key))
		return;

	// Get the node corresponding to the key from the hashtable
	ll_node_t *ht_node = ht_get(cache->ht, key);

	// Remove the node from the linked list and free the memory
	ll_node_t *node = ll_remove_node(cache->order, ht_node);
	free(((info_t *)node->data)->key);
	free(((info_t *)node->data)->value);
	free(node->data);
	free(node);

	// Remove the key-value pair from the hashtable
	ht_remove_entry(cache->ht, key);
}
