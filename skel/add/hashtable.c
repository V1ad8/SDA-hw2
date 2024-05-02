#include "hashtable.h"

hashtable_t *ht_create(unsigned int hmax, unsigned int (*hash_function)(void *),
					   int (*compare_function)(void *, void *))
{
	if (!hash_function || !compare_function) {
		return NULL;
	}

	hashtable_t *map = malloc(sizeof(hashtable_t));
	DIE(!map, "Error");

	map->size = 0;
	map->hmax = hmax;
	map->hash_function = hash_function;
	map->compare_function = compare_function;

	map->buckets = malloc(map->hmax * sizeof(ll_list_t *));
	DIE(!map->buckets, "Error");
	for (unsigned int i = 0; i < map->hmax; ++i) {
		map->buckets[i] = ll_create(sizeof(info_t));
	}

	return map;
}

int ht_has_key(hashtable_t *ht, void *key)
{
	if (!ht || !key) {
		return -1;
	}

	int hash_index = ht->hash_function(key) % ht->hmax;
	ll_node_t *node = ht->buckets[hash_index]->head;

	while (node != NULL) {
		info_t *data_info = (info_t *)node->data;
		if (!ht->compare_function(data_info->key, key)) {
			return 1;
		}
		node = node->next;
	}

	return 0;
}

void *ht_get(hashtable_t *ht, void *key)
{
	if (!ht || !key || ht_has_key(ht, key) != 1) {
		return NULL;
	}

	int hash_index = ht->hash_function(key) % ht->hmax;
	ll_node_t *node = ht->buckets[hash_index]->head;

	while (node != NULL) {
		info_t *data_info = (info_t *)node->data;
		if (!ht->compare_function(data_info->key, key)) {
			return data_info->value;
		}
		node = node->next;
	}

	return NULL;
}

void ht_put(hashtable_t *ht, void *key, unsigned int key_size, void *value,
			unsigned int value_size)
{
	if (!ht || !key || !value) {
		return;
	}

	int hash_index = ht->hash_function(key) % ht->hmax;

	if (ht_has_key(ht, key) == 1) {
		ll_node_t *node = ht->buckets[hash_index]->head;

		while (node != NULL) {
			info_t *data_info = node->data;

			if (!ht->compare_function(data_info->key, key)) {
				free(data_info->value);

				data_info->value = malloc(value_size);
				DIE(!data_info, "Error");

				memcpy(data_info->value, value, value_size);
				return;
			}

			node = node->next;
		}
	}

	info_t *data_info = malloc(sizeof(info_t));
	DIE(!data_info, "Error");

	data_info->key = malloc(key_size);
	DIE(!data_info->key, "Error");
	data_info->value = malloc(value_size);
	DIE(!data_info->value, "Error");

	memcpy(data_info->key, key, key_size);
	memcpy(data_info->value, value, value_size);

	ll_add_nth_node(ht->buckets[hash_index], 0, data_info);
	ht->size++;

	free(data_info);
}

void ht_remove_entry(hashtable_t *ht, void *key)
{
	if (!ht || !key || ht_has_key(ht, key) != 1) {
		return;
	}

	int hash_index = ht->hash_function(key) % ht->hmax;
	ll_node_t *node = ht->buckets[hash_index]->head;

	unsigned int node_nr = 0;

	while (node != NULL) {
		info_t *data_info = (info_t *)node->data;

		if (!ht->compare_function(data_info->key, key)) {
			free(data_info->key);
			free(data_info->value);
			free(data_info);

			ll_node_t *deleted_node =
				ll_remove_nth_node(ht->buckets[hash_index], node_nr);
			free(deleted_node);

			ht->size--;
			return;
		}

		node = node->next;
		node_nr++;
	}
}

void ht_free(hashtable_t *ht)
{
	if (!ht) {
		return;
	}

	for (unsigned int i = 0; i < ht->hmax; ++i) {
		ll_node_t *node = ht->buckets[i]->head;

		while (node != NULL) {
			info_t *data_info = (info_t *)node->data;
			free(data_info->key);
			free(data_info->value);
			node = node->next;
		}

		ll_free(&ht->buckets[i]);
	}

	free(ht->buckets);
	free(ht);
}

unsigned int ht_get_size(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->size;
}

unsigned int ht_get_hmax(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->hmax;
}

int compare_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}
