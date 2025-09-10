//
// Created by zack on 8/23/25.
//
// This header defines a small, generic singly linked list implementation
// with support for a custom allocator (DSCAlloc). The API provides creation
// and destruction, insertion/removal, higher-order operations (filter,
// transform, for_each), copying utilities, and iterator helpers. Functions
// return 0 on success and non-zero on failure where applicable. When
// copying or transforming elements the allocator's copy_func and
// data_free_func are used when provided.

#ifndef DSCONTAINERS_SINGLYLINKEDLIST_H
#define DSCONTAINERS_SINGLYLINKEDLIST_H

#include <stddef.h>
#include "Alloc.h"
#include "CStandardCompatibility.h"
#include "Iterator.h"
#include "PlatformDefs.h"

//==============================================================================
// Type definitions
//==============================================================================

/**
 * Node of a singly linked list.
 */
typedef struct DSCSinglyLinkedNode
{
    void* data;                       // Pointer to user data
    struct DSCSinglyLinkedNode* next; // Pointer to next node
} DSCSinglyLinkedNode;

/**
 * Singly linked list structure.
 * - head: pointer to first node
 * - tail: pointer to last node
 * - size: number of elements
 * - alloc: pointer to allocator used for node/list memory and data operations
 */
typedef struct DSCSinglyLinkedList
{
    DSCSinglyLinkedNode* head; // Pointer to first node
    DSCSinglyLinkedNode* tail; // Pointer to last node
    size_t size;               // Number of nodes in list

    DSCAlloc* alloc; // Custom allocator
} DSCSinglyLinkedList;

/**
 * Predicate function used by filter.
 * Should return non-zero for elements to include.
 *
 * @param data Pointer to element data
 * @return non-zero if matches, zero otherwise
 */
typedef int (*pred_func)(const void* data);

/**
 * Transformation function used by transform.
 * Should return a pointer to the transformed data (may allocate memory).
 *
 * @param data Pointer to original element data
 * @return Pointer to transformed data (or NULL on failure)
 */
typedef void*(*transform_func)(const void* data);

/**
 * Action function used by for-each traversal.
 *
 * @param data Pointer to element data
 */
typedef void (*action_func)(void* data);

//==============================================================================
// Creation and destruction functions
//==============================================================================

/**
 * Create a new, empty singly linked list with custom allocator.
 *
 * @param alloc Allocator to use (alloc->alloc_func/ dealloc_func must be valid)
 * @return Pointer to new list, or NULL on failure
 */
DSC_API DSCSinglyLinkedList* dsc_sll_create(DSCAlloc * alloc);

/**
 * Destroy the list and free all nodes.
 *
 * @param list The list to destroy
 * @param should_free_data If true the allocator's data_free_func is used to
 *                         free each element's data pointer
 */
DSC_API void dsc_sll_destroy(DSCSinglyLinkedList* list, bool should_free_data);

/**
 * Clear all nodes but keep the list structure intact.
 *
 * @param list The list to clear
 * @param should_free_data If true use allocator->data_free_func on elements
 */
DSC_API void dsc_sll_clear(DSCSinglyLinkedList* list, bool should_free_data);

//==============================================================================
// Information functions
//==============================================================================

/**
 * Get number of elements in list.
 *
 * @param list The list to query
 * @return Number of elements, or 0 if list is NULL
 */
DSC_API size_t dsc_sll_size(const DSCSinglyLinkedList* list);

/**
 * Check whether the list is empty.
 *
 * @param list The list to query
 * @return 1 if empty or NULL, 0 otherwise
 */
DSC_API int dsc_sll_is_empty(const DSCSinglyLinkedList* list);

/**
 * Find first node matching data using compare function.
 *
 * @param list The list to search
 * @param data The value to match
 * @param compare Comparison function (returns 0 when equal)
 * @return Pointer to matching node, or NULL if not found or on error
 */
DSC_API DSCSinglyLinkedNode* dsc_sll_find(const DSCSinglyLinkedList* list, const void* data, cmp_func compare);

/**
 * Compare two lists for equality element-wise using compare.
 *
 * @param list1 First list
 * @param list2 Second list
 * @param compare Comparison function used for elements
 * @return 1 if equal, 0 if not equal, -1 on error
 */
DSC_API int dsc_sll_equals(const DSCSinglyLinkedList* list1, const DSCSinglyLinkedList* list2, cmp_func compare);

//==============================================================================
// Insertion functions
//==============================================================================

/**
 * Insert element at the front of the list.
 *
 * @param list The list to modify
 * @param data Pointer to data (ownership transferred to list)
 * @return 0 on success, -1 on error
 */
DSC_API int dsc_sll_insert_front(DSCSinglyLinkedList* list, void* data);

/**
 * Insert element at the back of the list.
 *
 * @param list The list to modify
 * @param data Pointer to data (ownership transferred to list)
 * @return 0 on success, -1 on error
 */
DSC_API int dsc_sll_insert_back(DSCSinglyLinkedList* list, void* data);

/**
 * Insert element at a specific position (0..size).
 *
 * @param list The list to modify
 * @param pos Zero-based insertion index (0 = front, size = back)
 * @param data Pointer to data (ownership transferred to list)
 * @return 0 on success, -1 on error
 */
DSC_API int dsc_sll_insert_at(DSCSinglyLinkedList* list, size_t pos, void* data);

//==============================================================================
// Removal functions
//==============================================================================

