/*
 * Copyright (c) 2024, <>
 */

#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <stdbool.h>
#include "utils.h"
#include "add/hashtable.h"
#include "add/specific_linked_list.h"

typedef struct lru_cache {
	// List of keys in the order they were accessed
	ll_list_t *order;

	// Hashtable to store the key-value pairs
	hashtable_t *ht;
} lru_cache;

/*
 * init_lru_cache() - Initializes the LRU cache.
 * 
 * @param cache_capacity: The maximum number of key-value pairs that the cache can store.
 * 
 * @return lru_cache* - The initialized LRU cache.
 * 
 * @brief The function will allocate memory for the cache, initialize the hashtable
 * and the linked list used to store the key-value pairs.
 
*/
lru_cache *init_lru_cache(unsigned int cache_capacity);

/*
 * lru_cache_is_full() - Checks if the cache is full.
 * 
 * @param cache: The cache to be checked.
 * 
 * @return bool - True if the cache is full, false otherwise.
 * 
 * @brief The function will return true if the cache is full, meaning that the
 * number of key-value pairs stored in the cache is equal to the cache's capacity.
 */
bool lru_cache_is_full(lru_cache *cache);

/*
 * free_lru_cache() - Frees the memory allocated for the cache.
 * 
 * @param cache: The cache to be freed.
 * 
 * @brief The function will free the memory allocated for the cache, the hashtable
 * and the linked list used to store the key-value pairs.
 */
void free_lru_cache(lru_cache **cache);

/**
 * lru_cache_put() - Adds a new pair in our cache.
 * 
 * @param cache: Cache where the key-value pair will be stored.
 * @param key: Key of the pair.
 * @param value: Value of the pair.
 * @param evicted_key: The function will RETURN via this parameter the
 *      key removed from cache if the cache was full.
 * 
 * @return - true if the key was added to the cache,
 *      false if the key already existed.
 */
bool lru_cache_put(lru_cache *cache, void *key, void *value,
				   void **evicted_key);

/**
 * lru_cache_get() - Retrieves the value associated with a key.
 * 
 * @param cache: Cache where the key-value pair is stored.
 * @param key: Key of the pair.
 * 
 * @return - The value associated with the key,
 *      or NULL if the key is not found.
 */
void *lru_cache_get(lru_cache *cache, void *key);

/**
 * lru_cache_remove() - Removes a key-value pair from the cache.
 * 
 * @param cache: Cache where the key-value pair is stored.
 * @param key: Key of the pair.
*/
void lru_cache_remove(lru_cache *cache, void *key);

#endif /* LRU_CACHE_H */
