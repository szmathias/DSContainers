//
// Created by zack on 9/2/25.
//

#ifndef DSCONTAINERS_ARRAYLIST_H
#define DSCONTAINERS_ARRAYLIST_H

#include <stddef.h>
#include "Alloc.h"
#include "CStandardCompatibility.h"
#include "Iterator.h"

//==============================================================================
// Type definitions
//==============================================================================

/**
 * Dynamic array (ArrayList) structure with custom allocator support.
 * Provides O(1) access by index, O(1) amortized insertion/deletion at end,
 * and automatic memory management with configurable growth strategy.
 */
typedef struct DSCArrayList
{
    void** data;     // Array of pointers to user data
    size_t size;     // Current number of elements
    size_t capacity; // Maximum number of elements before reallocation
    DSCAlloc* alloc; // Custom allocator
} DSCArrayList;

/**
 * Predicate function for filtering elements.
 * Should return non-zero for elements that match the condition.
 *
 * @param data Pointer to element data
 * @return Non-zero if element matches, zero otherwise
 */
typedef int (*pred_func)(const void* data);

/**
 * Transformation function for mapping elements.
 * Should return a new pointer representing the transformed data.
 *
 * @param data Pointer to original element data
 * @return Pointer to transformed data (may be newly allocated)
 */
typedef void*(*transform_func)(const void* data);

/**
 * Action function for applying an operation to each element.
 * Used in for-each style traversal.
 *
 * @param data Pointer to element data
 */
typedef void (*action_func)(void* data);

//==============================================================================
// Creation and destruction functions
//==============================================================================

/**
 * Create a new, empty ArrayList with custom allocator and initial capacity.
 *
 * @param alloc Custom allocator (required)
 * @param initial_capacity Initial capacity (0 uses default)
 * @return Pointer to new ArrayList, or NULL on failure
 */
DSCArrayList* dsc_arraylist_create(DSCAlloc* alloc, size_t initial_capacity);

/**
 * Destroy the ArrayList and free all elements.
 *
 * @param list The ArrayList to destroy
 * @param should_free_data Whether to free the data elements using alloc->data_free_func
 */
void dsc_arraylist_destroy(DSCArrayList* list, bool should_free_data);

/**
 * Clear all elements from the ArrayList, but keep the structure intact.
 *
 * @param list The ArrayList to clear
 * @param should_free_data Whether to free the data elements
 */
void dsc_arraylist_clear(DSCArrayList* list, bool should_free_data);

//==============================================================================
// Information functions
//==============================================================================

/**
 * Get the number of elements in the ArrayList.
 *
 * @param list The ArrayList to query
 * @return Number of elements, or 0 if list is NULL
 */
size_t dsc_arraylist_size(const DSCArrayList* list);

/**
 * Get the current capacity of the ArrayList.
 *
 * @param list The ArrayList to query
 * @return Current capacity, or 0 if list is NULL
 */
size_t dsc_arraylist_capacity(const DSCArrayList* list);

/**
 * Check if the ArrayList is empty.
 *
 * @param list The ArrayList to check
 * @return 1 if ArrayList is empty or NULL, 0 if it contains elements
 */
int dsc_arraylist_is_empty(const DSCArrayList* list);

/**
 * Find the first element matching data using the comparison function.
 *
 * @param list The ArrayList to search
 * @param data The data to find
 * @param compare The comparison function to use
 * @return Index of matching element, or SIZE_MAX if not found or on error
 */
size_t dsc_arraylist_find(const DSCArrayList* list, const void* data, cmp_func compare);

/**
 * Compare two ArrayLists for equality using the given comparison function.
 *
 * @param list1 First ArrayList to compare
 * @param list2 Second ArrayList to compare
 * @param compare Function to compare elements
 * @return 1 if ArrayLists are equal, 0 if not equal, -1 on error
 */
int dsc_arraylist_equals(const DSCArrayList* list1, const DSCArrayList* list2, cmp_func compare);

//==============================================================================
// Element access functions
//==============================================================================

