//
// HashSet.h
// A set implementation built on top of HashMap.
//
// This file implements a hash set for storing unique keys only.
// Built as a wrapper around HashMap for code reuse and efficiency.
// Provides average O(1) add, remove, and contains operations.

#ifndef DSCONTAINERS_HASHSET_H
#define DSCONTAINERS_HASHSET_H

#include <stddef.h>
#include "HashMap.h"
#include "Alloc.h"
#include "Iterator.h"

//==============================================================================
// Type definitions
//==============================================================================

/**
 * Hash set structure with custom allocator support.
 * Uses HashMap internally with a sentinel value for all elements.
 * Provides average O(1) add, remove, and contains operations.
 */
typedef struct DSCHashSet
{
    DSCHashMap* map;                 // Underlying hash map
} DSCHashSet;

//==============================================================================
// Creation and destruction functions
//==============================================================================

/**
 * Create a new hash set with custom allocator and functions.
 *
 * @param alloc Custom allocator (required)
 * @param hash Hash function for keys (required)
 * @param key_equals Key equality function (required)
 * @param initial_capacity Initial number of buckets (0 for default)
 * @return Pointer to new hash set, or NULL on failure
 */
DSCHashSet* dsc_hashset_create(DSCAlloc* alloc, hash_func hash,
                               key_equals_func key_equals, size_t initial_capacity);

/**
 * Destroy the hash set and free all nodes.
 *
 * @param set The hash set to destroy
 * @param should_free_keys Whether to free key data using alloc->data_free_func
 */
void dsc_hashset_destroy(DSCHashSet* set, bool should_free_keys);

/**
 * Clear all elements from the hash set, but keep the structure intact.
 *
 * @param set The hash set to clear
 * @param should_free_keys Whether to free key data
 */
void dsc_hashset_clear(DSCHashSet* set, bool should_free_keys);

//==============================================================================
// Information functions
//==============================================================================

/**
 * Get the number of elements in the hash set.
 *
 * @param set The hash set to query
 * @return Number of elements, or 0 if set is NULL
 */
size_t dsc_hashset_size(const DSCHashSet* set);

/**
 * Check if the hash set is empty.
 *
 * @param set The hash set to check
 * @return 1 if empty or NULL, 0 if it contains elements
 */
int dsc_hashset_is_empty(const DSCHashSet* set);

/**
 * Get the current load factor of the hash set.
 *
 * @param set The hash set to query
 * @return Load factor (size / bucket_count), or 0.0 if set is NULL
 */
double dsc_hashset_load_factor(const DSCHashSet* set);

//==============================================================================
// Hash set operations
//==============================================================================

/**
 * Add an element to the hash set.
 * If the element already exists, this is a no-op.
 *
 * @param set The hash set to modify
 * @param key Pointer to key data (ownership transferred to set)
 * @return 0 on success, -1 on error
 */
int dsc_hashset_add(DSCHashSet* set, void* key);

/**
 * Add an element to the hash set, returning whether it was newly added.
 *
 * @param set The hash set to modify
 * @param key Pointer to key data (ownership transferred to set)
 * @param was_added_out Pointer to store whether key was newly added (not NULL if new)
 * @return 0 on success, -1 on error
 */
int dsc_hashset_add_check(DSCHashSet* set, void* key, bool* was_added_out);

/**
 * Check if the hash set contains an element.
 *
 * @param set The hash set to search
 * @param key The key to search for
 * @return 1 if element exists, 0 if not found or on error
 */
int dsc_hashset_contains(const DSCHashSet* set, const void* key);

/**
 * Remove an element from the hash set.
 *
 * @param set The hash set to modify
 * @param key The key to remove
 * @param should_free_key Whether to free the key data
 * @return 0 on success, -1 if key not found or on error
 */
int dsc_hashset_remove(DSCHashSet* set, const void* key, bool should_free_key);

/**
 * Remove an element and return it.
 *
 * @param set The hash set to modify
 * @param key The key to remove
 * @return Pointer to the removed key, or NULL if not found or on error
 */
void* dsc_hashset_remove_get(DSCHashSet* set, const void* key);

//==============================================================================
// Set operations
//==============================================================================

/**
 * Create the union of two hash sets (elements in either set).
 *
 * @param set1 First hash set
 * @param set2 Second hash set
 * @return New hash set containing union, or NULL on error
 */
DSCHashSet* dsc_hashset_union(const DSCHashSet* set1, const DSCHashSet* set2);

/**
 * Create the intersection of two hash sets (elements in both sets).
 *
 * @param set1 First hash set
 * @param set2 Second hash set
 * @return New hash set containing intersection, or NULL on error
 */
DSCHashSet* dsc_hashset_intersection(const DSCHashSet* set1, const DSCHashSet* set2);

/**
 * Create the difference of two hash sets (elements in first but not second).
 *
 * @param set1 First hash set
 * @param set2 Second hash set
 * @return New hash set containing difference, or NULL on error
 */
DSCHashSet* dsc_hashset_difference(const DSCHashSet* set1, const DSCHashSet* set2);

/**
 * Check if one set is a subset of another.
 *
 * @param subset The potential subset
 * @param superset The potential superset
 * @return 1 if subset is contained in superset, 0 otherwise
 */
int dsc_hashset_is_subset(const DSCHashSet* subset, const DSCHashSet* superset);

//==============================================================================
// Bulk operations
//==============================================================================

/**
 * Get all elements in the hash set.
 *
 * @param set The hash set to query
 * @param keys_out Pointer to array that will be allocated and filled with keys
 * @param count_out Pointer to size_t that will receive the number of keys
 * @return 0 on success, -1 on error
 */
int dsc_hashset_get_elements(const DSCHashSet* set, void*** keys_out, size_t* count_out);

/**
 * Apply an action function to each element in the hash set.
 *
 * @param set The hash set to process
 * @param action Function applied to each key
 */
void dsc_hashset_for_each(const DSCHashSet* set, void (*action)(void* key));

//==============================================================================
// Hash set copying functions
//==============================================================================

/**
 * Create a shallow copy of the hash set (sharing key pointers).
 *
 * @param set The hash set to copy
 * @return A new hash set with same structure but sharing data, or NULL on error
 */
DSCHashSet* dsc_hashset_copy(const DSCHashSet* set);

/**
 * Create a deep copy of the hash set (cloning keys).
 *
 * @param set The hash set to copy
 * @param key_copy Function to copy key data (NULL for shallow copy of keys)
 * @return A new hash set with copies of all data, or NULL on error
 */
DSCHashSet* dsc_hashset_copy_deep(const DSCHashSet* set, copy_func key_copy);

//==============================================================================
// Iterator functions
//==============================================================================

/**
 * Create an iterator for the hash set (unordered traversal).
 * Iterator yields key pointers directly (not DSCKeyValuePair).
 *
 * @param set The hash set to iterate over
 * @return An Iterator object for traversal
 */
DSCIterator dsc_hashset_iterator(const DSCHashSet* set);

/**
 * Create a new hash set from an iterator of keys.
 *
 * @param it The source iterator (must yield key pointers)
 * @param alloc The custom allocator to use
 * @param hash Hash function for keys
 * @param key_equals Key equality function
 * @return A new hash set with elements from iterator, or NULL on error
 */
DSCHashSet* dsc_hashset_from_iterator(DSCIterator* it, DSCAlloc* alloc,
                                      hash_func hash, key_equals_func key_equals);

#endif //DSCONTAINERS_HASHSET_H
