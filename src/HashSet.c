//
// HashSet.c
// Implementation of hash set functions.
//
// This file implements a hash set as a wrapper around HashMap.
// Uses a sentinel value for all HashMap values to implement set semantics.
// Provides average O(1) add, remove, and contains operations.

#include "HashSet.h"
#include <string.h>

//==============================================================================
// Private helper functions
//==============================================================================

// Global variable to store the action function for for_each
static void (*g_hashset_action)(void* key) = NULL;

// Wrapper function that ignores the value parameter for for_each operation
static void hashset_action_wrapper(void* key, void* value)
{
    (void)value; // Ignore the sentinel value
    if (g_hashset_action)
    {
        g_hashset_action(key);
    }
}

//==============================================================================
// Private constants
//==============================================================================

// Sentinel value used for all HashMap values in the set
static void* const HASHSET_PRESENT = (void*)1;

//==============================================================================
// Creation and destruction functions
//==============================================================================

DSCHashSet* dsc_hashset_create(DSCAllocator* alloc, const hash_func hash,
                               const key_equals_func key_equals, const size_t initial_capacity)
{
    if (!alloc || !hash || !key_equals)
    {
        return NULL;
    }

    DSCHashSet* set = dsc_alloc_malloc(alloc, sizeof(DSCHashSet));
    if (!set)
    {
        return NULL;
    }

    set->map = dsc_hashmap_create(alloc, hash, key_equals, initial_capacity);
    if (!set->map)
    {
        dsc_alloc_free(alloc, set);
        return NULL;
    }

    return set;
}

void dsc_hashset_destroy(DSCHashSet* set, const bool should_free_keys)
{
    if (!set)
    {
        return;
    }

    if (set->map)
    {
        const DSCAllocator* alloc = set->map->alloc;

        // Never free values (they're just sentinels), but optionally free keys
        dsc_hashmap_destroy(set->map, should_free_keys, false);

        dsc_alloc_free(alloc, set);
    }
}

void dsc_hashset_clear(DSCHashSet* set, const bool should_free_keys)
{
    if (!set || !set->map)
    {
        return;
    }

    // Never free values (they're just sentinels), but optionally free keys
    dsc_hashmap_clear(set->map, should_free_keys, false);
}

//==============================================================================
// Information functions
//==============================================================================

size_t dsc_hashset_size(const DSCHashSet* set)
{
    return set && set->map ? dsc_hashmap_size(set->map) : 0;
}

int dsc_hashset_is_empty(const DSCHashSet* set)
{
    return !set || !set->map || dsc_hashmap_is_empty(set->map);
}

double dsc_hashset_load_factor(const DSCHashSet* set)
{
    return set && set->map ? dsc_hashmap_load_factor(set->map) : 0.0;
}

//==============================================================================
// Hash set operations
//==============================================================================

int dsc_hashset_add(DSCHashSet* set, void* key)
{
    if (!set || !set->map || !key)
    {
        return -1;
    }

    return dsc_hashmap_put(set->map, key, HASHSET_PRESENT);
}

int dsc_hashset_add_check(DSCHashSet* set, void* key, bool* was_added_out)
{
    if (!set || !set->map || !key || !was_added_out)
    {
        return -1;
    }

    *was_added_out = false;

    void* old_value = NULL;
    const int result = dsc_hashmap_put_replace(set->map, key, HASHSET_PRESENT, &old_value);

    if (result == 0)
    {
        *was_added_out = (old_value == NULL); // New key if no old value
    }

    return result;
}

int dsc_hashset_contains(const DSCHashSet* set, const void* key)
{
    if (!set || !set->map || !key)
    {
        return 0;
    }

    return dsc_hashmap_contains_key(set->map, key);
}

int dsc_hashset_remove(DSCHashSet* set, const void* key, const bool should_free_key)
{
    if (!set || !set->map || !key)
    {
        return -1;
    }

    // Never free values (they're just sentinels), but optionally free keys
    return dsc_hashmap_remove(set->map, key, should_free_key, false);
}

void* dsc_hashset_remove_get(DSCHashSet* set, const void* key)
{
    if (!set || !set->map || !key)
    {
        return NULL;
    }

    // First check if the key exists and get all keys to find the exact pointer
    void** keys;
    size_t count;
    if (dsc_hashmap_get_keys(set->map, &keys, &count) != 0)
    {
        return NULL;
    }

    void* found_key = NULL;
    // Find the actual key pointer that matches
    for (size_t i = 0; i < count; i++)
    {
        if (set->map->key_equals(keys[i], key))
        {
            found_key = keys[i];
            break;
        }
    }

    // Free the keys array
    dsc_alloc_free(set->map->alloc, keys);

    if (!found_key)
    {
        return NULL;
    }

    // Remove the entry (but don't free the key since we're returning it)
    if (dsc_hashmap_remove(set->map, key, false, false) == 0)
    {
        return found_key;
    }

    return NULL;
}

