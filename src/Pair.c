//
// Created by zack on 9/15/25.
//

#include "Pair.h"

//==============================================================================
// Creation and destruction functions
//==============================================================================

DSCPair* dsc_pair_create(DSCAllocator* alloc, void* first, void* second)
{
    if (!alloc || !alloc->alloc_func || !alloc->dealloc_func)
    {
        return NULL;
    }

    DSCPair* pair = dsc_alloc_malloc(alloc, sizeof(DSCPair));
    if (!pair)
    {
        return NULL;
    }

    pair->first = first;
    pair->second = second;
    pair->alloc = alloc;

    return pair;
}

void dsc_pair_destroy(DSCPair* pair, const bool should_free_first, const bool should_free_second)
{
    if (!pair)
    {
        return;
    }

    if (should_free_first)
    {
        dsc_alloc_data_free(pair->alloc, pair->first);
    }

    if (should_free_second)
    {
        dsc_alloc_data_free(pair->alloc, pair->second);
    }

    dsc_alloc_free(pair->alloc, pair);
}

//==============================================================================
// Access functions
//==============================================================================

void* dsc_pair_first(const DSCPair* pair)
{
    return pair ? pair->first : NULL;
}

void* dsc_pair_second(const DSCPair* pair)
{
    return pair ? pair->second : NULL;
}

void dsc_pair_set_first(DSCPair* pair, void* first, const bool should_free_old)
{
    if (!pair)
    {
        return;
    }

    if (should_free_old)
    {
        dsc_alloc_data_free(pair->alloc, pair->first);
    }

    pair->first = first;
}

void dsc_pair_set_second(DSCPair* pair, void* second, const bool should_free_old)
{
    if (!pair)
    {
        return;
    }

    if (should_free_old)
    {
        dsc_alloc_data_free(pair->alloc, pair->second);
    }

    pair->second = second;
}

//==============================================================================
// Utility functions
//==============================================================================

void dsc_pair_swap(DSCPair* pair)
{
    if (!pair)
    {
        return;
    }

    void* temp = pair->first;
    pair->first = pair->second;
    pair->second = temp;
}

int dsc_pair_compare(const DSCPair* pair1, const DSCPair* pair2,
                     const pair_compare_func first_compare, const pair_compare_func second_compare)
{
    if (!pair1 && !pair2)
    {
        return 0;
    }

    if (!pair1)
    {
        return -1;
    }

    if (!pair2)
    {
        return 1;
    }

    // Compare first elements
    if (first_compare)
    {
        const int first_result = first_compare(pair1->first, pair2->first);
        if (first_result != 0)
        {
            return first_result;
        }
    }

    // First elements are equal, compare second elements
    if (second_compare)
    {
        return second_compare(pair1->second, pair2->second);
    }

    return 0;
}

int dsc_pair_equals(const DSCPair* pair1, const DSCPair* pair2,
                    const pair_compare_func first_compare, const pair_compare_func second_compare)
{
    return dsc_pair_compare(pair1, pair2, first_compare, second_compare) == 0;
}

DSCPair* dsc_pair_copy(const DSCPair* pair)
{
    if (!pair || !pair->alloc)
    {
        return NULL;
    }

    return dsc_pair_create(pair->alloc, pair->first, pair->second);
}

DSCPair* dsc_pair_copy_deep(const DSCPair* pair, const bool should_free, const copy_func first_copy, const copy_func second_copy)
{
    if (!pair || !pair->alloc)
    {
        return NULL;
    }

    DSCPair* new_pair = dsc_alloc_malloc(pair->alloc, sizeof(DSCPair));
    if (!new_pair)
    {
        return NULL;
    }
    new_pair->alloc = pair->alloc;

    // Copy first element (shallow if no copy function)
    if (pair->first)
    {
        new_pair->first = first_copy ? first_copy(pair->first) : pair->first;
        if (first_copy && !new_pair->first) // Only check for failure if copy function was used
        {
            dsc_pair_destroy(new_pair, false, false);
            return NULL;
        }
    }
    else
    {
        new_pair->first = NULL;
    }

    // Copy second element (shallow if no copy function)
    if (pair->second)
    {
        new_pair->second = second_copy ? second_copy(pair->second) : pair->second;
        if (second_copy && !new_pair->second) // Only check for failure if copy function was used
        {
            dsc_pair_destroy(new_pair, should_free, false);
            return NULL;
        }
    }
    else
    {
        new_pair->second = NULL;
    }

    return new_pair;
}