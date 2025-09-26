//
// Created by zack on 8/23/25.
//
// This header defines a small, generic singly linked list implementation
// with support for a custom allocator (ANVAllocator). The API provides creation
// and destruction, insertion/removal, higher-order operations (filter,
// transform, for_each), copying utilities, and iterator helpers. Functions
// return 0 on success and non-zero on failure where applicable. When
// copying or transforming elements the allocator's copy_func and
// data_free_func are used when provided.

#ifndef ANVIL_SINGLYLINKEDLIST_H
#define ANVIL_SINGLYLINKEDLIST_H

#include <stddef.h>

#include "Iterator.h"
#include "common/Allocator.h"
#include "common/CStandardCompatibility.h"

#ifdef __cplusplus
extern "C" {
#endif

//==============================================================================
// Type definitions
//==============================================================================

/**
 * Node of a singly linked list.
 */
typedef struct ANVSinglyLinkedNode
{
    void* data;                       // Pointer to user data
    struct ANVSinglyLinkedNode* next; // Pointer to next node
} ANVSinglyLinkedNode;

/**
 * Singly linked list structure.
 * - head: pointer to first node
 * - tail: pointer to last node
 * - size: number of elements
 * - alloc: pointer to allocator used for node/list memory and data operations
 */
typedef struct ANVSinglyLinkedList
{
    ANVSinglyLinkedNode* head; // Pointer to first node
    ANVSinglyLinkedNode* tail; // Pointer to last node
    size_t size;               // Number of nodes in list

    ANVAllocator* alloc; // Custom allocator
} ANVSinglyLinkedList;

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
ANV_API ANVSinglyLinkedList* anv_sll_create(ANVAllocator * alloc);

/**
 * Destroy the list and free all nodes.
 *
 * @param list The list to destroy
 * @param should_free_data If true the allocator's data_free_func is used to
 *                         free each element's data pointer
 */
ANV_API void anv_sll_destroy(ANVSinglyLinkedList* list, bool should_free_data);

/**
 * Clear all nodes but keep the list structure intact.
 *
 * @param list The list to clear
 * @param should_free_data If true use allocator->data_free_func on elements
 */
ANV_API void anv_sll_clear(ANVSinglyLinkedList* list, bool should_free_data);

//==============================================================================
// Information functions
//==============================================================================

/**
 * Get number of elements in list.
 *
 * @param list The list to query
 * @return Number of elements, or 0 if list is NULL
 */
ANV_API size_t anv_sll_size(const ANVSinglyLinkedList* list);

/**
 * Check whether the list is empty.
 *
 * @param list The list to query
 * @return 1 if empty or NULL, 0 otherwise
 */
ANV_API int anv_sll_is_empty(const ANVSinglyLinkedList* list);

/**
 * Find first node matching data using compare function.
 *
 * @param list The list to search
 * @param data The value to match
 * @param compare Comparison function (returns 0 when equal)
 * @return Pointer to matching node, or NULL if not found or on error
 */
ANV_API ANVSinglyLinkedNode* anv_sll_find(const ANVSinglyLinkedList* list, const void* data, cmp_func compare);

/**
 * Compare two lists for equality element-wise using compare.
 *
 * @param list1 First list
 * @param list2 Second list
 * @param compare Comparison function used for elements
 * @return 1 if equal, 0 if not equal, -1 on error
 */
ANV_API int anv_sll_equals(const ANVSinglyLinkedList* list1, const ANVSinglyLinkedList* list2, cmp_func compare);

//==============================================================================
// Insertion functions
//==============================================================================

/**
 * Add element to the front of the list.
 *
 * @param list The list to modify
 * @param data Pointer to data (ownership transferred to list)
 * @return 0 on success, -1 on error
 */
ANV_API int anv_sll_push_front(ANVSinglyLinkedList* list, void* data);

/**
 * Add element to the back of the list.
 *
 * @param list The list to modify
 * @param data Pointer to data (ownership transferred to list)
 * @return 0 on success, -1 on error
 */
ANV_API int anv_sll_push_back(ANVSinglyLinkedList* list, void* data);

/**
 * Insert element at a specific position (0..size).
 *
 * @param list The list to modify
 * @param pos Zero-based insertion index (0 = front, size = back)
 * @param data Pointer to data (ownership transferred to list)
 * @return 0 on success, -1 on error
 */
ANV_API int anv_sll_insert_at(ANVSinglyLinkedList* list, size_t pos, void* data);

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
ANV_API int anv_sll_remove(ANVSinglyLinkedList* list, const void* data, cmp_func compare, bool should_free_data);

/**
 * Remove element at position pos.
 *
 * @param list The list to modify
 * @param pos Zero-based index of element to remove
 * @param should_free_data If true free element data using allocator->data_free_func
 * @return 0 on success, -1 on error
 */
ANV_API int anv_sll_remove_at(ANVSinglyLinkedList* list, size_t pos, bool should_free_data);

/**
 * Remove the first element.
 *
 * @param list The list to modify
 * @param should_free_data If true free element data
 * @return 0 on success, -1 on error
 */
ANV_API int anv_sll_pop_front(ANVSinglyLinkedList* list, bool should_free_data);

/**
 * Remove the last element.
 *
 * @param list The list to modify
 * @param should_free_data If true free element data
 * @return 0 on success, -1 on error
 */
ANV_API int anv_sll_pop_back(ANVSinglyLinkedList* list, bool should_free_data);

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
ANV_API int anv_sll_sort(ANVSinglyLinkedList* list, cmp_func compare);

/**
 * Reverse the order of nodes in the list.
 *
 * @param list The list to reverse
 * @return 0 on success, -1 on error
 */
ANV_API int anv_sll_reverse(ANVSinglyLinkedList* list);

/**
 * Merge src list into dest (append src onto dest). After the call src is
 * emptied but not destroyed.
 *
 * @param dest Destination list
 * @param src Source list (will be emptied)
 * @return 0 on success, -1 on error
 */
ANV_API int anv_sll_merge(ANVSinglyLinkedList* dest, ANVSinglyLinkedList* src);

/**
 * Splice src into dest at position pos. After the call src is emptied but
 * not destroyed.
 *
 * @param dest Destination list
 * @param src Source list (will be emptied)
 * @param pos Position in dest where to insert src
 * @return 0 on success, -1 on error
 */
ANV_API int anv_sll_splice(ANVSinglyLinkedList* dest, ANVSinglyLinkedList* src, size_t pos);

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
ANV_API ANVSinglyLinkedList* anv_sll_filter(const ANVSinglyLinkedList* list, pred_func pred);

/**
 * Create a new list containing only elements that satisfy the predicate.
 * Performs a deep copy: if the allocator provides copy_func, elements are copied into the new list.
 *
 * @param list Source list
 * @param pred Predicate function
 * @return New list with matching elements (deep copy), or NULL on error
 */
ANV_API ANVSinglyLinkedList* anv_sll_filter_deep(const ANVSinglyLinkedList* list, pred_func pred);

/**
 * Create a new list by transforming each element using transform. The
 * transform function is responsible for allocating any new element data.
 *
 * @param list Source list
 * @param transform Transformation function
 * @param should_free_data If true free transformed element data on failure
 * @return New transformed list, or NULL on error
 */
ANV_API ANVSinglyLinkedList* anv_sll_transform(const ANVSinglyLinkedList* list, transform_func transform, bool should_free_data);

/**
 * Apply an action to each element in the list.
 *
 * @param list The list to traverse
 * @param action Function applied to each element
 */
ANV_API void anv_sll_for_each(const ANVSinglyLinkedList* list, action_func action);

//==============================================================================
// List copying functions
//==============================================================================

/**
 * Create a shallow copy of the list (data pointers are shared).
 *
 * @param list The list to copy
 * @return New list sharing the same data pointers, or NULL on error
 */
ANV_API ANVSinglyLinkedList* anv_sll_copy(const ANVSinglyLinkedList* list);

/**
 * Create a deep copy of the list using copy_data to clone elements.
 *
 * @param list The list to copy
 * @param copy_data Function to copy element data
 * @param should_free_data If true free copied data on failure
 * @return New list with deep-copied elements, or NULL on error
 */
ANV_API ANVSinglyLinkedList* anv_sll_copy_deep(const ANVSinglyLinkedList* list, copy_func copy_data, bool should_free_data);

//==============================================================================
// Iterator functions
//==============================================================================

/**
 * Create an iterator for the list (head->tail traversal).
 *
 * @param list The list to iterate
 * @return An iterator object configured for forward traversal
 */
ANV_API ANVIterator anv_sll_iterator(const ANVSinglyLinkedList* list);

/**
 * Create a new SinglyLinkedList from an iterator with custom allocator.
 *
 * This function consumes all elements from the provided iterator and creates
 * a new SinglyLinkedList containing those elements. The iteration follows the standard
 * get()/next() pattern, filtering out any NULL elements returned by the iterator.
 * Elements are added to the SinglyLinkedList in the order they are encountered from the iterator.
 *
 * @param it The source iterator (must be valid and support has_next/get/next)
 * @param alloc The custom allocator to use for the new SinglyLinkedList
 * @param should_copy If true, creates deep copies of all elements using alloc->copy_func.
 *                    If false, uses elements directly from iterator.
 *                    When true, alloc->copy_func must not be NULL.
 * @return A new SinglyLinkedList with elements from iterator, or NULL on error
 *
 * @note NULL elements from the iterator are always filtered out as they indicate
 *       iterator issues rather than valid data.
 * @note The iterator is consumed during this operation - it will be at the end
 *       position after the function completes.
 * @note If should_copy is true and copying fails for any element, the function
 *       cleans up and returns NULL.
 */
ANV_API ANVSinglyLinkedList* anv_sll_from_iterator(ANVIterator* it, ANVAllocator* alloc, bool should_copy);

#ifdef __cplusplus
}
#endif

#endif //ANVIL_SINGLYLINKEDLIST_H