//==============================================================================
// Set operations
//==============================================================================

DSCHashSet* dsc_hashset_union(const DSCHashSet* set1, const DSCHashSet* set2)
{
    if (!set1 || !set1->map || !set2 || !set2->map)
    {
        return NULL;
    }

    // Create result set with same configuration as set1
    DSCHashSet* result = dsc_hashset_create(set1->map->alloc, set1->map->hash,
                                            set1->map->key_equals, 0);
    if (!result)
    {
        return NULL;
    }

    // Add all elements from set1
    DSCIterator it1 = dsc_hashmap_iterator(set1->map);
    while (it1.has_next(&it1))
    {
        const DSCKeyValuePair* pair = it1.get(&it1);

        if (pair && dsc_hashset_add(result, pair->key) != 0)
        {
            it1.destroy(&it1);
            dsc_hashset_destroy(result, false);
            return NULL;
        }
        it1.next(&it1);
    }
    it1.destroy(&it1);

    // Add all elements from set2 (duplicates will be ignored)
    DSCIterator it2 = dsc_hashmap_iterator(set2->map);
    while (it2.has_next(&it2))
    {
        const DSCKeyValuePair* pair = it2.get(&it2);
        if (pair)
        {
            dsc_hashset_add(result, pair->key); // Ignore errors for duplicates
        }
        it2.next(&it2);
    }
    it2.destroy(&it2);

    return result;
}

DSCHashSet* dsc_hashset_intersection(const DSCHashSet* set1, const DSCHashSet* set2)
{
    if (!set1 || !set1->map || !set2 || !set2->map)
    {
        return NULL;
    }

    // Create result set
    DSCHashSet* result = dsc_hashset_create(set1->map->alloc, set1->map->hash,
                                            set1->map->key_equals, 0);
    if (!result)
    {
        return NULL;
    }

    // Iterate through smaller set for efficiency
    const DSCHashSet* smaller = dsc_hashset_size(set1) <= dsc_hashset_size(set2) ? set1 : set2;
    const DSCHashSet* larger = (smaller == set1) ? set2 : set1;

    DSCIterator it = dsc_hashmap_iterator(smaller->map);
    while (it.has_next(&it))
    {
        const DSCKeyValuePair* pair = it.get(&it);

        if (pair && dsc_hashset_contains(larger, pair->key))
        {
            if (dsc_hashset_add(result, pair->key) != 0)
            {
                it.destroy(&it);
                dsc_hashset_destroy(result, false);
                return NULL;
            }
        }
        it.next(&it);
    }
    it.destroy(&it);

    return result;
}

DSCHashSet* dsc_hashset_difference(const DSCHashSet* set1, const DSCHashSet* set2)
{
    if (!set1 || !set1->map)
    {
        return NULL;
    }

    // Create result set
    DSCHashSet* result = dsc_hashset_create(set1->map->alloc, set1->map->hash,
                                            set1->map->key_equals, 0);
    if (!result)
    {
        return NULL;
    }

    // Add elements from set1 that are not in set2
    DSCIterator it = dsc_hashmap_iterator(set1->map);
    while (it.has_next(&it))
    {
        const DSCKeyValuePair* pair = it.get(&it);
        if (pair && (!set2 || !set2->map || !dsc_hashset_contains(set2, pair->key)))
        {
            if (dsc_hashset_add(result, pair->key) != 0)
            {
                it.destroy(&it);
                dsc_hashset_destroy(result, false);
                return NULL;
            }
        }
        it.next(&it);
    }
    it.destroy(&it);

    return result;
}

int dsc_hashset_is_subset(const DSCHashSet* subset, const DSCHashSet* superset)
{
    if (!subset || !subset->map || !superset || !superset->map)
    {
        return 0;
    }

    // Empty set is subset of any set
    if (dsc_hashset_is_empty(subset))
    {
        return 1;
    }

    // Check if all elements of subset are in superset
    DSCIterator it = dsc_hashmap_iterator(subset->map);
    while (it.has_next(&it))
    {
        const DSCKeyValuePair* pair = it.get(&it);

        if (pair && !dsc_hashset_contains(superset, pair->key))
        {
            it.destroy(&it);
            return 0;
        }
        it.next(&it);
    }
    it.destroy(&it);

    return 1;
}

