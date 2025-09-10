//
// Created by zack on 8/30/25.
//

#ifndef DSCONTAINERS_ALLOC_H
#define DSCONTAINERS_ALLOC_H

#include <stddef.h>
#include "PlatformDefs.h"

typedef struct DSCAlloc
{
    void* (*alloc_func)(size_t size);
    void (*dealloc_func)(void* ptr);
    void (*data_free_func)(void* ptr);
    void* (*copy_func)(const void* data);
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
typedef void* (*alloc_func)(size_t size);

/**
 * Memory deallocation function compatible with free.
 * Used for custom deallocation of nodes and list structures.
 *
 * @param ptr Pointer to memory to be freed
 */
typedef void (*dealloc_func)(void* ptr);

/**
 * Function to free user data stored in the list.
 * Used when destroying or removing nodes.
 *
 * @param ptr Pointer to user data to be freed
 */
typedef void (*data_free_func)(void* ptr);

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
typedef int (*cmp_func)(const void* a, const void* b);

/**
 * Copy function for deep copying element data.
 * Should return a pointer to a newly allocated copy of the data.
 *
 * @param data Pointer to original element data
 * @return Pointer to copied data (must be freed by caller)
 */
typedef void*(*copy_func)(const void* data);

//==============================================================================
// Utility functions for DSCAlloc
//==============================================================================

/**
 * Create a default allocator using standard library functions.
 * Uses malloc and free for allocation. The default copy function
 * just returns the pointer provided to it.
 *
 * @return DSCAlloc struct with default functions
 */
DSC_API DSCAlloc dsc_alloc_default(void);

/**
 * Create a custom allocator with user-provided functions.
 *
 * @param alloc_func Memory allocation function (required)
 * @param dealloc_func Memory deallocation function (required)
 * @param data_free_func User data cleanup function (can be NULL)
 * @param copy_func Data copying function (can be NULL)
 * @return DSCAlloc struct with custom functions
 */
DSC_API DSCAlloc dsc_alloc_custom(alloc_func alloc_func, dealloc_func dealloc_func,
                          data_free_func data_free_func, copy_func copy_func);

/**
 * Allocate memory using the allocator's allocation function.
 *
 * @param alloc Pointer to DSCAlloc struct
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory, or NULL on failure
 */
DSC_API void* dsc_alloc_malloc(const DSCAlloc* alloc, size_t size);

/**
 * Free memory using the allocator's deallocation function.
 *
 * @param alloc Pointer to DSCAlloc struct
 * @param ptr Pointer to memory to free
 */
DSC_API void dsc_alloc_free(const DSCAlloc* alloc, void* ptr);

/**
 * Free user data using the allocator's data free function.
 * Does nothing if data_free_func is NULL.
 *
 * @param alloc Pointer to DSCAlloc struct
 * @param ptr Pointer to user data to free
 */
DSC_API void dsc_alloc_data_free(const DSCAlloc* alloc, void* ptr);

/**
 * Copy data using the allocator's copy function.
 * Returns NULL if copy_func is NULL.
 *
 * @param alloc Pointer to DSCAlloc struct
 * @param data Pointer to data to copy
 * @return Pointer to copied data, or NULL if copy_func is NULL or on failure
 */
DSC_API void* dsc_alloc_copy(const DSCAlloc* alloc, const void* data);



#endif //DSCONTAINERS_ALLOC_H