/**
 * Get element at specified index (bounds checked).
 *
 * @param list The ArrayList to access
 * @param index Zero-based index
 * @return Pointer to element data, or NULL if index is invalid or on error
 */
void* dsc_arraylist_get(const DSCArrayList* list, size_t index);

/**
 * Set element at specified index (bounds checked).
 *
 * @param list The ArrayList to modify
 * @param index Zero-based index
 * @param data New data for the element
 * @param should_free_old Whether to free the old data at this index
 * @return 0 on success, -1 on error (e.g., invalid index)
 */
int dsc_arraylist_set(DSCArrayList* list, size_t index, void* data, bool should_free_old);

/**
 * Get the first element (equivalent to get(0)).
 *
 * @param list The ArrayList to access
 * @return Pointer to first element, or NULL if empty or on error
 */
void* dsc_arraylist_front(const DSCArrayList* list);

/**
 * Get the last element (equivalent to get(size-1)).
 *
 * @param list The ArrayList to access
 * @return Pointer to last element, or NULL if empty or on error
 */
void* dsc_arraylist_back(const DSCArrayList* list);

//==============================================================================
// Insertion functions
//==============================================================================

/**
 * Add element to the end of the ArrayList.
 *
 * @param list The ArrayList to modify
 * @param data Pointer to the data to add (ownership transferred to ArrayList)
 * @return 0 on success, -1 on error
 */
int dsc_arraylist_push_back(DSCArrayList* list, void* data);

/**
 * Add element to the beginning of the ArrayList.
 *
 * @param list The ArrayList to modify
 * @param data Pointer to the data to add (ownership transferred to ArrayList)
 * @return 0 on success, -1 on error
 */
int dsc_arraylist_push_front(DSCArrayList* list, void* data);

/**
 * Insert element at a specific index.
 *
 * @param list The ArrayList to modify
 * @param index Zero-based index where to insert (0 = front, size = back)
 * @param data Pointer to the data to insert (ownership transferred to ArrayList)
 * @return 0 on success, -1 on error (e.g., invalid index)
 */
int dsc_arraylist_insert(DSCArrayList* list, size_t index, void* data);

//==============================================================================
// Removal functions
//==============================================================================

/**
 * Remove the last element from the ArrayList.
 *
 * @param list The ArrayList to modify
 * @param should_free_data Whether to free the removed data
 * @return 0 on success, -1 on error (e.g., empty list)
 */
int dsc_arraylist_pop_back(DSCArrayList* list, bool should_free_data);

/**
 * Remove the first element from the ArrayList.
 *
 * @param list The ArrayList to modify
 * @param should_free_data Whether to free the removed data
 * @return 0 on success, -1 on error (e.g., empty list)
 */
int dsc_arraylist_pop_front(DSCArrayList* list, bool should_free_data);

/**
 * Remove element at a specific index.
 *
 * @param list The ArrayList to modify
 * @param index Zero-based index of element to remove
 * @param should_free_data Whether to free the removed data
 * @return 0 on success, -1 on error (e.g., invalid index)
 */
int dsc_arraylist_remove_at(DSCArrayList* list, size_t index, bool should_free_data);

/**
 * Remove the first element matching data using the comparison function.
 *
 * @param list The ArrayList to modify
 * @param data The data to match for removal
 * @param compare Function to compare elements
 * @param should_free_data Whether to free the removed data
 * @return 0 on success, -1 if not found or on error
 */
int dsc_arraylist_remove(DSCArrayList* list, const void* data, cmp_func compare, bool should_free_data);

//==============================================================================
// Memory management functions
//==============================================================================

/**
 * Reserve space for at least the specified number of elements.
 *
 * @param list The ArrayList to modify
 * @param new_capacity Minimum capacity to reserve
 * @return 0 on success, -1 on error
 */
int dsc_arraylist_reserve(DSCArrayList* list, size_t new_capacity);

/**
 * Shrink the ArrayList's capacity to fit its current size.
 *
 * @param list The ArrayList to modify
 * @return 0 on success, -1 on error
 */
