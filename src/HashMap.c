//
// HashMap.c
// Implementation of hash map functions.
//
// This file implements a hash map with separate chaining for collision resolution.
// Supports custom allocators, hash functions, and key equality functions.
// Provides average O(1) operations with automatic resizing.

#include "HashMap.h"
#include <string.h>
#include <stdint.h>

//==============================================================================
// Default constants
//==============================================================================

#define DEFAULT_INITIAL_CAPACITY 16
#define DEFAULT_MAX_LOAD_FACTOR 0.75

//==============================================================================
// Static helper functions
//==============================================================================

/**
 * Create a new hash map node.
 */
static DSCHashMapNode* create_node(DSCHashMap* map, void* key, void* value)
{
    if (!map || !map->alloc)
    {
        return NULL;
    }

    DSCHashMapNode* node = dsc_alloc_malloc(map->alloc, sizeof(DSCHashMapNode));
    if (!node)
    {
        return NULL;
    }

    node->key = key;
    node->value = value;
    node->next = NULL;
    return node;
}

/**
 * Free a hash map node and optionally its data.
 */
static void free_node(DSCHashMap* map, DSCHashMapNode* node,
                      const bool should_free_key, const bool should_free_value)
{
    if (!map || !node)
    {
        return;
    }

    if (should_free_key && node->key)
    {
        dsc_alloc_data_free(map->alloc, node->key);
    }

    if (should_free_value && node->value)
    {
        dsc_alloc_data_free(map->alloc, node->value);
    }

    dsc_alloc_free(map->alloc, node);
}

/**
 * Get bucket index for a key.
 */
static size_t get_bucket_index(const DSCHashMap* map, const void* key)
{
    if (!map || !map->hash || map->bucket_count == 0)
    {
        return 0;
    }
    return map->hash(key) % map->bucket_count;
}

/**
 * Resize the hash map to a new bucket count.
 */
static int resize_map(DSCHashMap* map, const size_t new_bucket_count)
{
    if (!map || new_bucket_count == 0)
    {
        return -1;
    }

    // Allocate new bucket array
    DSCHashMapNode** new_buckets = dsc_alloc_malloc(map->alloc,
        new_bucket_count * sizeof(DSCHashMapNode*));
    if (!new_buckets)
    {
        return -1;
    }

    // Initialize new buckets to NULL
    for (size_t i = 0; i < new_bucket_count; i++)
    {
        new_buckets[i] = NULL;
    }

    // Save old buckets
    DSCHashMapNode** old_buckets = map->buckets;
    const size_t old_bucket_count = map->bucket_count;

    // Update map with new buckets
    map->buckets = new_buckets;
    map->bucket_count = new_bucket_count;

    // Rehash all existing nodes
    for (size_t i = 0; i < old_bucket_count; i++)
    {
        DSCHashMapNode* node = old_buckets[i];
        while (node)
        {
            DSCHashMapNode* next = node->next;

            // Rehash this node
            const size_t new_index = get_bucket_index(map, node->key);
            node->next = map->buckets[new_index];
            map->buckets[new_index] = node;

            node = next;
        }
    }

    // Free old bucket array
    dsc_alloc_free(map->alloc, old_buckets);
    return 0;
}

/**
 * Check if map needs resizing and resize if necessary.
 */
static int check_and_resize(DSCHashMap* map)
{
    if (!map)
    {
        return -1;
    }

    const double current_load_factor = dsc_hashmap_load_factor(map);
    if (current_load_factor > map->max_load_factor)
    {
        const size_t new_size = map->bucket_count * 2;
        if (new_size < map->bucket_count) // Overflow check
        {
            return -1;
        }
        return resize_map(map, new_size);
    }
    return 0;
}

//==============================================================================
// Creation and destruction functions
//==============================================================================

DSCHashMap* dsc_hashmap_create(DSCAlloc* alloc, const hash_func hash,
                               const key_equals_func key_equals, const size_t initial_capacity)
{
    if (!alloc || !hash || !key_equals)
    {
        return NULL;
    }

    DSCHashMap* map = dsc_alloc_malloc(alloc, sizeof(DSCHashMap));
    if (!map)
    {
        return NULL;
    }

    const size_t capacity = initial_capacity > 0 ? initial_capacity : DEFAULT_INITIAL_CAPACITY;

    map->buckets = dsc_alloc_malloc(alloc, capacity * sizeof(DSCHashMapNode*));
    if (!map->buckets)
    {
        dsc_alloc_free(alloc, map);
        return NULL;
    }

    // Initialize all buckets to NULL
    for (size_t i = 0; i < capacity; i++)
    {
        map->buckets[i] = NULL;
    }

    map->bucket_count = capacity;
    map->size = 0;
    map->max_load_factor = DEFAULT_MAX_LOAD_FACTOR;
    map->hash = hash;
    map->key_equals = key_equals;
    map->alloc = alloc;

    return map;
}

