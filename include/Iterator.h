//
// Created by zack on 8/26/25.
//
// Iterator interface for generic data traversal.
// This implementation provides a flexible iterator pattern that can be
// used with any data structure, with support for transformation and filtering.

#ifndef DSCONTAINERS_ITERATOR_H
#define DSCONTAINERS_ITERATOR_H

#include "Alloc.h"
#include "CStandardCompatibility.h"

#ifdef __cplusplus
extern "C" {
#endif

//==============================================================================
// Iterator interface
//==============================================================================

/**
* Generic iterator structure.
* Provides a common interface for traversing different data structures.
*/
typedef struct DSCIterator DSCIterator;

/**
* Indexed element structure for enumerate iterator.
* Contains both the index/position and the element data.
*/
typedef struct DSCIndexedElement
{
    size_t index;         // Zero-based index/position
    void* element;        // Pointer to the element data
    DSCAllocator* alloc;  // Allocator for memory management
} DSCIndexedElement;

/**
* Iterator interface with function pointers for standard operations.
*/
struct DSCIterator
{
    void* data_state;          // Implementation-specific state data
    const DSCAllocator* alloc; // Allocator for memory management

    // Element access
    void*(*get)(const DSCIterator* it); // Get current element without advancing

    // Forward iteration
    int (*has_next)(const DSCIterator* it); // Check if more elements exist
    int (*next)(const DSCIterator* it);     // Advance to next position (returns 0 on success, -1 on failure)

    // Backward iteration
    int (*has_prev)(const DSCIterator* it); // Check if previous elements exist
    int (*prev)(const DSCIterator* it);     // Move back to previous position (returns 0 on success, -1 on failure)

    // Control operations
    void (*reset)(const DSCIterator* it);   // Reset to starting position
    int (*is_valid)(const DSCIterator* it); // Check if iterator is valid

    // Resource management
    void (*destroy)(DSCIterator* it); // Free iterator resources
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
typedef void*(*transform_func)(const void* element);

/**
* Filter predicate function: tests if elements should be included.
*
* @param element The element to test
* @return Non-zero to include element, 0 to exclude
*/
typedef int (*filter_func)(const void* element);

// Copy function provided in Alloc.h

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
* Transform functions should return non-allocated memory (static buffers,
* pointers to existing data, etc.) or handle their own memory management.
* If you need to collect transformed values, use dsc_iterator_copy().
*
* @param it Base iterator to transform (ownership transferred)
* @param alloc The allocator to use for the new iterator's state
* @param transform Function to apply to each element
* @param transform_allocates Flag: does transform function allocate memory
* @return A new iterator producing transformed elements
*/
DSC_API DSCIterator dsc_iterator_transform(DSCIterator* it, const DSCAllocator* alloc, transform_func transform, int transform_allocates);

/**
* Create a filtering iterator that only yields elements matching a predicate.
* The new iterator skips elements that don't satisfy the filter function.
*
* Note: The returned iterator takes ownership of the source iterator
* and will destroy it when the filter iterator is destroyed.
*
* @param it Base iterator to filter (ownership transferred)
* @param alloc The allocator to use for the new iterator's state.
* @param filter Predicate function that determines which elements to include
* @return A new iterator yielding only elements that satisfy the predicate
*/
DSC_API DSCIterator dsc_iterator_filter(DSCIterator* it, const DSCAllocator* alloc, filter_func filter);

/**
* Create an iterator that yields integers in a specified range.
*
* @param start Starting value (inclusive)
* @param end Ending value (exclusive)
* @param step Step value (positive or negative, non-zero)
* @param alloc The allocator to use for the new iterator's state.
* @return A new iterator yielding integers in the specified range
*/
DSC_API DSCIterator dsc_iterator_range(int start, int end, int step, const DSCAllocator* alloc);

/**
* Create a copy iterator that returns deep copies of elements.
*
* The copy iterator creates deep copies that the USER OWNS and must free.
* This is different from other iterators which return cached pointers.
*
* @param it Base iterator to copy from (ownership transferred)
* @param alloc The allocator to use for iterator state and element copies
* @param copy Function to create deep copies of elements
* @return A new iterator yielding user-owned copies
*/
DSC_API DSCIterator dsc_iterator_copy(DSCIterator* it, const DSCAllocator* alloc, copy_func copy);

/**
* Create a take iterator that limits iteration to first N elements.
* The new iterator will yield at most N elements from the base iterator.
*
* Note: The returned iterator takes ownership of the source iterator
* and will destroy it when the take iterator is destroyed.
*
* @param it Base iterator to take from (ownership transferred)
* @param alloc The allocator to use for the new iterator's state
* @param count Maximum number of elements to yield (0 means no elements)
* @return A new iterator yielding at most count elements
*/
DSC_API DSCIterator dsc_iterator_take(DSCIterator* it, const DSCAllocator* alloc, size_t count);

/**
* Create a skip iterator that skips first N elements, then yields the rest.
* The new iterator will skip the first N elements and then yield all remaining elements.
*
* Note: The returned iterator takes ownership of the source iterator
* and will destroy it when the skip iterator is destroyed.
*
* @param it Base iterator to skip from (ownership transferred)
* @param alloc The allocator to use for the new iterator's state
* @param count Number of elements to skip (0 means skip none)
* @return A new iterator yielding elements after skipping count elements
*/
DSC_API DSCIterator dsc_iterator_skip(DSCIterator* it, const DSCAllocator* alloc, size_t count);

/**
* Create a zip iterator that combines elements from two iterators pairwise.
* The new iterator yields DSCPair objects containing corresponding elements from both iterators.
* Iteration stops when either iterator is exhausted.
*
* Note: The returned iterator takes ownership of both source iterators
* and will destroy them when the zip iterator is destroyed.
*
* @param it1 First iterator (ownership transferred)
* @param it2 Second iterator (ownership transferred)
* @param alloc The allocator to use for the new iterator's state and pair creation
* @return A new iterator yielding DSCPair objects with elements from both iterators
*/
DSC_API DSCIterator dsc_iterator_zip(DSCIterator* it1, DSCIterator* it2, const DSCAllocator* alloc);

/**
* Create an enumerate iterator that adds index numbers to elements.
* The new iterator yields DSCIndexedElement objects containing both the zero-based
* index and the original element data.
*
* Note: The returned iterator takes ownership of the source iterator
* and will destroy it when the enumerate iterator is destroyed.
*
* @param it Base iterator to enumerate (ownership transferred)
* @param alloc The allocator to use for the new iterator's state
* @param start_index Starting index value (typically 0)
* @return A new iterator yielding DSCIndexedElement objects with index and data
*/
DSC_API DSCIterator dsc_iterator_enumerate(DSCIterator* it, const DSCAllocator* alloc, size_t start_index);

/**
* Create a repeat iterator that yields the same value N times.
* The iterator returns a pointer to the original value for each iteration,
* allowing the same data to be processed multiple times through iterator chains.
*
* Note: The returned iterator does not take ownership of the value pointer.
* The caller must ensure the value remains valid for the lifetime of the iterator.
* All calls to get() return the same pointer to the original value.
* Chain with copy iterator if unique instances are needed
*
* @param value Pointer to the value to repeat (must remain valid during iteration)
* @param alloc The allocator to use for the iterator's internal state
* @param count Number of times to repeat the value (0 means no elements)
* @return A new iterator yielding the same value count times
*/
DSC_API DSCIterator dsc_iterator_repeat(const void* value, const DSCAllocator* alloc, size_t count);

/**
* Create a chain iterator that combines multiple iterators sequentially.
* The new iterator yields all elements from the first iterator, then all elements
* from the second iterator, and so on. This allows appending datasets together.
*
* Note: The returned iterator takes ownership of all source iterators
* and will destroy them when the chain iterator is destroyed.
*
* @param iterators Array of iterators to chain together (ownership transferred)
* @param iterator_count Number of iterators in the array
* @param alloc The allocator to use for the iterator's internal state
* @return A new iterator yielding elements from all source iterators sequentially
*/
DSC_API DSCIterator dsc_iterator_chain(DSCIterator* iterators, size_t iterator_count, const DSCAllocator* alloc);

#ifdef __cplusplus
}
#endif

#endif //DSCONTAINERS_ITERATOR_H