int dsc_arraylist_shrink_to_fit(DSCArrayList* list);

//==============================================================================
// ArrayList manipulation functions
//==============================================================================

/**
 * Sort the ArrayList using the given comparison function.
 * Uses a stable sorting algorithm (merge sort).
 *
 * @param list The ArrayList to sort
 * @param compare Comparison function
 * @return 0 on success, -1 on error
 */
int dsc_arraylist_sort(DSCArrayList* list, cmp_func compare);

/**
 * Reverse the order of elements in the ArrayList.
 *
 * @param list The ArrayList to reverse
 * @return 0 on success, -1 on error
 */
int dsc_arraylist_reverse(DSCArrayList* list);

//==============================================================================
// Higher-order functions
//==============================================================================

/**
 * Create a new ArrayList containing only elements that satisfy the predicate function.
 * Performs a shallow copy: the new ArrayList shares data pointers with the source.
 *
 * @param list The source ArrayList
 * @param pred Function that returns non-zero for elements to include
 * @return A new ArrayList with matching elements (shallow copy), or NULL on error
 */
DSCArrayList* dsc_arraylist_filter(const DSCArrayList* list, pred_func pred);

/**
 * Create a new ArrayList containing only elements that satisfy the predicate function.
 * Performs a deep copy: the new ArrayList contains cloned copies of the matching elements.
 *
 * @param list The source ArrayList
 * @param pred Function that returns non-zero for elements to include
 * @return A new ArrayList with matching elements (deep copy), or NULL on error
 */
DSCArrayList* dsc_arraylist_filter_deep(const DSCArrayList* list, pred_func pred);


/**
 * Create a new ArrayList by transforming each element with the given function.
 *
 * @param list The source ArrayList
 * @param transform Function that returns a new element based on the original
 * @param should_free_data Whether to free the data returned after
 * transformation if an error occurred
 * @return A new ArrayList with transformed elements, or NULL on error
 */
DSCArrayList* dsc_arraylist_transform(const DSCArrayList* list, transform_func transform, bool should_free_data);

/**
 * Apply an action function to each element in the ArrayList.
 *
 * @param list The ArrayList to process
 * @param action Function applied to each element
 */
void dsc_arraylist_for_each(const DSCArrayList* list, action_func action);

//==============================================================================
// ArrayList copying functions
//==============================================================================

/**
 * Create a shallow copy of the ArrayList (sharing data pointers).
 *
 * @param list The ArrayList to copy
 * @return A new ArrayList with same structure but sharing data, or NULL on error
 */
DSCArrayList* dsc_arraylist_copy(const DSCArrayList* list);

/**
 * Create a deep copy of the ArrayList (cloning data using the provided function).
 *
 * @param list The ArrayList to copy
 * @param should_free_data Whether the copy function should handle data freeing
 * @return A new ArrayList with copies of all data, or NULL on error
 */
DSCArrayList* dsc_arraylist_copy_deep(const DSCArrayList* list, bool should_free_data);

//==============================================================================
// Iterator functions
//==============================================================================

/**
 * Create an iterator for the ArrayList (index 0 to size-1 traversal).
 *
 * @param list The ArrayList to iterate over
 * @return An Iterator object for forward traversal
 */
DSCIterator dsc_arraylist_iterator(const DSCArrayList* list);

/**
 * Create a reverse iterator for the ArrayList (index size-1 to 0 traversal).
 *
 * @param list The ArrayList to iterate over
 * @return An Iterator object for backward traversal
 */
DSCIterator dsc_arraylist_iterator_reverse(const DSCArrayList* list);

/**
 * Create a new ArrayList from an iterator with custom allocator.
 *
 * @param it The source iterator (must be valid)
 * @param alloc The custom allocator to use
 * @return A new ArrayList with elements from iterator, or NULL on error
 */
DSCArrayList* dsc_arraylist_from_iterator(DSCIterator * it, DSCAlloc * alloc);

#endif //DSCONTAINERS_ARRAYLIST_H