void dsc_hashmap_destroy(DSCHashMap* map, const bool should_free_keys, const bool should_free_values)
{
    if (!map)
    {
        return;
    }

    dsc_hashmap_clear(map, should_free_keys, should_free_values);

    dsc_alloc_free(map->alloc, map->buckets);
    dsc_alloc_free(map->alloc, map);
}

void dsc_hashmap_clear(DSCHashMap* map, const bool should_free_keys, const bool should_free_values)
{
    if (!map || !map->buckets)
    {
        return;
    }

    for (size_t i = 0; i < map->bucket_count; i++)
    {
        DSCHashMapNode* node = map->buckets[i];
        while (node)
        {
            DSCHashMapNode* next = node->next;
            free_node(map, node, should_free_keys, should_free_values);
            node = next;
        }
        map->buckets[i] = NULL;
    }

    map->size = 0;
}

//==============================================================================
// Information functions
//==============================================================================

size_t dsc_hashmap_size(const DSCHashMap* map)
{
    return map ? map->size : 0;
}

int dsc_hashmap_is_empty(const DSCHashMap* map)
{
    return !map || map->size == 0;
}

double dsc_hashmap_load_factor(const DSCHashMap* map)
{
    if (!map || map->bucket_count == 0)
    {
        return 0.0;
    }
    return (double)map->size / (double)map->bucket_count;
}

int dsc_hashmap_contains_key(const DSCHashMap* map, const void* key)
{
    return dsc_hashmap_get(map, key) != NULL;
}

//==============================================================================
// Hash map operations
//==============================================================================

int dsc_hashmap_put(DSCHashMap* map, void* key, void* value)
{
    if (!map || !key)
    {
        return -1;
    }

    const size_t index   = get_bucket_index(map, key);
    DSCHashMapNode* node = map->buckets[index];

    // Check if key already exists
    while (node)
    {
        if (map->key_equals(node->key, key))
        {
            // Update existing value
            node->value = value;
            return 0;
        }
        node = node->next;
    }

    // Create new node
    DSCHashMapNode* new_node = create_node(map, key, value);
    if (!new_node)
    {
        return -1;
    }

    // Insert at beginning of bucket
    new_node->next      = map->buckets[index];
    map->buckets[index] = new_node;
    map->size++;

    // Check if resize is needed
    return check_and_resize(map);
}

int dsc_hashmap_put_replace(DSCHashMap* map, void* key, void* value, void** old_value_out)
{
    if (!map || !key || !old_value_out)
    {
        return -1;
    }

    *old_value_out = NULL;  // Initialize to NULL

    const size_t index   = get_bucket_index(map, key);
    DSCHashMapNode* node = map->buckets[index];

    // Check if key already exists
    while (node)
    {
        if (map->key_equals(node->key, key))
        {
            // Return the old value and update with new value
            *old_value_out = node->value;
            node->value    = value;
            return 0;
        }
        node = node->next;
    }

    // Create new node (key doesn't exist)
    DSCHashMapNode* new_node = create_node(map, key, value);
    if (!new_node)
    {
        return -1;
    }

    // Insert at beginning of bucket
    new_node->next      = map->buckets[index];
    map->buckets[index] = new_node;
    map->size++;

    // Check if resize is needed
    return check_and_resize(map);
}

int dsc_hashmap_put_with_free(DSCHashMap* map, void* key, void* value, const bool should_free_old_value)
{
    if (!map || !key)
    {
        return -1;
    }

    const size_t index   = get_bucket_index(map, key);
    DSCHashMapNode* node = map->buckets[index];

    // Check if key already exists
    while (node)
    {
        if (map->key_equals(node->key, key))
        {
            // Free the old value if requested and possible
            if (should_free_old_value && node->value && map->alloc && map->alloc->data_free_func)
            {
                map->alloc->data_free_func(node->value);
            }

            // Update with new value
            node->value = value;
            return 0;
        }
        node = node->next;
    }

    // Create new node (key doesn't exist)
    DSCHashMapNode* new_node = create_node(map, key, value);
    if (!new_node)
    {
        return -1;
    }

    // Insert at beginning of bucket
    new_node->next      = map->buckets[index];
    map->buckets[index] = new_node;
    map->size++;

    // Check if resize is needed
    return check_and_resize(map);
}

