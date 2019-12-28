#include <stdint.h>

// Hash table entry.
typedef struct {
  uint64_t key;
  uint8_t value;
} ht_item;

// Hash table bucket.
typedef struct {
  uint32_t size;
  uint32_t nr_items;
  ht_item *items;
} ht_bucket;

// Hash table.
typedef struct {
  ht_bucket *buckets;
} ht_hash_table;