//==============================================================================
// Bulk operations
//==============================================================================

int dsc_hashset_get_elements(const DSCHashSet* set, void*** keys_out, size_t* count_out)
{
    if (!set || !set->map)
    {
        return -1;
    }

    return dsc_hashmap_get_keys(set->map, keys_out, count_out);
}

void dsc_hashset_for_each(const DSCHashSet* set, void (*action)(void* key))
{
    if (!set || !set->map || !action)
    {
        return;
    }

    g_hashset_action = action;
    dsc_hashmap_for_each(set->map, hashset_action_wrapper);
    g_hashset_action = NULL; // Reset after use
}

//==============================================================================
// Hash set copying functions
//==============================================================================

DSCHashSet* dsc_hashset_copy(const DSCHashSet* set)
{
    if (!set || !set->map)
    {
        return NULL;
    }

    DSCHashSet* copy = dsc_hashset_create(set->map->alloc, set->map->hash,
                                          set->map->key_equals, set->map->bucket_count);
    if (!copy)
    {
        return NULL;
    }

    // Copy all elements
    DSCIterator it = dsc_hashmap_iterator(set->map);
    while (it.has_next(&it))
    {
        const DSCKeyValuePair* pair = it.get(&it);

        if (pair && dsc_hashset_add(copy, pair->key) != 0)
        {
            it.destroy(&it);
            dsc_hashset_destroy(copy, false);
            return NULL;
        }
        it.next(&it);
    }
    it.destroy(&it);

    return copy;
}

DSCHashSet* dsc_hashset_copy_deep(const DSCHashSet* set, const copy_func key_copy)
{
    if (!set || !set->map)
    {
        return NULL;
    }

    DSCHashSet* copy = dsc_hashset_create(set->map->alloc, set->map->hash,
                                          set->map->key_equals, set->map->bucket_count);
    if (!copy)
    {
        return NULL;
    }

    // Copy all elements with deep copying of keys
    DSCIterator it = dsc_hashmap_iterator(set->map);
    while (it.has_next(&it))
    {
        const DSCKeyValuePair* pair = it.get(&it);

        if (pair)
        {
            void* copied_key = key_copy ? key_copy(pair->key) : pair->key;
            if ((key_copy && !copied_key) || dsc_hashset_add(copy, copied_key) != 0)
            {
                if (key_copy && copied_key)
                {
                    dsc_alloc_data_free(set->map->alloc, copied_key);
                }
                it.destroy(&it);
                dsc_hashset_destroy(copy, key_copy != NULL);
                return NULL;
            }
        }
        it.next(&it);
    }
    it.destroy(&it);

    return copy;
}

//==============================================================================
// Iterator implementation
//==============================================================================

// HashSet iterator state - wraps HashMap iterator but only returns keys
typedef struct HashSetIteratorState
{
    DSCIterator map_iterator; // Underlying HashMap iterator
    void* current_key;        // Current key to return
    DSCAllocator* alloc;      // Allocator for freeing this state
} HashSetIteratorState;

static void* hashset_iterator_get(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return NULL;
    }

    HashSetIteratorState* state = it->data_state;

    // If we don't have a current key, try to get one from the underlying iterator
    if (!state->current_key && state->map_iterator.has_next(&state->map_iterator))
    {
        const DSCKeyValuePair* pair = state->map_iterator.get(&state->map_iterator);
        if (pair)
        {
            state->current_key = pair->key;
        }
    }

    return state->current_key;
}

static int hashset_iterator_has_next(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const HashSetIteratorState* state = it->data_state;
    return state->map_iterator.has_next(&state->map_iterator);
}

static int hashset_iterator_next(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return -1;
    }

    HashSetIteratorState* state = it->data_state;

    // Advance the underlying iterator
    int result = state->map_iterator.next(&state->map_iterator);

    // Clear current key so next get() will fetch the new one
    state->current_key = NULL;

    return result;
}

static int hashset_iterator_has_prev(const DSCIterator* it)
{
    (void)it;
    return 0; // Hash set iterator doesn't support backward iteration
}

static int hashset_iterator_prev(const DSCIterator* it)
{
    (void)it;
    return -1; // Hash set iterator doesn't support backward iteration
}