void* dsc_hashmap_get(const DSCHashMap* map, const void* key)
{
    if (!map || !key)
    {
        return NULL;
    }

    const size_t index         = get_bucket_index(map, key);
    const DSCHashMapNode* node = map->buckets[index];

    while (node)
    {
        if (map->key_equals(node->key, key))
        {
            return node->value;
        }
        node = node->next;
    }

    return NULL;
}

int dsc_hashmap_remove(DSCHashMap* map, const void* key,
                       const bool should_free_key, const bool should_free_value)
{
    if (!map || !key)
    {
        return -1;
    }

    const size_t index   = get_bucket_index(map, key);
    DSCHashMapNode* node = map->buckets[index];
    DSCHashMapNode* prev = NULL;

    while (node)
    {
        if (map->key_equals(node->key, key))
        {
            // Remove node from chain
            if (prev)
            {
                prev->next = node->next;
            }
            else
            {
                map->buckets[index] = node->next;
            }

            free_node(map, node, should_free_key, should_free_value);
            map->size--;
            return 0;
        }
        prev = node;
        node = node->next;
    }

    return -1; // Key not found
}

void* dsc_hashmap_remove_get(DSCHashMap* map, const void* key, const bool should_free_key)
{
    if (!map || !key)
    {
        return NULL;
    }

    const size_t index   = get_bucket_index(map, key);
    DSCHashMapNode* node = map->buckets[index];
    DSCHashMapNode* prev = NULL;

    while (node)
    {
        if (map->key_equals(node->key, key))
        {
            // Remove node from chain
            if (prev)
            {
                prev->next = node->next;
            }
            else
            {
                map->buckets[index] = node->next;
            }

            void* value = node->value;
            free_node(map, node, should_free_key, false); // Don't free value
            map->size--;
            return value;
        }
        prev = node;
        node = node->next;
    }

    return NULL; // Key not found
}

//==============================================================================
// Bulk operations
//==============================================================================

int dsc_hashmap_get_keys(const DSCHashMap* map, void*** keys_out, size_t* count_out)
{
    if (!map || !keys_out || !count_out)
    {
        return -1;
    }

    if (map->size == 0)
    {
        *keys_out = NULL;
        *count_out = 0;
        return 0;
    }

    void** keys = dsc_alloc_malloc(map->alloc, map->size * sizeof(void*));
    if (!keys)
    {
        return -1;
    }

    size_t key_index = 0;
    for (size_t i = 0; i < map->bucket_count; i++)
    {
        const DSCHashMapNode* node = map->buckets[i];
        while (node)
        {
            keys[key_index++] = node->key;
            node = node->next;
        }
    }

    *keys_out = keys;
    *count_out = map->size;
    return 0;
}

int dsc_hashmap_get_values(const DSCHashMap* map, void*** values_out, size_t* count_out)
{
    if (!map || !values_out || !count_out)
    {
        return -1;
    }

    if (map->size == 0)
    {
        *values_out = NULL;
        *count_out = 0;
        return 0;
    }

    void** values = dsc_alloc_malloc(map->alloc, map->size * sizeof(void*));
    if (!values)
    {
        return -1;
    }

    size_t value_index = 0;
    for (size_t i = 0; i < map->bucket_count; i++)
    {
        const DSCHashMapNode* node = map->buckets[i];
        while (node)
        {
            values[value_index++] = node->value;
            node = node->next;
        }
    }

    *values_out = values;
    *count_out = map->size;
    return 0;
}

void dsc_hashmap_for_each(const DSCHashMap* map, void (*action)(void* key, void* value))
{
    if (!map || !action)
    {
        return;
    }

    for (size_t i = 0; i < map->bucket_count; i++)
    {
        const DSCHashMapNode* node = map->buckets[i];
        while (node)
        {
            action(node->key, node->value);
            node = node->next;
        }
    }
}

//==============================================================================
// Hash map copying functions
//==============================================================================

