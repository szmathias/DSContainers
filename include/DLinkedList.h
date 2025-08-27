//
// Created by zack on 8/25/25.
//
// Doubly linked list interface for generic data storage.
// This implementation provides a comprehensive set of operations
// for a doubly linked list with support for custom memory allocation.

#ifndef DS_DLINKEDLIST_H
#define DS_DLINKEDLIST_H

#include <stddef.h>

#include "Iterator.h"

//==============================================================================
// Type definitions
//==============================================================================

// Node of doubly linked list
typedef struct DListNode
{
    void *data;               // Pointer to user data
    struct DListNode *next;   // Pointer to next node
    struct DListNode *prev;   // Pointer to previous node
} DListNode;

// Doubly linked list structure with custom allocator support
typedef struct DLinkedList
{
    DListNode *head;          // Pointer to first node
    DListNode *tail;          // Pointer to last node
    size_t size;              // Number of nodes in list

    void * (*alloc)(size_t);  // Custom allocation function
    void (*dealloc)(void *);  // Custom deallocation function
} DLinkedList;

//==============================================================================
// Function pointer types
//==============================================================================

// Comparison function: returns <0 if a<b, 0 if a==b, >0 if a>b
typedef int (*cmp_func)(const void *a, const void *b);

// Function to free user data
typedef void (*free_func)(void *ptr);

// Memory allocation function (compatible with malloc)
typedef void * (*alloc_func)(size_t size);

// Memory deallocation function (compatible with free)
typedef void (*dealloc_func)(void *ptr);

// Predicate function: returns non-zero for matched elements, 0 otherwise
typedef int (*pred_func)(const void *data);

// Transformation function: creates a new element from an existing one
typedef void *(*transform_func)(const void *data);

// Action function: performs an operation on each element
typedef void (*action_func)(void *data);

// Copy function: creates a deep copy of data
typedef void *(*copy_func)(const void *data);

//==============================================================================
// Creation and destruction functions
//==============================================================================

/**
 * Create a new, empty doubly linked list using malloc/free.
 *
 * @return Pointer to new DLinkedList, or NULL on allocation failure.
 */
DLinkedList *dll_create(void);

/**
 * Create a new, empty doubly linked list with custom allocator.
 *
 * @param alloc Custom allocation function (if NULL, uses malloc)
 * @param dealloc Custom deallocation function (if NULL, uses free)
 * @return Pointer to new DLinkedList, or NULL on failure.
 */
DLinkedList *dll_create_custom(alloc_func alloc, dealloc_func dealloc);

/**
 * Destroy the list and free all nodes.
 *
 * @param list The list to destroy
 * @param data_free Function to free user data (can be NULL if data shouldn't be freed)
 */
void dll_destroy(DLinkedList *list, free_func data_free);

/**
 * Clear all nodes from the list, but keep the list structure intact.
 *
 * @param list The list to clear
 * @param data_free Function to free user data (can be NULL if data shouldn't be freed)
 */
void dll_clear(DLinkedList *list, free_func data_free);

//==============================================================================
// Information functions
//==============================================================================

/**
 * Get the number of elements in the list.
 *
 * @param list The list to query
 * @return Number of elements, or 0 if list is NULL
 */
size_t dll_size(const DLinkedList *list);

/**
 * Check if the list is empty.
 *
 * @param list The list to check
 * @return 1 if list is empty or NULL, 0 if list contains elements
 */
int dll_is_empty(const DLinkedList *list);

/**
 * Find the first node matching data using the comparison function.
 *
 * @param list The list to search
 * @param data The data to find
 * @param compare The comparison function to use
 * @return Pointer to matching node, or NULL if not found or on error
 */
DListNode *dll_find(DLinkedList *list, const void *data, cmp_func compare);

/**
 * Compare two lists for equality using the given comparison function.
 *
 * @param list1 First list to compare
 * @param list2 Second list to compare
 * @param compare Function to compare elements
 * @return 1 if lists are equal, 0 if not equal, -1 on error
 */
int dll_equals(const DLinkedList *list1, const DLinkedList *list2, cmp_func compare);

//==============================================================================
// Insertion functions
//==============================================================================

/**
 * Insert data at the front of the list.
 *
 * @param list The list to modify
 * @param data Pointer to the data to insert (ownership transferred to list)
 * @return 0 on success, -1 on error
 */
int dll_insert_front(DLinkedList *list, void *data);

/**
 * Insert data at the back of the list.
 *
 * @param list The list to modify
 * @param data Pointer to the data to insert (ownership transferred to list)
 * @return 0 on success, -1 on error
 */
int dll_insert_back(DLinkedList *list, void *data);

/**
 * Insert data at a specific position in the list.
 *
 * @param list The list to modify
 * @param pos Zero-based index where to insert (0 = front, size = back)
 * @param data Pointer to the data to insert (ownership transferred to list)
 * @return 0 on success, -1 on error (e.g., invalid position)
 */
