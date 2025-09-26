//
// Created by zack on 9/2/25.
//

#ifndef ANVIL_ARRAYLIST_H
#define ANVIL_ARRAYLIST_H

#include <stddef.h>

#include "Iterator.h"
#include "common/Allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

//==============================================================================
// Type definitions
//==============================================================================

/**
 * Dynamic array (ArrayList) structure with custom allocator support.
 * Provides O(1) access by index, O(1) amortized insertion/deletion at end,
 * and automatic memory management with configurable growth strategy.
 */
typedef struct ANVArrayList
{
    void** data;         // Array of pointers to user data
    size_t size;         // Current number of elements
    size_t capacity;     // Maximum number of elements before reallocation
    ANVAllocator* alloc; // Custom allocator
} ANVArrayList;

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
ANV_API ANVArrayList* anv_arraylist_create(ANVAllocator* alloc, size_t initial_capacity);

/**
 * Destroy the ArrayList and free all elements.
 *
 * @param list The ArrayList to destroy
 * @param should_free_data Whether to free the data elements using alloc->data_free_func
 */
ANV_API void anv_arraylist_destroy(ANVArrayList* list, bool should_free_data);

/**
 * Clear all elements from the ArrayList, but keep the structure intact.
 *
 * @param list The ArrayList to clear
 * @param should_free_data Whether to free the data elements
 */
ANV_API void anv_arraylist_clear(ANVArrayList* list, bool should_free_data);

//==============================================================================
// Information functions
//==============================================================================

/**
 * Get the number of elements in the ArrayList.
 *
 * @param list The ArrayList to query
 * @return Number of elements, or 0 if list is NULL
 */
ANV_API size_t anv_arraylist_size(const ANVArrayList* list);

/**
 * Get the current capacity of the ArrayList.
 *
 * @param list The ArrayList to query
 * @return Current capacity, or 0 if list is NULL
 */
ANV_API size_t anv_arraylist_capacity(const ANVArrayList* list);

/**
 * Check if the ArrayList is empty.
 *
 * @param list The ArrayList to check
 * @return 1 if ArrayList is empty or NULL, 0 if it contains elements
 */
ANV_API int anv_arraylist_is_empty(const ANVArrayList* list);

/**
 * Find the first element matching data using the comparison function.
 *
 * @param list The ArrayList to search
 * @param data The data to find
 * @param compare The comparison function to use
 * @return Index of matching element, or SIZE_MAX if not found or on error
 */
ANV_API size_t anv_arraylist_find(const ANVArrayList* list, const void* data, cmp_func compare);

/**
 * Compare two ArrayLists for equality using the given comparison function.
 *
 * @param list1 First ArrayList to compare
 * @param list2 Second ArrayList to compare
 * @param compare Function to compare elements
 * @return 1 if ArrayLists are equal, 0 if not equal, -1 on error
 */
ANV_API int anv_arraylist_equals(const ANVArrayList* list1, const ANVArrayList* list2, cmp_func compare);

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
ANV_API void* anv_arraylist_get(const ANVArrayList* list, size_t index);

/**
 * Set element at specified index (bounds checked).
 *
 * @param list The ArrayList to modify
 * @param index Zero-based index
 * @param data New data for the element
 * @param should_free_old Whether to free the old element data
 * @return 0 on success, -1 on error
 */
ANV_API int anv_arraylist_set(ANVArrayList* list, size_t index, void* data, bool should_free_old);

/**
 * Get pointer to first element (NULL if empty).
 *
 * @param list The ArrayList to access
 * @return Pointer to first element data, or NULL if empty or on error
 */
ANV_API void* anv_arraylist_front(const ANVArrayList* list);

/**
 * Get pointer to last element (NULL if empty).
 *
 * @param list The ArrayList to access
 * @return Pointer to last element data, or NULL if empty or on error
 */
ANV_API void* anv_arraylist_back(const ANVArrayList* list);

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
ANV_API int anv_arraylist_push_back(ANVArrayList* list, void* data);

/**
 * Add element to the beginning of the ArrayList.
 *
 * @param list The ArrayList to modify
 * @param data Pointer to the data to add (ownership transferred to ArrayList)
 * @return 0 on success, -1 on error
 */
ANV_API int anv_arraylist_push_front(ANVArrayList* list, void* data);

/**
 * Insert element at a specific position.
 *
 * @param list The ArrayList to modify
 * @param index Zero-based index where to insert (0 = front, size = back)
 * @param data Pointer to the data to insert (ownership transferred to ArrayList)
 * @return 0 on success, -1 on error (e.g., invalid index)
 */
ANV_API int anv_arraylist_insert(ANVArrayList* list, size_t index, void* data);

//==============================================================================
// Removal functions
//==============================================================================

/**
 * Remove the last element from the ArrayList.
 *
 * @param list The ArrayList to modify
 * @param should_free_data Whether to free the element data
 * @return 0 on success, -1 on error (e.g., empty ArrayList)
 */
ANV_API int anv_arraylist_pop_back(ANVArrayList* list, bool should_free_data);

/**
 * Remove the first element from the ArrayList.
 *
 * @param list The ArrayList to modify
 * @param should_free_data Whether to free the removed data
 * @return 0 on success, -1 on error (e.g., empty list)
 */
ANV_API int anv_arraylist_pop_front(ANVArrayList* list, bool should_free_data);

