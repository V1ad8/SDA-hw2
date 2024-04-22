/*
 * Copyright (c) 2024, <>
 */

#include <stdio.h>
#include <string.h>
#include "lru_cache.h"
#include "utils.h"

lru_cache *init_lru_cache(unsigned int cache_capacity)
{
	lru_cache *cache = malloc(sizeof(*cache));
	DIE(!cache, "malloc cache");

	cache->ht = ht_create(cache_capacity, hash_string, compare_strings);
	cache->ht->hmax = cache_capacity;
	cache->ht->size = 0;

	cache->order = ll_create(sizeof(void *));

	return cache;
}

bool lru_cache_is_full(lru_cache *cache)
{
	return cache->ht->hmax == cache->ht->size;
}

void free_lru_cache(lru_cache **cache)
{
	ht_free((*cache)->ht);
	ll_free(&(*cache)->order);

	free(*cache);
	*cache = NULL;
}

bool lru_cache_put(lru_cache *cache, void *key, void *value, void **evicted_key)
{
	info_t *info = malloc(sizeof(*info));
	DIE(!info, "malloc info");

	info->key = key;
	info->value = value;

	if (ht_has_key(cache->ht, key)) {
		ll_node_t *node = move_node_to_end(cache->order, info);
		ht_put(cache->ht, key, sizeof(key), node, sizeof(node));

		return false;
	}

	if (lru_cache_is_full(cache)) {
		*evicted_key = ((info_t *)cache->order->head->data)->key;
		ll_remove_nth_node(cache->order, 0);
		ht_remove_entry(cache->ht, *evicted_key);
	}

	ll_node_t *node = ll_add_nth_node(cache->order, cache->order->size, info);
	ht_put(cache->ht, key, sizeof(key), node, sizeof(node));

	return true;
}

void *lru_cache_get(lru_cache *cache, void *key)
{
	if (ht_has_key(cache->ht, key)) {
		ll_node_t *node = ht_get(cache->ht, key);
		move_node_to_end(cache->order, node->data);

		return ((info_t *)node->data)->value;
	}

	return NULL;
}

void lru_cache_remove(lru_cache *cache, void *key)
{
	if (ht_has_key(cache->ht, key)) {
		ll_node_t *node = ht_get(cache->ht, key);
		free(ll_remove_node(cache->order, node));
		ht_remove_entry(cache->ht, key);
	}
}