DSCHashMap* dsc_hashmap_copy(const DSCHashMap* map)
{
    if (!map)
    {
        return NULL;
    }

    DSCHashMap* copy = dsc_hashmap_create(map->alloc, map->hash,
                                          map->key_equals, map->bucket_count);
    if (!copy)
    {
        return NULL;
    }

    copy->max_load_factor = map->max_load_factor;

    // Copy all key-value pairs
    for (size_t i = 0; i < map->bucket_count; i++)
    {
        const DSCHashMapNode* node = map->buckets[i];
        while (node)
        {
            if (dsc_hashmap_put(copy, node->key, node->value) != 0)
            {
                dsc_hashmap_destroy(copy, false, false);
                return NULL;
            }
            node = node->next;
        }
    }

    return copy;
}

DSCHashMap* dsc_hashmap_copy_deep(const DSCHashMap* map,
                                  const copy_func key_copy, const copy_func value_copy)
{
    if (!map)
    {
        return NULL;
    }

    DSCHashMap* copy = dsc_hashmap_create(map->alloc, map->hash,
                                          map->key_equals, map->bucket_count);
    if (!copy)
    {
        return NULL;
    }

    copy->max_load_factor = map->max_load_factor;

    // Copy all key-value pairs with deep copying
    for (size_t i = 0; i < map->bucket_count; i++)
    {
        const DSCHashMapNode* node = map->buckets[i];
        while (node)
        {
            void* copied_key = key_copy ? key_copy(node->key) : node->key;
            void* copied_value = value_copy ? value_copy(node->value) : node->value;

            // Check if copy functions failed
            if ((key_copy && !copied_key) || (value_copy && !copied_value))
            {
                // Clean up any successful copies
                if (key_copy && copied_key)
                {
                    dsc_alloc_data_free(map->alloc, copied_key);
                }
                if (value_copy && copied_value)
                {
                    dsc_alloc_data_free(map->alloc, copied_value);
                }
                dsc_hashmap_destroy(copy, key_copy != NULL, value_copy != NULL);
                return NULL;
            }

            if (dsc_hashmap_put(copy, copied_key, copied_value) != 0)
            {
                // Clean up on failure
                if (key_copy && copied_key)
                {
                    map->alloc->data_free_func(copied_key);
                }
                if (value_copy && copied_value && map->alloc->data_free_func)
                {
                    map->alloc->data_free_func(copied_value);
                }
                dsc_hashmap_destroy(copy, key_copy != NULL, value_copy != NULL);
                return NULL;
            }
            node = node->next;
        }
    }

    return copy;
}

//==============================================================================
// Iterator implementation
//==============================================================================

typedef struct HashMapIteratorState
{
    const DSCHashMap* map;
    size_t current_bucket;
    DSCHashMapNode* current_node;
    DSCKeyValuePair current_pair;
} HashMapIteratorState;

static void* hashmap_iterator_get(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    HashMapIteratorState* state = it->data_state;
    if (!state->current_node)
    {
        return NULL;
    }

    state->current_pair.key = state->current_node->key;
    state->current_pair.value = state->current_node->value;
    return &state->current_pair;
}

static int hashmap_iterator_has_next(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const HashMapIteratorState* state = it->data_state;
    return state->current_node != NULL;
}

static void* hashmap_iterator_next(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    HashMapIteratorState* state = it->data_state;
    if (!state->current_node)
    {
        return NULL;
    }

    // Save current pair
    state->current_pair.key = state->current_node->key;
    state->current_pair.value = state->current_node->value;

    // Advance to next node
    state->current_node = state->current_node->next;

    // If no more nodes in current bucket, find next non-empty bucket
    while (!state->current_node && state->current_bucket < state->map->bucket_count - 1)
    {
        state->current_bucket++;
        state->current_node = state->map->buckets[state->current_bucket];
    }

    return &state->current_pair;
}

static int hashmap_iterator_has_prev(const DSCIterator* it)
{
    (void)it;
    return 0; // Hash map iterator doesn't support backward iteration
}

static void* hashmap_iterator_prev(const DSCIterator* it)
{
    (void)it;
    return NULL; // Hash map iterator doesn't support backward iteration
}

static void hashmap_iterator_reset(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return;
    }

    HashMapIteratorState* state = it->data_state;
    state->current_bucket = 0;
    state->current_node = NULL;

    // Find first non-empty bucket
    for (size_t i = 0; i < state->map->bucket_count; i++)
    {
        if (state->map->buckets[i])
        {
            state->current_bucket = i;
            state->current_node = state->map->buckets[i];
            break;
        }
    }
}

static int hashmap_iterator_is_valid(const DSCIterator* it)
{
    return it && it->data_state != NULL;
}

static void hashmap_iterator_destroy(DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return;
    }

    HashMapIteratorState* state = it->data_state;
    if (state->map)
    {
        dsc_alloc_free(state->map->alloc, state);
    }
    it->data_state = NULL;
}