/**
 * Remove element at a specific position.
 *
 * @param list The ArrayList to modify
 * @param index Zero-based index of element to remove
 * @param should_free_data Whether to free the element data
 * @return 0 on success, -1 on error (e.g., invalid index)
 */
ANV_API int anv_arraylist_remove_at(ANVArrayList* list, size_t index, bool should_free_data);

/**
 * Remove the first element matching data using the comparison function.
 *
 * @param list The ArrayList to modify
 * @param data The data to match for removal
 * @param compare Function to compare elements
 * @param should_free_data Whether to free the element data
 * @return 0 on success, -1 if not found or on error
 */
ANV_API int anv_arraylist_remove(ANVArrayList* list, const void* data, cmp_func compare, bool should_free_data);

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
ANV_API int anv_arraylist_reserve(ANVArrayList* list, size_t new_capacity);

/**
 * Shrink the ArrayList capacity to fit its current size.
 *
 * @param list The ArrayList to modify
 * @return 0 on success, -1 on error
 */
ANV_API int anv_arraylist_shrink_to_fit(ANVArrayList* list);

//==============================================================================
// Algorithm functions
//==============================================================================

/**
 * Sort the ArrayList using the specified comparison function.
 *
 * @param list The ArrayList to sort
 * @param compare Comparison function
 * @return 0 on success, -1 on error
 */
ANV_API int anv_arraylist_sort(ANVArrayList* list, cmp_func compare);

/**
 * Reverse the order of elements in the ArrayList.
 *
 * @param list The ArrayList to reverse
 * @return 0 on success, -1 on error
 */
ANV_API int anv_arraylist_reverse(ANVArrayList* list);

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
ANV_API ANVArrayList* anv_arraylist_filter(const ANVArrayList* list, pred_func pred);

/**
 * Create a new ArrayList containing only elements that satisfy the predicate function.
 * Performs a deep copy: the new ArrayList contains cloned copies of the matching elements.
 *
 * @param list The source ArrayList
 * @param pred Function that returns non-zero for elements to include
 * @return A new ArrayList with matching elements (deep copy), or NULL on error
 */
ANV_API ANVArrayList* anv_arraylist_filter_deep(const ANVArrayList* list, pred_func pred);

/**
 * Create a new ArrayList by transforming each element with the given function.
 *
 * @param list The source ArrayList
 * @param transform Function that returns a new element based on the original
 * @param should_free_data Whether to free transformed data if error occurs
 * @return A new ArrayList with transformed elements, or NULL on error
 */
ANV_API ANVArrayList* anv_arraylist_transform(const ANVArrayList* list, transform_func transform, bool should_free_data);

/**
 * Apply an action function to each element in the ArrayList.
 *
 * @param list The ArrayList to process
 * @param action Function applied to each element
 */
ANV_API void anv_arraylist_for_each(const ANVArrayList* list, action_func action);

//==============================================================================
// ArrayList copying functions
//==============================================================================

/**
 * Create a shallow copy of the ArrayList (sharing data pointers).
 *
 * @param list The ArrayList to copy
 * @return A new ArrayList with same structure but sharing data, or NULL on error
 */
ANV_API ANVArrayList* anv_arraylist_copy(const ANVArrayList* list);

/**
 * Create a deep copy of the ArrayList (cloning data using the provided function).
 *
 * @param list The ArrayList to copy
 * @param should_free_data Whether the copy function should handle data freeing
 * @return A new ArrayList with copies of all data, or NULL on error
 */
ANV_API ANVArrayList* anv_arraylist_copy_deep(const ANVArrayList* list, bool should_free_data);

//==============================================================================
// Iterator functions
//==============================================================================

/**
 * Create an iterator for the ArrayList.
 *
 * @param list The ArrayList to iterate over
 * @return An Iterator object for traversal
 */
ANV_API ANVIterator anv_arraylist_iterator(const ANVArrayList* list);

/**
 * Create a reverse iterator for the ArrayList.
 *
 * @param list The ArrayList to iterate over
 * @return An Iterator object for reverse traversal
 */
ANV_API ANVIterator anv_arraylist_iterator_reverse(const ANVArrayList* list);

/**
 * Create a new ArrayList from an iterator with custom allocator.
 *
 * This function consumes all elements from the provided iterator and creates
 * a new ArrayList containing those elements. The iteration follows the standard
 * get()/next() pattern, filtering out any NULL elements returned by the iterator.
 * Elements are added to the ArrayList in the order they are encountered from the iterator.
 *
 * @param it The source iterator (must be valid and support has_next/get/next)
 * @param alloc The custom allocator to use for the new ArrayList
 * @param should_copy If true, creates deep copies of all elements using alloc->copy_func.
 *                    If false, uses elements directly from iterator.
 *                    When true, alloc->copy_func must not be NULL.
 * @return A new ArrayList with elements from iterator, or NULL on error
 *
 * @note NULL elements from the iterator are always filtered out as they indicate
 *       iterator issues rather than valid data.
 * @note The iterator is consumed during this operation - it will be at the end
 *       position after the function completes.
 * @note If should_copy is true and copying fails for any element, the function
 *       cleans up and returns NULL.
 */
ANV_API ANVArrayList* anv_arraylist_from_iterator(ANVIterator* it, ANVAllocator* alloc, bool should_copy);

#ifdef __cplusplus
}
#endif

#endif //ANVIL_ARRAYLIST_H