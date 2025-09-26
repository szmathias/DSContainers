//
// Created by zack on 9/10/25.
//

#include <stdlib.h>

#include "Allocator.h"

//==============================================================================
// Default copy function for simple data types
//==============================================================================

/**
 * Default copy function that simply returns the original pointer.
 * This is used when no custom copy function is provided.
 */
static void* default_copy(const void* data)
{
    return (void*)data; // Shallow copy - just return the pointer
}

//==============================================================================
// Utility function implementations
//==============================================================================

ANV_API ANVAllocator anv_alloc_default(void)
{
    const ANVAllocator alloc = {
        .allocate = malloc,
        .deallocate = free,
        .data_free = free,
        .copy = default_copy
    };
    return alloc;
}

ANV_API ANVAllocator anv_alloc_custom(const alloc_func alloc_func, const dealloc_func dealloc_func,
                              const data_free_func data_free_func, const copy_func copy_func)
{
    const ANVAllocator alloc = {
        .allocate = alloc_func,
        .deallocate = dealloc_func,
        .data_free = data_free_func,
        .copy = copy_func ? copy_func : default_copy
    };
    return alloc;
}

ANV_API void* anv_alloc_malloc(const ANVAllocator* alloc, const size_t size)
{
    if (!alloc || !alloc->allocate)
    {
        return NULL;
    }
    return alloc->allocate(size);
}

ANV_API void anv_alloc_free(const ANVAllocator* alloc, void* ptr)
{
    if (alloc && alloc->deallocate && ptr)
    {
        alloc->deallocate(ptr);
    }
}

ANV_API void anv_alloc_data_free(const ANVAllocator* alloc, void* ptr)
{
    if (alloc && alloc->data_free && ptr)
    {
        alloc->data_free(ptr);
    }
}

ANV_API void* anv_alloc_copy(const ANVAllocator* alloc, const void* data)
{
    if (!alloc || !alloc->copy || !data)
    {
        return NULL;
    }
    return alloc->copy(data);
}