DSCIterator dsc_hashmap_iterator(const DSCHashMap* map)
{
    DSCIterator it = {0};

    it.get = hashmap_iterator_get;
    it.has_next = hashmap_iterator_has_next;
    it.next = hashmap_iterator_next;
    it.has_prev = hashmap_iterator_has_prev;
    it.prev = hashmap_iterator_prev;
    it.reset = hashmap_iterator_reset;
    it.is_valid = hashmap_iterator_is_valid;
    it.destroy = hashmap_iterator_destroy;

    if (!map || !map->alloc)
    {
        return it;
    }

    HashMapIteratorState* state = dsc_alloc_malloc(map->alloc, sizeof(HashMapIteratorState));
    if (!state)
    {
        return it;
    }

    state->map = map;
    state->current_bucket = 0;
    state->current_node = NULL;

    // Find first non-empty bucket
    for (size_t i = 0; i < map->bucket_count; i++)
    {
        if (map->buckets[i])
        {
            state->current_bucket = i;
            state->current_node = map->buckets[i];
            break;
        }
    }

    it.alloc = map->alloc;
    it.data_state = state;
    return it;
}

DSCHashMap* dsc_hashmap_from_iterator(DSCIterator* it, DSCAlloc* alloc,
                                      const hash_func hash, const key_equals_func key_equals)
{
    if (!it || !alloc || !hash || !key_equals)
    {
        return NULL;
    }

    if (!it->is_valid(it))
    {
        return NULL;
    }

    DSCHashMap* map = dsc_hashmap_create(alloc, hash, key_equals, 0);
    if (!map)
    {
        return NULL;
    }

    while (it->has_next(it))
    {
        DSCKeyValuePair* pair = it->next(it);
        if (!pair)
        {
            continue;
        }

        void* key_to_insert = pair->key;
        void* value_to_insert = pair->value;

        // Use copy functions if available
        if (alloc->copy_func)
        {
            key_to_insert = alloc->copy_func(pair->key);
            if (!key_to_insert)
            {
                dsc_hashmap_destroy(map, true, true);
                return NULL;
            }

            value_to_insert = alloc->copy_func(pair->value);
            if (!value_to_insert)
            {
                alloc->data_free_func(key_to_insert);
                dsc_hashmap_destroy(map, true, true);
                return NULL;
            }
        }

        if (dsc_hashmap_put(map, key_to_insert, value_to_insert) != 0)
        {
            if (alloc->copy_func && alloc->data_free_func)
            {
                alloc->data_free_func(key_to_insert);
                alloc->data_free_func(value_to_insert);
            }
            dsc_hashmap_destroy(map, alloc->copy_func != NULL, alloc->copy_func != NULL);
            return NULL;
        }
    }

    return map;
}

//==============================================================================
// Utility hash functions
//==============================================================================

size_t dsc_hash_string(const void* key)
{
    if (!key)
    {
        return 0;
    }

    const char* str = key;
    size_t hash = 5381; // djb2 hash algorithm
    int c;

    while ((c = (unsigned char)*str++))
    {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return hash;
}

size_t dsc_hash_int(const void* key)
{
    if (!key)
    {
        return 0;
    }

    const int value = *(const int*)key;
    // Simple integer hash
    size_t hash = (size_t)value;
    hash = ((hash >> 16) ^ hash) * 0x45d9f3b;
    hash = ((hash >> 16) ^ hash) * 0x45d9f3b;
    hash = (hash >> 16) ^ hash;
    return hash;
}

size_t dsc_hash_pointer(const void* key)
{
    // Hash the pointer value itself
    const uintptr_t addr = (uintptr_t)key;
    size_t hash = addr;
    hash = ((hash >> 16) ^ hash) * 0x45d9f3b;
    hash = ((hash >> 16) ^ hash) * 0x45d9f3b;
    hash = (hash >> 16) ^ hash;
    return hash;
}

//==============================================================================
// Utility equality functions
//==============================================================================

int dsc_key_equals_string(const void* key1, const void* key2)
{
    if (!key1 || !key2)
    {
        return key1 == key2;
    }
    return strcmp(key1, key2) == 0;
}

int dsc_key_equals_int(const void* key1, const void* key2)
{
    if (!key1 || !key2)
    {
        return 0;
    }
    return *(const int*)key1 == *(const int*)key2;
}

int dsc_key_equals_pointer(const void* key1, const void* key2)
{
    return key1 == key2;
}