/**
 * Remove first element matching data using compare.
 *
 * @param list The list to modify
 * @param data The value to match
 * @param compare Comparison function (returns 0 when equal)
 * @param should_free_data If true free element data using allocator->data_free_func
 * @return 0 on success, -1 if not found or on error
 */
DSC_API int dsc_sll_remove(DSCSinglyLinkedList* list, const void* data, cmp_func compare, bool should_free_data);

/**
 * Remove element at position pos.
 *
 * @param list The list to modify
 * @param pos Zero-based index of element to remove
 * @param should_free_data If true free element data using allocator->data_free_func
 * @return 0 on success, -1 on error
 */
DSC_API int dsc_sll_remove_at(DSCSinglyLinkedList* list, size_t pos, bool should_free_data);

/**
 * Remove the first element.
 *
 * @param list The list to modify
 * @param should_free_data If true free element data
 * @return 0 on success, -1 on error
 */
DSC_API int dsc_sll_remove_front(DSCSinglyLinkedList* list, bool should_free_data);

/**
 * Remove the last element.
 *
 * @param list The list to modify
 * @param should_free_data If true free element data
 * @return 0 on success, -1 on error
 */
DSC_API int dsc_sll_remove_back(DSCSinglyLinkedList* list, bool should_free_data);

//==============================================================================
// List manipulation functions
//==============================================================================

/**
 * Sort the list using a merge-based algorithm.
 *
 * @param list The list to sort
 * @param compare Comparison function
 * @return 0 on success, -1 on error
 */
DSC_API int dsc_sll_sort(DSCSinglyLinkedList* list, cmp_func compare);

/**
 * Reverse the order of nodes in the list.
 *
 * @param list The list to reverse
 * @return 0 on success, -1 on error
 */
DSC_API int dsc_sll_reverse(DSCSinglyLinkedList* list);

/**
 * Merge src list into dest (append src onto dest). After the call src is
 * emptied but not destroyed.
 *
 * @param dest Destination list
 * @param src Source list (will be emptied)
 * @return 0 on success, -1 on error
 */
DSC_API int dsc_sll_merge(DSCSinglyLinkedList* dest, DSCSinglyLinkedList* src);

/**
 * Splice src into dest at position pos. After the call src is emptied but
 * not destroyed.
 *
 * @param dest Destination list
 * @param src Source list (will be emptied)
 * @param pos Position in dest where to insert src
 * @return 0 on success, -1 on error
 */
DSC_API int dsc_sll_splice(DSCSinglyLinkedList* dest, DSCSinglyLinkedList* src, size_t pos);

//==============================================================================
// Higher-order functions
//==============================================================================

/**
 * Create a new list containing only elements that satisfy the predicate.
 * Performs a shallow copy: elements in the new list reuse the original data pointers.
 *
 * @param list Source list
 * @param pred Predicate function
 * @return New list with matching elements (shallow copy), or NULL on error
 */
DSC_API DSCSinglyLinkedList* dsc_sll_filter(const DSCSinglyLinkedList* list, pred_func pred);

/**
 * Create a new list containing only elements that satisfy the predicate.
 * Performs a deep copy: if the allocator provides copy_func, elements are copied into the new list.
 *
 * @param list Source list
 * @param pred Predicate function
 * @return New list with matching elements (deep copy), or NULL on error
 */
DSC_API DSCSinglyLinkedList* dsc_sll_filter_deep(const DSCSinglyLinkedList* list, pred_func pred);


/**
 * Create a new list by transforming each element using transform. The
 * transform function is responsible for allocating any new element data.
 *
 * @param list Source list
 * @param transform Transformation function
 * @param should_free_data If true free transformed element data on failure
 * @return New transformed list, or NULL on error
 */
DSC_API DSCSinglyLinkedList* dsc_sll_transform(const DSCSinglyLinkedList* list, transform_func transform, bool should_free_data);

/**
 * Apply an action to each element in the list.
 *
 * @param list The list to traverse
 * @param action Function applied to each element
 */
DSC_API void dsc_sll_for_each(const DSCSinglyLinkedList* list, action_func action);

//==============================================================================
// List copying functions
//==============================================================================

/**
 * Create a shallow copy of the list (data pointers are shared).
 *
 * @param list The list to copy
 * @return New list sharing the same data pointers, or NULL on error
 */
DSC_API DSCSinglyLinkedList* dsc_sll_copy(const DSCSinglyLinkedList* list);

/**
 * Create a deep copy of the list using copy_data to clone elements.
 *
 * @param list The list to copy
 * @param copy_data Function to copy element data
 * @param should_free_data If true free copied data on failure
 * @return New list with deep-copied elements, or NULL on error
 */
DSC_API DSCSinglyLinkedList* dsc_sll_copy_deep(const DSCSinglyLinkedList* list, copy_func copy_data, bool should_free_data);

//==============================================================================
// Iterator functions
//==============================================================================

/**
 * Create an iterator for the list (head->tail traversal).
 *
 * @param list The list to iterate
 * @return An iterator object configured for forward traversal
 */
DSC_API DSCIterator dsc_sll_iterator(const DSCSinglyLinkedList* list);

/**
 * Create a new list from an iterator using the provided allocator.
 *
 * @param it Source iterator (must be valid)
 * @param alloc Allocator to use for the new list
 * @return New list with elements from iterator, or NULL on error
 */
DSC_API DSCSinglyLinkedList* dsc_sll_from_iterator(DSCIterator * it, DSCAlloc * alloc);

#endif //DSCONTAINERS_SINGLYLINKEDLIST_H

