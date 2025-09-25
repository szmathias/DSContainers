//
// Created by zack on 9/19/25.
//
// Binary Search Tree interface for generic data storage.
// This implementation provides a comprehensive set of operations
// for a binary search tree with support for custom memory allocation.

#ifndef DSCONTAINERS_BINARYSEARCHTREE_H
#define DSCONTAINERS_BINARYSEARCHTREE_H

#include "Alloc.h"
#include "CStandardCompatibility.h"
#include "Iterator.h"

#ifdef __cplusplus
extern "C" {
#endif

//==============================================================================
// Type definitions
//==============================================================================

// Node of binary search tree
typedef struct DSCBinarySearchTreeNode
{
    void* data;                            // Pointer to user data
    struct DSCBinarySearchTreeNode* left;  // Pointer to left child
    struct DSCBinarySearchTreeNode* right; // Pointer to right child
    struct DSCBinarySearchTreeNode* parent; // Pointer to parent node
} DSCBinarySearchTreeNode;

// Binary search tree structure with custom allocator support
typedef struct DSCBinarySearchTree
{
    DSCBinarySearchTreeNode* root; // Pointer to root node
    size_t size;                   // Number of nodes in tree
    cmp_func compare;              // Comparison function for ordering
    DSCAllocator* alloc;           // Custom allocator
} DSCBinarySearchTree;

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
 * Used in traversal operations.
 *
 * @param data Pointer to element data
 */
typedef void (*action_func)(void* data);

//==============================================================================
// Creation and destruction functions
//==============================================================================

/**
 * Create a new, empty binary search tree with custom allocator.
 *
 * @param alloc Custom allocator (can be NULL for default)
 * @param compare Comparison function for ordering elements
 * @return Pointer to new BinarySearchTree, or NULL on failure
 */
DSC_API DSCBinarySearchTree* dsc_bst_create(DSCAllocator* alloc, cmp_func compare);

/**
 * Destroy the tree and free all nodes.
 *
 * @param tree The tree to destroy
 * @param should_free_data Whether to free the data stored in nodes
 */
DSC_API void dsc_bst_destroy(DSCBinarySearchTree* tree, bool should_free_data);

/**
 * Clear all nodes from the tree, but keep the tree structure intact.
 *
 * @param tree The tree to clear
 * @param should_free_data Whether to free the data stored in nodes
 */
DSC_API void dsc_bst_clear(DSCBinarySearchTree* tree, bool should_free_data);

//==============================================================================
// Information functions
//==============================================================================

/**
 * Get the number of elements in the tree.
 *
 * @param tree The tree to query
 * @return Number of elements, or 0 if tree is NULL
 */
DSC_API size_t dsc_bst_size(const DSCBinarySearchTree* tree);

/**
 * Check if the tree is empty.
 *
 * @param tree The tree to check
 * @return 1 if tree is empty or NULL, 0 if tree contains elements
 */
DSC_API int dsc_bst_is_empty(const DSCBinarySearchTree* tree);

/**
 * Get the height of the tree.
 *
 * @param tree The tree to query
 * @return Height of the tree (0 for empty tree, 1 for root only)
 */
DSC_API size_t dsc_bst_height(const DSCBinarySearchTree* tree);

/**
 * Check if a value exists in the tree.
 *
 * @param tree The tree to search
 * @param data The data to search for
 * @return 1 if found, 0 if not found or on error
 */
DSC_API int dsc_bst_contains(const DSCBinarySearchTree* tree, const void* data);

/**
 * Find the minimum element in the tree.
 *
 * @param tree The tree to search
 * @return Pointer to minimum data, or NULL if tree is empty
 */
DSC_API void* dsc_bst_min(const DSCBinarySearchTree* tree);

/**
 * Find the maximum element in the tree.
 *
 * @param tree The tree to search
 * @return Pointer to maximum data, or NULL if tree is empty
 */
DSC_API void* dsc_bst_max(const DSCBinarySearchTree* tree);

//==============================================================================
// Insertion and removal functions
//==============================================================================

/**
 * Insert data into the tree.
 *
 * @param tree The tree to modify
 * @param data Pointer to the data to insert (ownership transferred to tree)
 * @return 0 on success, -1 on error, 1 if duplicate exists
 */
DSC_API int dsc_bst_insert(DSCBinarySearchTree* tree, void* data);

/**
 * Remove the first occurrence of data from the tree.
 *
 * @param tree The tree to modify
 * @param data The data to remove
 * @param should_free_data Whether to free the data
 * @return 0 on success, -1 if not found or on error
 */
DSC_API int dsc_bst_remove(DSCBinarySearchTree* tree, const void* data, bool should_free_data);

//==============================================================================
// Traversal functions
//==============================================================================

/**
 * Perform in-order traversal of the tree.
 * Visits nodes in sorted order.
 *
 * @param tree The tree to traverse
 * @param action Function to call for each element
 */
DSC_API void dsc_bst_inorder(const DSCBinarySearchTree* tree, action_func action);

/**
 * Perform pre-order traversal of the tree.
 * Visits root before children.
 *
 * @param tree The tree to traverse
 * @param action Function to call for each element
 */
DSC_API void dsc_bst_preorder(const DSCBinarySearchTree* tree, action_func action);

/**
 * Perform post-order traversal of the tree.
 * Visits children before root.
 *
 * @param tree The tree to traverse
 * @param action Function to call for each element
 */
DSC_API void dsc_bst_postorder(const DSCBinarySearchTree* tree, action_func action);

//==============================================================================
// Iterator support
//==============================================================================

/**
 * Create an iterator for in-order traversal of the tree.
 * Visits nodes in sorted order.
 *
 * @param tree The tree to iterate over
 * @return Iterator for in-order traversal
 */
DSC_API DSCIterator dsc_bst_iterator(const DSCBinarySearchTree* tree);

/**
 * Create an iterator for pre-order traversal of the tree.
 * Visits root before children.
 *
 * @param tree The tree to iterate over
 * @return Iterator for pre-order traversal
 */
DSC_API DSCIterator dsc_bst_iterator_preorder(const DSCBinarySearchTree* tree);

/**
 * Create an iterator for post-order traversal of the tree.
 * Visits children before root.
 *
 * @param tree The tree to iterate over
 * @return Iterator for post-order traversal
 */
DSC_API DSCIterator dsc_bst_iterator_postorder(const DSCBinarySearchTree* tree);

/**
 * Create a new binary search tree from an iterator.
 * Elements from the iterator will be inserted in the order they appear.
 *
 * @param it Iterator to read elements from
 * @param alloc Allocator for the new tree
 * @param compare Comparison function for the new tree
 * @param should_copy Whether to copy the data from iterator elements
 * @return New tree containing elements from iterator, or NULL on failure
 */
DSC_API DSCBinarySearchTree* dsc_bst_from_iterator(DSCIterator* it, DSCAllocator* alloc, cmp_func compare, bool should_copy);

#ifdef __cplusplus
}
#endif

#endif //DSCONTAINERS_BINARYSEARCHTREE_H