//
// Created by zack on 9/15/25.
//

#ifndef DSCONTAINERS_PAIR_H
#define DSCONTAINERS_PAIR_H

#include "Alloc.h"
#include "CStandardCompatibility.h"
#include "PlatformDefs.h"

#ifdef __cplusplus
extern "C" {

#endif

//==============================================================================
// Type definitions
//==============================================================================

/**
 * Generic pair structure that holds two values of any type.
 * Provides a simple way to group two related values together.
 * The pair takes ownership of the pointers and can optionally free them.
 */
typedef struct DSCPair
{
    void* first;         // Pointer to first element data
    void* second;        // Pointer to second element data
    DSCAllocator* alloc; // Custom allocator
} DSCPair;

/**
 * Comparison function for pair elements.
 * Should return:
 *   <0 if a < b,
 *    0 if a == b,
 *   >0 if a > b
 *
 * @param a Pointer to first element
 * @param b Pointer to second element
 * @return Comparison result
 */
typedef int (*pair_compare_func)(const void* a, const void* b);

//==============================================================================
// Creation and destruction functions
//==============================================================================

/**
 * Create a new pair with custom allocator.
 * The pair takes ownership of the provided pointers.
 *
 * @param alloc Custom allocator (required)
 * @param first Pointer to first element data (pair takes ownership)
 * @param second Pointer to second element data (pair takes ownership)
 * @return Pointer to new pair, or NULL on failure
 */
DSC_API DSCPair* dsc_pair_create(DSCAllocator* alloc, void* first, void* second);

/**
* Initialize a DSCPair structure with the given elements (no allocation).
* The pair takes ownership of the provided pointers but is not itself allocated.
* Use this when you have a stack-allocated DSCPair structure.
*
* @param pair Pointer to existing DSCPair structure
* @param alloc Custom allocator (can be NULL for simple cases)
* @param first Pointer to first element data
* @param second Pointer to second element data
* @return 0 on success, -1 if pair is NULL
*/
DSC_API int dsc_pair_init(DSCPair* pair, DSCAllocator* alloc, void* first, void* second);

/**
 * Destroy the pair and optionally free the contained data.
 *
 * @param pair The pair to destroy
 * @param should_free_first Whether to free first element data using alloc->data_free_func
 * @param should_free_second Whether to free second element data using alloc->data_free_func
 */
DSC_API void dsc_pair_destroy(DSCPair* pair, bool should_free_first, bool should_free_second);

//==============================================================================
// Access functions
//==============================================================================

/**
 * Get the first element of the pair.
 *
 * @param pair The pair
 * @return Pointer to first element data, or NULL if pair is NULL
 */
DSC_API void* dsc_pair_first(const DSCPair* pair);

/**
 * Get the second element of the pair.
 *
 * @param pair The pair
 * @return Pointer to second element data, or NULL if pair is NULL
 */
DSC_API void* dsc_pair_second(const DSCPair* pair);

/**
 * Set the first element of the pair.
 * The pair takes ownership of the new pointer.
 *
 * @param pair The pair
 * @param first New first element data (pair takes ownership)
 * @param should_free_old Whether to free the old first element using alloc->data_free_func
 */
DSC_API void dsc_pair_set_first(DSCPair* pair, void* first, bool should_free_old);

/**
 * Set the second element of the pair.
 * The pair takes ownership of the new pointer.
 *
 * @param pair The pair
 * @param second New second element data (pair takes ownership)
 * @param should_free_old Whether to free the old second element using alloc->data_free_func
 */
DSC_API void dsc_pair_set_second(DSCPair* pair, void* second, bool should_free_old);

//==============================================================================
// Utility functions
//==============================================================================

/**
 * Swap the first and second elements of the pair.
 *
 * @param pair The pair
 */
DSC_API void dsc_pair_swap(DSCPair* pair);

/**
 * Compare two pairs lexicographically using provided comparison functions.
 * First compares the first elements, then the second elements if first are equal.
 *
 * @param pair1 First pair
 * @param pair2 Second pair
 * @param first_compare Comparison function for first elements
 * @param second_compare Comparison function for second elements
 * @return <0 if pair1 < pair2, 0 if equal, >0 if pair1 > pair2
 */
DSC_API int dsc_pair_compare(const DSCPair* pair1, const DSCPair* pair2,
                             pair_compare_func first_compare, pair_compare_func second_compare);

/**
 * Check if two pairs are equal using provided comparison functions.
 *
 * @param pair1 First pair
 * @param pair2 Second pair
 * @param first_compare Comparison function for first elements
 * @param second_compare Comparison function for second elements
 * @return 1 if pairs are equal, 0 otherwise
 */
DSC_API int dsc_pair_equals(const DSCPair* pair1, const DSCPair* pair2,
                            pair_compare_func first_compare, pair_compare_func second_compare);

/**
 * Create a shallow copy of a pair (copies pair structure but not data).
 *
 * @param pair The pair to copy
 * @return Pointer to new pair copy, or NULL on failure
 */
DSC_API DSCPair* dsc_pair_copy(const DSCPair* pair);

/**
 * Create a deep copy of a pair with copied data using separate copy functions.
 * This allows copying pairs with different data types for first and second elements.
 *
 * @param pair The pair to copy
 * @param should_free Whether to free first element if copy fails (for cleanup)
 * @param first_copy Copy function for first element (NULL to skip copying, just reference)
 * @param second_copy Copy function for second element (NULL to skip copying, just reference)
 * @return Pointer to new pair copy, or NULL on failure
 */
DSC_API DSCPair* dsc_pair_copy_deep(const DSCPair* pair, bool should_free, copy_func first_copy, copy_func second_copy);

//==============================================================================
// Common copy helper functions
//==============================================================================

/**
 * Copy function for pairs containing string first and int second elements.
 * Creates a deep copy of a DSCPair where first is a string and second is an int.
 *
 * @param pair_data Pointer to a DSCPair containing string first, int second
 * @return Pointer to newly allocated DSCPair copy, or NULL on failure
 */
DSC_API void* dsc_pair_copy_string_int(const void* pair_data);

/**
 * Copy function for pairs containing int first and string second elements.
 * Creates a deep copy of a DSCPair where first is an int and second is a string.
 *
 * @param pair_data Pointer to a DSCPair containing int first, string second
 * @return Pointer to newly allocated DSCPair copy, or NULL on failure
 */
DSC_API void* dsc_pair_copy_int_string(const void* pair_data);

/**
 * Copy function for pairs containing string first and string second elements.
 * Creates a deep copy of a DSCPair where both elements are strings.
 *
 * @param pair_data Pointer to a DSCPair containing string first, string second
 * @return Pointer to newly allocated DSCPair copy, or NULL on failure
 */
DSC_API void* dsc_pair_copy_string_string(const void* pair_data);

/**
 * Copy function for pairs containing int first and int second elements.
 * Creates a deep copy of a DSCPair where both elements are integers.
 *
 * @param pair_data Pointer to a DSCPair containing int first, int second
 * @return Pointer to newly allocated DSCPair copy, or NULL on failure
 */
DSC_API void* dsc_pair_copy_int_int(const void* pair_data);

#ifdef __cplusplus
}
#endif

#endif //DSCONTAINERS_PAIR_H
