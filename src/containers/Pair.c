//
// Created by zack on 9/15/25.
//

#include <string.h>

#include "Pair.h"

//==============================================================================
// Creation and destruction functions
//==============================================================================

ANVPair* anv_pair_create(ANVAllocator* alloc, void* first, void* second)
{
    if (!alloc || !alloc->allocate || !alloc->deallocate)
    {
        return NULL;
    }

    ANVPair* pair = anv_alloc_malloc(alloc, sizeof(ANVPair));
    if (!pair)
    {
        return NULL;
    }

    pair->first = first;
    pair->second = second;
    pair->alloc = alloc;

    return pair;
}

int anv_pair_init(ANVPair* pair, ANVAllocator* alloc, void* first, void* second)
{
    if (!pair)
    {
        return -1;
    }

    pair->first = first;
    pair->second = second;
    pair->alloc = alloc;

    return 0;
}

void anv_pair_destroy(ANVPair* pair, const bool should_free_first, const bool should_free_second)
{
    if (!pair)
    {
        return;
    }

    if (should_free_first)
    {
        anv_alloc_data_free(pair->alloc, pair->first);
    }

    if (should_free_second)
    {
        anv_alloc_data_free(pair->alloc, pair->second);
    }

    anv_alloc_free(pair->alloc, pair);
}

//==============================================================================
// Access functions
//==============================================================================

void* anv_pair_first(const ANVPair* pair)
{
    return pair ? pair->first : NULL;
}

void* anv_pair_second(const ANVPair* pair)
{
    return pair ? pair->second : NULL;
}

void anv_pair_set_first(ANVPair* pair, void* first, const bool should_free_old)
{
    if (!pair)
    {
        return;
    }

    if (should_free_old)
    {
        anv_alloc_data_free(pair->alloc, pair->first);
    }

    pair->first = first;
}

void anv_pair_set_second(ANVPair* pair, void* second, const bool should_free_old)
{
    if (!pair)
    {
        return;
    }

    if (should_free_old)
    {
        anv_alloc_data_free(pair->alloc, pair->second);
    }

    pair->second = second;
}

//==============================================================================
// Utility functions
//==============================================================================

void anv_pair_swap(ANVPair* pair)
{
    if (!pair)
    {
        return;
    }

    void* temp = pair->first;
    pair->first = pair->second;
    pair->second = temp;
}

int anv_pair_compare(const ANVPair* pair1, const ANVPair* pair2,
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

int anv_pair_equals(const ANVPair* pair1, const ANVPair* pair2,
                    const pair_compare_func first_compare, const pair_compare_func second_compare)
{
    return anv_pair_compare(pair1, pair2, first_compare, second_compare) == 0;
}

ANVPair* anv_pair_copy(const ANVPair* pair)
{
    if (!pair || !pair->alloc)
    {
        return NULL;
    }

    return anv_pair_create(pair->alloc, pair->first, pair->second);
}

ANVPair* anv_pair_copy_deep(const ANVPair* pair, const bool should_free, const copy_func first_copy, const copy_func second_copy)
{
    if (!pair || !pair->alloc)
    {
        return NULL;
    }

    ANVPair* new_pair = anv_alloc_malloc(pair->alloc, sizeof(ANVPair));
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
            anv_pair_destroy(new_pair, false, false);
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
            anv_pair_destroy(new_pair, should_free, false);
            return NULL;
        }
    }
    else
    {
        new_pair->second = NULL;
    }

    return new_pair;
}

//==============================================================================
// Common copy helper functions
//==============================================================================

void* anv_pair_copy_string_int(const void* pair_data)
{
    if (!pair_data)
    {
        return NULL;
    }

    const ANVPair* original = pair_data;
    if (!original->alloc)
    {
        return NULL;
    }

    // Create new pair structure
    ANVPair* new_pair = anv_alloc_malloc(original->alloc, sizeof(ANVPair));
    if (!new_pair)
    {
        return NULL;
    }

    new_pair->alloc = original->alloc;
    new_pair->first = NULL;
    new_pair->second = NULL;

    // Copy string first element
    if (original->first)
    {
        const char* str = original->first;
        const size_t len = strlen(str) + 1;
        char* str_copy = anv_alloc_malloc(original->alloc, len);
        if (!str_copy)
        {
            anv_alloc_free(original->alloc, new_pair);
            return NULL;
        }
        strcpy(str_copy, str);
        new_pair->first = str_copy;
    }

    // Copy int second element
    if (original->second)
    {
        int* int_copy = anv_alloc_malloc(original->alloc, sizeof(int));
        if (!int_copy)
        {
            if (new_pair->first)
            {
                anv_alloc_free(original->alloc, new_pair->first);
            }
            anv_alloc_free(original->alloc, new_pair);
            return NULL;
        }
        *int_copy = *(const int*)original->second;
        new_pair->second = int_copy;
    }

    return new_pair;
}