static void hashset_iterator_reset(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return;
    }

    HashSetIteratorState* state = it->data_state;
    state->map_iterator.reset(&state->map_iterator);
    state->current_key = NULL;
}

static int hashset_iterator_is_valid(const DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return 0;
    }

    const HashSetIteratorState* state = it->data_state;
    return state->map_iterator.is_valid(&state->map_iterator);
}

static void hashset_iterator_destroy(DSCIterator* it)
{
    if (!it || !it->data_state)
    {
        return;
    }

    HashSetIteratorState* state = it->data_state;
    state->map_iterator.destroy(&state->map_iterator);

    // Free the state using the stored allocator
    dsc_alloc_free(state->alloc, state);

    it->data_state = NULL;
}

// Safe stub functions for invalid iterators
static void* invalid_iterator_get(const DSCIterator* it)
{
    (void)it;
    return NULL;
}

static int invalid_iterator_has_next(const DSCIterator* it)
{
    (void)it;
    return 0;
}

static int invalid_iterator_next(const DSCIterator* it)
{
    (void)it;
    return -1;
}

static int invalid_iterator_has_prev(const DSCIterator* it)
{
    (void)it;
    return 0;
}

static int invalid_iterator_prev(const DSCIterator* it)
{
    (void)it;
    return -1;
}

static void invalid_iterator_reset(const DSCIterator* it)
{
    (void)it;
}

static int invalid_iterator_is_valid(const DSCIterator* it)
{
    (void)it;
    return 0;
}

static void invalid_iterator_destroy(DSCIterator* it)
{
    (void)it;
}

DSCIterator dsc_hashset_iterator(const DSCHashSet* set)
{
    DSCIterator it = {0};

    if (!set || !set->map)
    {
        // Return iterator with safe stub functions
        it.get = invalid_iterator_get;
        it.has_next = invalid_iterator_has_next;
        it.next = invalid_iterator_next;
        it.has_prev = invalid_iterator_has_prev;
        it.prev = invalid_iterator_prev;
        it.reset = invalid_iterator_reset;
        it.is_valid = invalid_iterator_is_valid;
        it.destroy = invalid_iterator_destroy;
        it.data_state = NULL;
        return it;
    }

    it.get = hashset_iterator_get;
    it.has_next = hashset_iterator_has_next;
    it.next = hashset_iterator_next;
    it.has_prev = hashset_iterator_has_prev;
    it.prev = hashset_iterator_prev;
    it.reset = hashset_iterator_reset;
    it.is_valid = hashset_iterator_is_valid;
    it.destroy = hashset_iterator_destroy;

    HashSetIteratorState* state = dsc_alloc_malloc(set->map->alloc, sizeof(HashSetIteratorState));
    if (!state)
    {
        return it;
    }

    state->map_iterator = dsc_hashmap_iterator(set->map);
    state->current_key = NULL;
    state->alloc = set->map->alloc; // Set the allocator reference

    it.alloc = set->map->alloc;
    it.data_state = state;
    return it;
}

DSCHashSet* dsc_hashset_from_iterator(DSCIterator* it, DSCAllocator* alloc,
                                      const hash_func hash, const key_equals_func key_equals, const bool should_copy)
{
    if (!it || !alloc || !hash || !key_equals)
    {
        return NULL;
    }
    if (should_copy && !alloc->copy_func)
    {
        return NULL; // Can't copy without copy function
    }

    if (!it->is_valid || !it->is_valid(it))
    {
        return NULL;
    }

    DSCHashSet* set = dsc_hashset_create(alloc, hash, key_equals, 0);
    if (!set)
    {
        return NULL;
    }

    while (it->has_next(it))
    {
        void* key = it->get(it);

        // Skip NULL elements - they indicate iterator issues
        if (!key)
        {
            if (it->next(it) != 0)
            {
                break; // Iterator exhausted or failed
            }
            continue;
        }

        void* key_to_insert = key;

        // Use copy function if requested
        if (should_copy)
        {
            key_to_insert = alloc->copy_func(key);
            if (!key_to_insert)
            {
                dsc_hashset_destroy(set, true);
                return NULL;
            }
        }

        if (dsc_hashset_add(set, key_to_insert) != 0)
        {
            if (should_copy)
            {
                dsc_alloc_data_free(alloc, key_to_insert);
            }
            dsc_hashset_destroy(set, should_copy);
            return NULL;
        }

        if (it->next(it) != 0)
        {
            break; // Iterator done or failed
        }
    }

    return set;
}
