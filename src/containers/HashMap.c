//
// HashMap.c
// Implementation of hash map functions.
//
// This file implements a hash map with separate chaining for collision resolution.
// Supports custom allocators, hash functions, and key equality functions.
// Provides average O(1) operations with automatic resizing.

#include <stdint.h>
#include <string.h>

#include "HashMap.h"
#include "Pair.h"

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
static ANVHashMapNode* create_node(const ANVHashMap* map, void* key, void* value)
{
    if (!map->alloc)
    {
        return NULL;
    }

    ANVHashMapNode* node = anv_alloc_malloc(map->alloc, sizeof(ANVHashMapNode));
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
static void free_node(const ANVHashMap* map, ANVHashMapNode* node,
                      const bool should_free_key, const bool should_free_value)
{
    if (!node)
    {
        return;
    }

    if (should_free_key && node->key)
    {
        anv_alloc_data_free(map->alloc, node->key);
    }

    if (should_free_value && node->value)
    {
        anv_alloc_data_free(map->alloc, node->value);
    }

    anv_alloc_free(map->alloc, node);
}

/**
 * Get bucket index for a key.
 */
static size_t get_bucket_index(const ANVHashMap* map, const void* key)
{
    if (!map->hash || map->bucket_count == 0)
    {
        return 0;
    }
    return map->hash(key) % map->bucket_count;
}

/**
 * Resize the hash map to a new bucket count.
 */
static int resize_map(ANVHashMap* map, const size_t new_bucket_count)
{
    if (new_bucket_count == 0)
    {
        return -1;
    }

    // Allocate new bucket array
    ANVHashMapNode** new_buckets = anv_alloc_malloc(map->alloc,
                                                    new_bucket_count * sizeof(ANVHashMapNode*));
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
    ANVHashMapNode** old_buckets = map->buckets;
    const size_t old_bucket_count = map->bucket_count;

    // Update map with new buckets
    map->buckets = new_buckets;
    map->bucket_count = new_bucket_count;

    // Rehash all existing nodes
    for (size_t i = 0; i < old_bucket_count; i++)
    {
        ANVHashMapNode* node = old_buckets[i];
        while (node)
        {
            ANVHashMapNode* next = node->next;

            // Rehash this node
            const size_t new_index = get_bucket_index(map, node->key);
            node->next = map->buckets[new_index];
            map->buckets[new_index] = node;

            node = next;
        }
    }

    // Free old bucket array
    anv_alloc_free(map->alloc, old_buckets);
    return 0;
}

/**
 * Check if map needs resizing and resize if necessary.
 */
static int check_and_resize(ANVHashMap* map)
{
    const double current_load_factor = anv_hashmap_load_factor(map);
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

ANV_API ANVHashMap* anv_hashmap_create(ANVAllocator* alloc, const hash_func hash,
                               const key_equals_func key_equals, const size_t initial_capacity)
{
    if (!alloc || !hash || !key_equals)
    {
        return NULL;
    }

    ANVHashMap* map = anv_alloc_malloc(alloc, sizeof(ANVHashMap));
    if (!map)
    {
        return NULL;
    }

    const size_t capacity = initial_capacity > 0 ? initial_capacity : DEFAULT_INITIAL_CAPACITY;

    map->buckets = anv_alloc_malloc(alloc, capacity * sizeof(ANVHashMapNode*));
    if (!map->buckets)
    {
        anv_alloc_free(alloc, map);
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

ANV_API void anv_hashmap_destroy(ANVHashMap* map, const bool should_free_keys, const bool should_free_values)
{
    if (!map)
    {
        return;
    }

    anv_hashmap_clear(map, should_free_keys, should_free_values);

    anv_alloc_free(map->alloc, map->buckets);
    anv_alloc_free(map->alloc, map);
}

ANV_API void anv_hashmap_clear(ANVHashMap* map, const bool should_free_keys, const bool should_free_values)
{
    if (!map || !map->buckets)
    {
        return;
    }

    for (size_t i = 0; i < map->bucket_count; i++)
    {
        ANVHashMapNode* node = map->buckets[i];
        while (node)
        {
            ANVHashMapNode* next = node->next;
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

ANV_API size_t anv_hashmap_size(const ANVHashMap* map)
{
    return map ? map->size : 0;
}

ANV_API int anv_hashmap_is_empty(const ANVHashMap* map)
{
    return !map || map->size == 0;
}

ANV_API double anv_hashmap_load_factor(const ANVHashMap* map)
{
    if (!map || map->bucket_count == 0)
    {
        return 0.0;
    }
    return (double)map->size / (double)map->bucket_count;
}

ANV_API int anv_hashmap_contains_key(const ANVHashMap* map, const void* key)
{
    return anv_hashmap_get(map, key) != NULL;
}

//==============================================================================
// Hash map operations
//==============================================================================

ANV_API int anv_hashmap_put(ANVHashMap* map, void* key, void* value)
{
    if (!map || !key)
    {
        return -1;
    }

    const size_t index = get_bucket_index(map, key);
    ANVHashMapNode* node = map->buckets[index];

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
    ANVHashMapNode* new_node = create_node(map, key, value);
    if (!new_node)
    {
        return -1;
    }

    // Insert at beginning of bucket
    new_node->next = map->buckets[index];
    map->buckets[index] = new_node;
    map->size++;

    // Check if resize is needed
    return check_and_resize(map);
}

ANV_API int anv_hashmap_put_replace(ANVHashMap* map, void* key, void* value, void** old_value_out)
{
    if (!map || !key || !old_value_out)
    {
        return -1;
    }

    *old_value_out = NULL; // Initialize to NULL

    const size_t index = get_bucket_index(map, key);
    ANVHashMapNode* node = map->buckets[index];

    // Check if key already exists
    while (node)
    {
        if (map->key_equals(node->key, key))
        {
            // Return the old value and update with new value
            *old_value_out = node->value;
            node->value = value;
            return 0;
        }
        node = node->next;
    }

    // Create new node (key doesn't exist)
    ANVHashMapNode* new_node = create_node(map, key, value);
    if (!new_node)
    {
        return -1;
    }

    // Insert at beginning of bucket
    new_node->next = map->buckets[index];
    map->buckets[index] = new_node;
    map->size++;

    // Check if resize is needed
    return check_and_resize(map);
}

ANV_API int anv_hashmap_put_with_free(ANVHashMap* map, void* key, void* value, const bool should_free_old_value)
{
    if (!map || !key)
    {
        return -1;
    }

    const size_t index = get_bucket_index(map, key);
    ANVHashMapNode* node = map->buckets[index];

    // Check if key already exists
    while (node)
    {
        if (map->key_equals(node->key, key))
        {
            // Free the old value if requested and possible
            if (should_free_old_value && node->value && map->alloc && map->alloc->data_free)
            {
                map->alloc->data_free(node->value);
            }

            // Update with new value
            node->value = value;
            return 0;
        }
        node = node->next;
    }

    // Create new node (key doesn't exist)
    ANVHashMapNode* new_node = create_node(map, key, value);
    if (!new_node)
    {
        return -1;
    }

    // Insert at beginning of bucket
    new_node->next = map->buckets[index];
    map->buckets[index] = new_node;
    map->size++;

    // Check if resize is needed
    return check_and_resize(map);
}

ANV_API void* anv_hashmap_get(const ANVHashMap* map, const void* key)
{
    if (!map || !key)
    {
        return NULL;
    }

    const size_t index = get_bucket_index(map, key);
    const ANVHashMapNode* node = map->buckets[index];

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

ANV_API int anv_hashmap_remove(ANVHashMap* map, const void* key,
                       const bool should_free_key, const bool should_free_value)
{
    if (!map || !key)
    {
        return -1;
    }

    const size_t index = get_bucket_index(map, key);
    ANVHashMapNode* node = map->buckets[index];
    ANVHashMapNode* prev = NULL;

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

ANV_API void* anv_hashmap_remove_get(ANVHashMap* map, const void* key, const bool should_free_key)
{
    if (!map || !key)
    {
        return NULL;
    }

    const size_t index = get_bucket_index(map, key);
    ANVHashMapNode* node = map->buckets[index];
    ANVHashMapNode* prev = NULL;

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

ANV_API int anv_hashmap_get_keys(const ANVHashMap* map, void*** keys_out, size_t* count_out)
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

    void** keys = anv_alloc_malloc(map->alloc, map->size * sizeof(void*));
    if (!keys)
    {
        return -1;
    }

    size_t key_index = 0;
    for (size_t i = 0; i < map->bucket_count; i++)
    {
        const ANVHashMapNode* node = map->buckets[i];
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

ANV_API int anv_hashmap_get_values(const ANVHashMap* map, void*** values_out, size_t* count_out)
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

    void** values = anv_alloc_malloc(map->alloc, map->size * sizeof(void*));
    if (!values)
    {
        return -1;
    }

    size_t value_index = 0;
    for (size_t i = 0; i < map->bucket_count; i++)
    {
        const ANVHashMapNode* node = map->buckets[i];
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

ANV_API void anv_hashmap_for_each(const ANVHashMap* map, void (*action)(void* key, void* value))
{
    if (!map || !action)
    {
        return;
    }

    for (size_t i = 0; i < map->bucket_count; i++)
    {
        const ANVHashMapNode* node = map->buckets[i];
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

ANV_API ANVHashMap* anv_hashmap_copy(const ANVHashMap* map)
{
    if (!map)
    {
        return NULL;
    }

    ANVHashMap* copy = anv_hashmap_create(map->alloc, map->hash,
                                          map->key_equals, map->bucket_count);
    if (!copy)
    {
        return NULL;
    }

    copy->max_load_factor = map->max_load_factor;

    // Copy all key-value pairs
    for (size_t i = 0; i < map->bucket_count; i++)
    {
        const ANVHashMapNode* node = map->buckets[i];
        while (node)
        {
            if (anv_hashmap_put(copy, node->key, node->value) != 0)
            {
                anv_hashmap_destroy(copy, false, false);
                return NULL;
            }
            node = node->next;
        }
    }

    return copy;
}

ANV_API ANVHashMap* anv_hashmap_copy_deep(const ANVHashMap* map,
                                  const copy_func key_copy, const copy_func value_copy)
{
    if (!map)
    {
        return NULL;
    }

    ANVHashMap* copy = anv_hashmap_create(map->alloc, map->hash,
                                          map->key_equals, map->bucket_count);
    if (!copy)
    {
        return NULL;
    }

    copy->max_load_factor = map->max_load_factor;

    // Copy all key-value pairs with deep copying
    for (size_t i = 0; i < map->bucket_count; i++)
    {
        const ANVHashMapNode* node = map->buckets[i];
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
                    anv_alloc_data_free(map->alloc, copied_key);
                }
                if (value_copy && copied_value)
                {
                    anv_alloc_data_free(map->alloc, copied_value);
                }
                anv_hashmap_destroy(copy, key_copy != NULL, value_copy != NULL);
                return NULL;
            }

            if (anv_hashmap_put(copy, copied_key, copied_value) != 0)
            {
                // Clean up on failure
                if (key_copy)
                {
                    anv_alloc_data_free(map->alloc, copied_key);
                }
                if (value_copy)
                {
                    anv_alloc_data_free(map->alloc, copied_value);
                }
                anv_hashmap_destroy(copy, key_copy != NULL, value_copy != NULL);
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
    const ANVHashMap* map;
    size_t current_bucket;
    ANVHashMapNode* current_node;
    ANVPair current_pair;
} HashMapIteratorState;

static void* hashmap_iterator_get(const ANVIterator* it)
{
    HashMapIteratorState* state = it->data_state;
    if (!state->current_node)
    {
        return NULL;
    }

    state->current_pair = (ANVPair) {
        .first = state->current_node->key,
        .second = state->current_node->value,
        .alloc = state->map->alloc
    };

    return &state->current_pair;
}

static int hashmap_iterator_has_next(const ANVIterator* it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const HashMapIteratorState* state = it->data_state;
    return state->current_node != NULL;
}

static int hashmap_iterator_next(const ANVIterator* it)
{
    if (!it || !it->data_state)
    {
        return -1;
    }

    HashMapIteratorState* state = it->data_state;
    if (!state->current_node)
    {
        return -1;
    }

    // Advance to next node
    state->current_node = state->current_node->next;

    // If no more nodes in current bucket, find next non-empty bucket
    while (!state->current_node && state->current_bucket < state->map->bucket_count - 1)
    {
        state->current_bucket++;
        state->current_node = state->map->buckets[state->current_bucket];
    }

    return 0;
}

static int hashmap_iterator_has_prev(const ANVIterator* it)
{
    (void)it;
    return 0; // Hash map iterator doesn't support backward iteration
}

static int hashmap_iterator_prev(const ANVIterator* it)
{
    (void)it;
    return -1; // Hash map iterator doesn't support backward iteration
}

static void hashmap_iterator_reset(const ANVIterator* it)
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

static int hashmap_iterator_is_valid(const ANVIterator* it)
{
    return it && it->data_state != NULL;
}

static void hashmap_iterator_destroy(ANVIterator* it)
{
    if (!it || !it->data_state)
    {
        return;
    }

    HashMapIteratorState* state = it->data_state;
    if (state->map)
    {
        anv_alloc_free(state->map->alloc, state);
    }
    it->data_state = NULL;
}

ANV_API ANVIterator anv_hashmap_iterator(const ANVHashMap* map)
{
    ANVIterator it = {0};

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

    HashMapIteratorState* state = anv_alloc_malloc(map->alloc, sizeof(HashMapIteratorState));
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

ANV_API ANVHashMap* anv_hashmap_from_iterator(ANVIterator* it, ANVAllocator* alloc,
                                      const hash_func hash, const key_equals_func key_equals, const bool should_copy)
{
    if (!it || !alloc || !hash || !key_equals)
    {
        return NULL;
    }

    if (should_copy && !alloc->copy)
    {
        return NULL; // Can't copy without copy function
    }

    if (!it->is_valid || !it->is_valid(it))
    {
        return NULL;
    }

    ANVHashMap* map = anv_hashmap_create(alloc, hash, key_equals, 0);
    if (!map)
    {
        return NULL;
    }

    while (it->has_next(it))
    {
        ANVPair* pair = it->get(it);

        if (!pair)
        {
            if (it->next(it) != 0)
            {
                break;
            }
            continue;
        }

        void* key_to_insert;
        void* value_to_insert;

        if (should_copy)
        {
            // Use allocator's copy function on the pair
            ANVPair* copied_pair = alloc->copy(pair);
            if (!copied_pair)
            {
                anv_hashmap_destroy(map, true, true);
                return NULL;
            }

            key_to_insert = anv_pair_first(copied_pair);
            value_to_insert = anv_pair_second(copied_pair);

            // Free the pair structure but not the data (we're transferring ownership)
            anv_pair_destroy(copied_pair, false, false);
        }
        else
        {
            key_to_insert = anv_pair_first(pair);
            value_to_insert = anv_pair_second(pair);
        }

        if (anv_hashmap_put(map, key_to_insert, value_to_insert) != 0)
        {
            if (should_copy)
            {
                anv_alloc_data_free(alloc, key_to_insert);
                anv_alloc_data_free(alloc, value_to_insert);
            }
            anv_hashmap_destroy(map, should_copy, should_copy);
            return NULL;
        }

        if (it->next(it) != 0)
        {
            break;
        }
    }

    return map;
}

//==============================================================================
// Utility hash functions
//==============================================================================

ANV_API size_t anv_hash_string(const void* key)
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

ANV_API size_t anv_hash_int(const void* key)
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

ANV_API size_t anv_hash_pointer(const void* key)
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

ANV_API int anv_key_equals_string(const void* key1, const void* key2)
{
    if (!key1 || !key2)
    {
        return key1 == key2;
    }
    return strcmp(key1, key2) == 0;
}

ANV_API int anv_key_equals_int(const void* key1, const void* key2)
{
    if (!key1 || !key2)
    {
        return 0;
    }
    return *(const int*)key1 == *(const int*)key2;
}

ANV_API int anv_key_equals_pointer(const void* key1, const void* key2)
{
    return key1 == key2;
}