int dll_insert_at(DLinkedList *list, size_t pos, void *data);

//==============================================================================
// Removal functions
//==============================================================================

/**
 * Remove the first node matching data using the comparison function.
 *
 * @param list The list to modify
 * @param data The data to match for removal
 * @param compare Function to compare elements
 * @param remove Function to free user data (can be NULL)
 * @return 0 on success, -1 if not found or on error
 */
int dll_remove(DLinkedList *list, const void *data, cmp_func compare, free_func remove);

/**
 * Remove node at a specific position.
 *
 * @param list The list to modify
 * @param pos Zero-based index of node to remove
 * @param remove Function to free user data (can be NULL)
 * @return 0 on success, -1 on error (e.g., invalid position)
 */
int dll_remove_at(DLinkedList *list, size_t pos, free_func remove);

/**
 * Remove the first node in the list.
 *
 * @param list The list to modify
 * @param remove Function to free user data (can be NULL)
 * @return 0 on success, -1 on error (e.g., empty list)
 */
int dll_remove_front(DLinkedList *list, free_func remove);

/**
 * Remove the last node in the list.
 *
 * @param list The list to modify
 * @param remove Function to free user data (can be NULL)
 * @return 0 on success, -1 on error (e.g., empty list)
 */
int dll_remove_back(DLinkedList *list, free_func remove);

//==============================================================================
// List manipulation functions
//==============================================================================

/**
 * Sort the list using merge sort algorithm.
 *
 * @param list The list to sort
 * @param compare Comparison function
 * @return 0 on success, -1 on error
 */
int dll_sort(DLinkedList *list, cmp_func compare);

/**
 * Reverse the order of nodes in the list.
 *
 * @param list The list to reverse
 * @return 0 on success, -1 on error
 */
int dll_reverse(DLinkedList *list);

/**
 * Merge src list into dest list (append src to the end of dest).
 * After merging, src will be empty but not destroyed.
 *
 * @param dest Destination list (will contain all elements)
 * @param src Source list (will be emptied)
 * @return 0 on success, -1 on error
 */
int dll_merge(DLinkedList *dest, DLinkedList *src);

/**
 * Splice src list into dest list at the specified position.
 * After splicing, src will be empty but not destroyed.
 *
 * @param dest Destination list
 * @param src Source list (will be emptied)
 * @param pos Position in dest where to insert src
 * @return 0 on success, -1 on error
 */
int dll_splice(DLinkedList *dest, DLinkedList *src, size_t pos);

//==============================================================================
// Higher-order functions
//==============================================================================

/**
 * Create a new list containing only elements that satisfy the predicate function.
 *
 * @param list The source list
 * @param pred Function that returns non-zero for elements to include
 * @return A new list with matching elements, or NULL on error
 */
DLinkedList *dll_filter(DLinkedList *list, pred_func pred);

/**
 * Create a new list by transforming each element with the given function.
 *
 * @param list The source list
 * @param transform Function that returns a new element based on the original
 * @return A new list with transformed elements, or NULL on error
 */
DLinkedList *dll_transform(DLinkedList *list, transform_func transform);

/**
 * Apply an action function to each element in the list.
 *
 * @param list The list to process
 * @param action Function applied to each element
 */
void dll_for_each(DLinkedList *list, action_func action);

//==============================================================================
// List copying functions
//==============================================================================

/**
 * Create a shallow copy of the list (sharing data pointers).
 *
 * @param list The list to copy
 * @return A new list with same structure but sharing data, or NULL on error
 */
DLinkedList *dll_copy(DLinkedList *list);

/**
 * Create a deep copy of the list (cloning data using the provided function).
 *
 * @param list The list to copy
 * @param copy_data Function to create copies of the data (required)
 * @return A new list with copies of all data, or NULL on error
 */
DLinkedList *dll_copy_deep(DLinkedList *list, copy_func copy_data);

/**
 * Create a new list from an iterator.
 *
 * @param it The source iterator (must be valid)
 * @param copy Optional function to copy data (if NULL, uses original pointers)
 * @return A new list with elements from iterator, or NULL on error
 */
DLinkedList *dll_from_iterator(Iterator *it, copy_func copy);

//==============================================================================
// Iterator functions
//==============================================================================

/**
 * Create an iterator for the list (head to tail traversal).
 *
 * @param list The list to iterate over
 * @return An Iterator object for forward traversal
 */
Iterator dll_iterator(DLinkedList *list);

/**
 * Create a reverse iterator for the list (tail to head traversal).
 *
 * @param list The list to iterate over
 * @return An Iterator object for backward traversal
 */
Iterator dll_iterator_reverse(DLinkedList *list);

#endif //DS_DLINKEDLIST_H