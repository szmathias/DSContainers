//
// Created by zack on 8/30/25.
//

#ifndef DSCONTAINERS_ALLOC_H
#define DSCONTAINERS_ALLOC_H

#include <stddef.h>

typedef struct DSCAlloc
{
    void * (*alloc_func)(size_t size);
    void   (*dealloc_func)(void *ptr);
    void   (*data_free_func)(void *ptr);
    void * (*copy_func)(const void *data);
} DSCAlloc;

//==============================================================================
// Function pointer types
//==============================================================================

/**
 * Memory allocation function compatible with malloc.
 * Used for custom allocation of nodes and list structures.
 *
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory, or NULL on failure
 */
typedef void * (*alloc_func)(size_t size);

/**
 * Memory deallocation function compatible with free.
 * Used for custom deallocation of nodes and list structures.
 *
 * @param ptr Pointer to memory to be freed
 */
typedef void (*dealloc_func)(void *ptr);

/**
 * Function to free user data stored in the list.
 * Used when destroying or removing nodes.
 *
 * @param ptr Pointer to user data to be freed
 */
typedef void (*data_free_func)(void *ptr);

/**
 * Comparison function for list elements.
 * Should return:
 *   <0 if a < b,
 *    0 if a == b,
 *   >0 if a > b.
 *
 * Used for searching, sorting, and equality checks.
 *
 * @param a Pointer to first element
 * @param b Pointer to second element
 * @return Integer indicating comparison result
 */
typedef int (*cmp_func)(const void *a, const void *b);

/**
 * Copy function for deep copying element data.
 * Should return a pointer to a newly allocated copy of the data.
 *
 * @param data Pointer to original element data
 * @return Pointer to copied data (must be freed by caller)
 */
typedef void *(*copy_func)(const void *data);

#endif //DSCONTAINERS_ALLOC_H