void* anv_pair_copy_int_string(const void* pair_data)
{
    if (!pair_data)
    {
        return NULL;
    }

    const ANVPair* original = pair_data;
    if (!original->alloc)
    {
        return NULL;
    }

    // Create new pair structure
    ANVPair* new_pair = anv_alloc_malloc(original->alloc, sizeof(ANVPair));
    if (!new_pair)
    {
        return NULL;
    }

    new_pair->alloc = original->alloc;
    new_pair->first = NULL;
    new_pair->second = NULL;

    // Copy int first element
    if (original->first)
    {
        int* int_copy = anv_alloc_malloc(original->alloc, sizeof(int));
        if (!int_copy)
        {
            anv_alloc_free(original->alloc, new_pair);
            return NULL;
        }
        *int_copy = *(const int*)original->first;
        new_pair->first = int_copy;
    }

    // Copy string second element
    if (original->second)
    {
        const char* str = (const char*)original->second;
        const size_t len = strlen(str) + 1;
        char* str_copy = anv_alloc_malloc(original->alloc, len);
        if (!str_copy)
        {
            if (new_pair->first)
            {
                anv_alloc_free(original->alloc, new_pair->first);
            }
            anv_alloc_free(original->alloc, new_pair);
            return NULL;
        }
        strcpy(str_copy, str);
        new_pair->second = str_copy;
    }

    return new_pair;
}

void* anv_pair_copy_string_string(const void* pair_data)
{
    if (!pair_data)
    {
        return NULL;
    }

    const ANVPair* original = pair_data;
    if (!original->alloc)
    {
        return NULL;
    }

    // Create new pair structure
    ANVPair* new_pair = anv_alloc_malloc(original->alloc, sizeof(ANVPair));
    if (!new_pair)
    {
        return NULL;
    }

    new_pair->alloc = original->alloc;
    new_pair->first = NULL;
    new_pair->second = NULL;

    // Copy string first element
    if (original->first)
    {
        const char* str1 = (const char*)original->first;
        const size_t len1 = strlen(str1) + 1;
        char* str1_copy = anv_alloc_malloc(original->alloc, len1);
        if (!str1_copy)
        {
            anv_alloc_free(original->alloc, new_pair);
            return NULL;
        }
        strcpy(str1_copy, str1);
        new_pair->first = str1_copy;
    }

    // Copy string second element
    if (original->second)
    {
        const char* str2 = (const char*)original->second;
        const size_t len2 = strlen(str2) + 1;
        char* str2_copy = anv_alloc_malloc(original->alloc, len2);
        if (!str2_copy)
        {
            if (new_pair->first)
            {
                anv_alloc_free(original->alloc, new_pair->first);
            }
            anv_alloc_free(original->alloc, new_pair);
            return NULL;
        }
        strcpy(str2_copy, str2);
        new_pair->second = str2_copy;
    }

    return new_pair;
}

void* anv_pair_copy_int_int(const void* pair_data)
{
    if (!pair_data)
    {
        return NULL;
    }

    const ANVPair* original = pair_data;
    if (!original->alloc)
    {
        return NULL;
    }

    // Create new pair structure
    ANVPair* new_pair = anv_alloc_malloc(original->alloc, sizeof(ANVPair));
    if (!new_pair)
    {
        return NULL;
    }

    new_pair->alloc = original->alloc;
    new_pair->first = NULL;
    new_pair->second = NULL;

    // Copy int first element
    if (original->first)
    {
        int* int1_copy = anv_alloc_malloc(original->alloc, sizeof(int));
        if (!int1_copy)
        {
            anv_alloc_free(original->alloc, new_pair);
            return NULL;
        }
        *int1_copy = *(const int*)original->first;
        new_pair->first = int1_copy;
    }

    // Copy int second element
    if (original->second)
    {
        int* int2_copy = anv_alloc_malloc(original->alloc, sizeof(int));
        if (!int2_copy)
        {
            if (new_pair->first)
            {
                anv_alloc_free(original->alloc, new_pair->first);
            }
            anv_alloc_free(original->alloc, new_pair);
            return NULL;
        }
        *int2_copy = *(const int*)original->second;
        new_pair->second = int2_copy;
    }

    return new_pair;
}
