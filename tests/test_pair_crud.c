//
// Created by zack on 9/15/25.
//

#include "Pair.h"
#include "TestAssert.h"
#include "TestHelpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper copy functions for testing
static void* int_copy_func(const void* data)
{
    int* copy = malloc(sizeof(int));
    if (copy)
    {
        *copy = *(const int*)data;
    }
    return copy;
}

static void* string_copy_func(const void* data)
{
    const char* str = (const char*)data;
    size_t len = strlen(str) + 1;
    char* copy = malloc(len);
    if (copy)
    {
        strcpy(copy, str);
    }
    return copy;
}

int test_pair_copy_functions(void)
{
    DSCAllocator alloc = create_int_allocator();

    int* first = malloc(sizeof(int));
    int* second = malloc(sizeof(int));
    *first = 42;
    *second = 84;

    DSCPair* original = dsc_pair_create(&alloc, first, second);

    // Test shallow copy
    DSCPair* shallow = dsc_pair_copy(original);
    ASSERT_NOT_NULL(shallow);
    ASSERT_NOT_EQ_PTR(shallow, original);
    ASSERT_EQ_PTR(shallow->first, original->first);   // Same pointers
    ASSERT_EQ_PTR(shallow->second, original->second); // Same pointers
    ASSERT_EQ_PTR(shallow->alloc, original->alloc);

    // Test deep copy with both copy functions
    DSCPair* deep = dsc_pair_copy_deep(original, true, int_copy_func, int_copy_func);
    ASSERT_NOT_NULL(deep);
    ASSERT_NOT_EQ_PTR(deep, original);
    ASSERT_NOT_EQ_PTR(deep->first, original->first);         // Different pointers
    ASSERT_NOT_EQ_PTR(deep->second, original->second);       // Different pointers
    ASSERT_EQ(*(int*)deep->first, *(int*)original->first);   // Same values
    ASSERT_EQ(*(int*)deep->second, *(int*)original->second); // Same values

    // Test deep copy with only first copy function
    DSCPair* partial = dsc_pair_copy_deep(original, true, int_copy_func, NULL);
    ASSERT_NOT_NULL(partial);
    ASSERT_NOT_EQ_PTR(partial->first, original->first); // Copied
    ASSERT_EQ_PTR(partial->second, original->second);   // Referenced
    ASSERT_EQ(*(int*)partial->first, *(int*)original->first);

    // Test copy with NULL
    ASSERT_NULL(dsc_pair_copy(NULL));
    ASSERT_NULL(dsc_pair_copy_deep(NULL, true, int_copy_func, int_copy_func));

    dsc_pair_destroy(original, true, true);
    dsc_pair_destroy(shallow, false, false); // Don't free data (shared with original)
    dsc_pair_destroy(deep, true, true);
    dsc_pair_destroy(partial, true, false); // First copied, second referenced
    return TEST_SUCCESS;
}

int test_pair_mixed_type_copy(void)
{
    DSCAllocator alloc = create_int_allocator();

    int* first = malloc(sizeof(int));
    char* second = malloc(10);
    *first = 42;
    strcpy(second, "hello");

    DSCPair* original = dsc_pair_create(&alloc, first, second);

    // Test deep copy with different copy functions for each element
    DSCPair* mixed_copy = dsc_pair_copy_deep(original, true, int_copy_func, string_copy_func);
    ASSERT_NOT_NULL(mixed_copy);
    ASSERT_NOT_EQ_PTR(mixed_copy->first, original->first);
    ASSERT_NOT_EQ_PTR(mixed_copy->second, original->second);
    ASSERT_EQ(*(int*)mixed_copy->first, 42);
    ASSERT_EQ(strcmp((char*)mixed_copy->second, "hello"), 0);

    dsc_pair_destroy(original, true, true);
    dsc_pair_destroy(mixed_copy, true, true);
    return TEST_SUCCESS;
}

int test_pair_create_destroy(void)
{
    DSCAllocator alloc = create_int_allocator();

    int* first = malloc(sizeof(int));
    int* second = malloc(sizeof(int));
    *first = 42;
    *second = 84;

    DSCPair* pair = dsc_pair_create(&alloc, first, second);
    ASSERT_NOT_NULL(pair);
    ASSERT_EQ_PTR(pair->first, first);
    ASSERT_EQ_PTR(pair->second, second);
    ASSERT_EQ_PTR(pair->alloc, &alloc);

    dsc_pair_destroy(pair, true, true);
    return TEST_SUCCESS;
}

