#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "hash_table.h"

// Number of buckets in the hash table.
#define SIZE 4

/*
 * Returns the bucket index from the key.
 */
static uint8_t ht_bucket_index(uint64_t key) {
  return key % SIZE;
}

/*
 * Adds a key-value pair to the given hash table.
 */
void ht_add(ht_hash_table *table, uint64_t key, uint8_t value) {
  ht_item *item;
  uint8_t idx = ht_bucket_index(key);
  ht_bucket *bucket = &table->buckets[idx];
  printf("Bucket %d contains %d items\n", idx, bucket->nr_items);

  if (bucket->nr_items < bucket->size) {
    item = &bucket->items[bucket->nr_items++];
    printf("Adding item %d to bucket: %d\n", item->value, idx);
  } else {
    uint32_t new_size = bucket->size * 2;
    bucket->items = realloc(bucket->items, new_size * sizeof(ht_item));
    if (bucket->items == NULL) {
      return;
    }
    bucket->size = new_size; 
    item = &bucket->items[bucket->nr_items++];
    printf("Adding item %d to increased bucket: %d\n", item->value, idx);
  }

  item->key = key;
  item->value = value;
}

uint8_t ht_get(ht_hash_table *table, uint64_t key) {
  uint8_t idx = ht_bucket_index(key);
  ht_bucket *bucket = &table->buckets[idx];

  for (int i = 0; i < bucket->nr_items; i++) {
    ht_item *item = &bucket->items[i];

    if (item->key == key) {
      return item->value;
    }
  }

  return 0;
}

/*
 * Creates a new hash table.
 */
ht_hash_table *ht_create(void) {
  ht_hash_table *table = malloc(sizeof(ht_hash_table));
  table->buckets = malloc(SIZE * sizeof(ht_bucket));

  for (int i = 0; i < SIZE; i++) {
    table->buckets[i].items = malloc(2 * sizeof(ht_item));
    table->buckets[i].size = 2;
    table->buckets[i].nr_items = 0;
  }

  return table;
}

/*
 * Frees all memory allocated by the given hash
 * table.
 */
void ht_free(ht_hash_table *table) {
  for (int i = 0; i < SIZE; i++) {
    free(table->buckets[i].items);
  }

  free(table->buckets);
  free(table);
}

int main(void) {
  ht_hash_table *table = ht_create();
  ht_add(table, 4, 33);
  ht_add(table, 8, 83);
  ht_add(table, 16, 58);
  ht_add(table, 32, 12);
  ht_add(table, 64, 2);

  printf("%d\n", ht_get(table, 64));

  ht_free(table);
}
