//
// Created by zack on 9/10/25.
//

#include "Alloc.h"
#include <stdlib.h>

//==============================================================================
// Default copy function for simple data types
//==============================================================================

/**
 * Default copy function that simply returns the original pointer.
 * This is used when no custom copy function is provided.
 */
static void* default_copy_func(const void* data)
{
    return (void*)data; // Shallow copy - just return the pointer
}

//==============================================================================
// Utility function implementations
//==============================================================================

DSCAllocator dsc_alloc_default(void)
{
    const DSCAllocator alloc = {
        .alloc_func = malloc,
        .dealloc_func = free,
        .data_free_func = free,
        .copy_func = default_copy_func
    };
    return alloc;
}

DSCAllocator dsc_alloc_custom(const alloc_func alloc_func, const dealloc_func dealloc_func,
                          const data_free_func data_free_func, const copy_func copy_func)
{
    DSCAllocator alloc = {
        .alloc_func = alloc_func,
        .dealloc_func = dealloc_func,
        .data_free_func = data_free_func,
        .copy_func = copy_func ? copy_func : default_copy_func
    };
    return alloc;
}

void* dsc_alloc_malloc(const DSCAllocator* alloc, const size_t size)
{
    if (!alloc || !alloc->alloc_func)
    {
        return NULL;
    }
    return alloc->alloc_func(size);
}

void dsc_alloc_free(const DSCAllocator* alloc, void* ptr)
{
    if (alloc && alloc->dealloc_func && ptr)
    {
        alloc->dealloc_func(ptr);
    }
}

void dsc_alloc_data_free(const DSCAllocator* alloc, void* ptr)
{
    if (alloc && alloc->data_free_func && ptr)
    {
        alloc->data_free_func(ptr);
    }
}

void* dsc_alloc_copy(const DSCAllocator* alloc, const void* data)
{
    if (!alloc || !alloc->copy_func || !data)
    {
        return NULL;
    }
    return alloc->copy_func(data);
}