int test_pair_create_with_null_elements(void)
{
    DSCAllocator alloc = create_int_allocator();

    int* first = malloc(sizeof(int));
    *first = 42;

    // Test with second element NULL
    DSCPair* pair1 = dsc_pair_create(&alloc, first, NULL);
    ASSERT_NOT_NULL(pair1);
    ASSERT_NOT_NULL(pair1->first);
    ASSERT_NULL(pair1->second);
    ASSERT_EQ(*(int*)pair1->first, 42);

    // Test with first element NULL
    int* second = malloc(sizeof(int));
    *second = 84;
    DSCPair* pair2 = dsc_pair_create(&alloc, NULL, second);
    ASSERT_NOT_NULL(pair2);
    ASSERT_NULL(pair2->first);
    ASSERT_NOT_NULL(pair2->second);
    ASSERT_EQ(*(int*)pair2->second, 84);

    // Test with both NULL
    DSCPair* pair3 = dsc_pair_create(&alloc, NULL, NULL);
    ASSERT_NOT_NULL(pair3);
    ASSERT_NULL(pair3->first);
    ASSERT_NULL(pair3->second);

    dsc_pair_destroy(pair1, true, false);
    dsc_pair_destroy(pair2, false, true);
    dsc_pair_destroy(pair3, false, false);
    return TEST_SUCCESS;
}

int test_pair_create_invalid_allocator(void)
{
    // Test with NULL allocator
    DSCPair* pair1 = dsc_pair_create(NULL, NULL, NULL);
    ASSERT_NULL(pair1);

    // Test with incomplete allocator
    DSCAllocator incomplete_alloc = {0};
    DSCPair* pair2 = dsc_pair_create(&incomplete_alloc, NULL, NULL);
    ASSERT_NULL(pair2);

    return TEST_SUCCESS;
}

int test_pair_accessors(void)
{
    DSCAllocator alloc = create_int_allocator();

    int* first = malloc(sizeof(int));
    int* second = malloc(sizeof(int));
    *first = 42;
    *second = 84;

    DSCPair* pair = dsc_pair_create(&alloc, first, second);

    // Test getters
    void* get_first = dsc_pair_first(pair);
    void* get_second = dsc_pair_second(pair);
    ASSERT_EQ_PTR(get_first, first);
    ASSERT_EQ_PTR(get_second, second);
    ASSERT_EQ(*(int*)get_first, 42);
    ASSERT_EQ(*(int*)get_second, 84);

    // Test getters with NULL pair
    ASSERT_NULL(dsc_pair_first(NULL));
    ASSERT_NULL(dsc_pair_second(NULL));

    dsc_pair_destroy(pair, true, true);
    return TEST_SUCCESS;
}

int test_pair_setters(void)
{
    DSCAllocator alloc = create_int_allocator();

    int* first = malloc(sizeof(int));
    int* second = malloc(sizeof(int));
    int* new_first = malloc(sizeof(int));
    int* new_second = malloc(sizeof(int));
    *first = 42;
    *second = 84;
    *new_first = 100;
    *new_second = 200;

    DSCPair* pair = dsc_pair_create(&alloc, first, second);

    // Test set first without freeing old
    dsc_pair_set_first(pair, new_first, false);
    ASSERT_EQ_PTR(pair->first, new_first);
    ASSERT_EQ(*(int*)pair->first, 100);

    // Test set second with freeing old
    dsc_pair_set_second(pair, new_second, true);
    ASSERT_EQ_PTR(pair->second, new_second);
    ASSERT_EQ(*(int*)pair->second, 200);

    // Test setters with NULL pair
    dsc_pair_set_first(NULL, NULL, false);
    dsc_pair_set_second(NULL, NULL, false);

    // Clean up (first wasn't freed by set_first, so free manually)
    free(first);
    dsc_pair_destroy(pair, true, true);
    return TEST_SUCCESS;
}

int test_pair_swap(void)
{
    DSCAllocator alloc = create_int_allocator();

    int* first = malloc(sizeof(int));
    int* second = malloc(sizeof(int));
    *first = 42;
    *second = 84;

    DSCPair* pair = dsc_pair_create(&alloc, first, second);

    // Swap elements
    dsc_pair_swap(pair);
    ASSERT_EQ_PTR(pair->first, second);
    ASSERT_EQ_PTR(pair->second, first);
    ASSERT_EQ(*(int*)pair->first, 84);
    ASSERT_EQ(*(int*)pair->second, 42);

    // Test swap with NULL pair
    dsc_pair_swap(NULL);

    dsc_pair_destroy(pair, true, true);
    return TEST_SUCCESS;
}

int main(void)
{
    printf("Running Pair CRUD tests...\n");

    int (*tests[])(void) = {
        test_pair_create_destroy,
        test_pair_create_with_null_elements,
        test_pair_create_invalid_allocator,
        test_pair_accessors,
        test_pair_setters,
        test_pair_swap,
        test_pair_copy_functions,
        test_pair_mixed_type_copy,
    };

    const char* test_names[] = {
        "test_pair_create_destroy",
        "test_pair_create_with_null_elements",
        "test_pair_create_invalid_allocator",
        "test_pair_accessors",
        "test_pair_setters",
        "test_pair_swap",
        "test_pair_copy_functions",
        "test_pair_mixed_type_copy",
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++)
    {
        printf("Running %s... ", test_names[i]);
        if (tests[i]() == TEST_SUCCESS)
        {
            printf("PASSED\n");
            passed++;
        }
        else
        {
            printf("FAILED\n");
        }
    }

    printf("Pair CRUD tests: %d/%d passed\n", passed, num_tests);
    return (passed == num_tests) ? 0 : 1;
}