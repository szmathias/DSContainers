//
// Created by zack on 8/26/25.
//
// Iterator interface for generic data traversal.
// This implementation provides a flexible iterator pattern that can be
// used with any data structure, with support for transformation and filtering.

#ifndef DS_ITERATOR_H
#define DS_ITERATOR_H

//==============================================================================
// Iterator interface
//==============================================================================

/**
 * Generic iterator structure.
 * Provides a common interface for traversing different data structures.
 */
typedef struct Iterator Iterator;

/**
 * Iterator interface with function pointers for standard operations.
 */
struct Iterator
{
    void *data_state;              // Implementation-specific state data

    // Element access
    void *(*get)(const Iterator *it);  // Get current element without advancing

    // Forward iteration
    int (*has_next)(const Iterator *it);  // Check if more elements exist
    void *(*next)(const Iterator *it);    // Get next element and advance

    // Backward iteration
    int (*has_prev)(const Iterator *it);  // Check if previous elements exist
    void *(*prev)(const Iterator *it);    // Get previous element and move back

    // Control operations
    void (*reset)(const Iterator *it);    // Reset to starting position
    int (*is_valid)(const Iterator *it);  // Check if iterator is valid

    // Resource management
    void (*destroy)(Iterator *it);        // Free iterator resources
};

//==============================================================================
// Function pointer types
//==============================================================================

/**
 * Transformation function: creates a new element from an existing one.
 *
 * @param element The source element to transform
 * @return A new element derived from the source
 */
typedef void *(*transform_func)(const void *element);

/**
 * Filter predicate function: tests if elements should be included.
 *
 * @param element The element to test
 * @return Non-zero to include element, 0 to exclude
 */
typedef int (*filter_func)(const void *element);

//==============================================================================
// Higher-order iterator functions
//==============================================================================

/**
 * Create a transforming iterator that applies a function to each element.
 * The new iterator produces transformed elements on-the-fly.
 *
 * Note: The returned iterator takes ownership of the source iterator
 * and will destroy it when the transform iterator is destroyed.
 *
 * @param it Base iterator to transform (ownership transferred)
 * @param transform Function to apply to each element
 * @return A new iterator producing transformed elements
 */
Iterator iterator_transform(Iterator *it, transform_func transform);

/**
 * Create a filtering iterator that only yields elements matching a predicate.
 * The new iterator skips elements that don't satisfy the filter function.
 *
 * Note: The returned iterator takes ownership of the source iterator
 * and will destroy it when the filter iterator is destroyed.
 *
 * @param it Base iterator to filter (ownership transferred)
 * @param filter Predicate function that determines which elements to include
 * @return A new iterator yielding only elements that satisfy the predicate
 */
Iterator iterator_filter(Iterator *it, filter_func filter);

/**
 * Create an iterator that yields integers in a specified range.
 * Currently not implemented.
 */
// Iterator iterator_range(int start, int end, int step);

#endif //DS_ITERATOR_H