//
// Created by zack on 9/9/25.
//

#include "TestAssert.h"
#include "TestHelpers.h"
#include "Stack.h"
#include <stdio.h>
#include <stdlib.h>

// Test stack with iterator
int test_stack_iterator(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCStack* stack = dsc_stack_create(alloc);

    // Add some test data
    for (int i = 0; i < 5; i++)
    {
        const int values[] = {10, 20, 30, 40, 50};
        int* data          = malloc(sizeof(int));
        *data              = values[i];
        ASSERT_EQ(dsc_stack_push(stack, data), 0);
    }

    // Create iterator
    DSCIterator it = dsc_stack_iterator(stack);
    ASSERT(it.is_valid(&it));

    // Iterate through stack (should be in LIFO order: 50, 40, 30, 20, 10)
    int index = 0;

    while (it.has_next(&it))
    {
        const int expected[] = {50, 40, 30, 20, 10};
        void* data           = it.next(&it);
        ASSERT_NOT_NULL(data);
        ASSERT_EQ(*(int*)data, expected[index]);
        index++;
    }
    ASSERT_EQ(index, 5);

    // Test reset functionality
    it.reset(&it);
    ASSERT(it.has_next(&it));
    void* first = it.next(&it);
    ASSERT_EQ(*(int*)first, 50); // Should be top element again

    // Test get without advancing
    it.reset(&it);
    void* peek_data = it.get(&it);
    ASSERT_EQ(*(int*)peek_data, 50);
    ASSERT(it.has_next(&it)); // Should still have next

    it.destroy(&it);
    dsc_stack_destroy(stack, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test creating stack from iterator
int test_stack_from_iterator(void)
{
    DSCAlloc* alloc = create_std_allocator();

    // Create a range iterator (0, 1, 2, 3, 4)
    DSCIterator range_it = dsc_iterator_range(0, 5, 1);

    // Create stack from iterator
    DSCStack* stack = dsc_stack_from_iterator(&range_it, alloc);
    ASSERT_NOT_NULL(stack);
    ASSERT_EQ(dsc_stack_size(stack), 5);

    // Clean up the iterator immediately after use
    range_it.destroy(&range_it);

    // Verify stack has correct values in LIFO order
    // Iterator gives 0,1,2,3,4 but stack should have them as 4,3,2,1,0 (top to bottom)
    for (int expected = 4; expected >= 0; expected--)
    {
        void* data = dsc_stack_pop_data(stack);
        ASSERT_NOT_NULL(data);
        ASSERT_EQ(*(int*)data, expected);
        free(data);
    }

    dsc_stack_destroy(stack, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test iterator with empty stack
int test_stack_iterator_empty(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCStack* stack = dsc_stack_create(alloc);

    DSCIterator it = dsc_stack_iterator(stack);
    ASSERT(it.is_valid(&it));
    ASSERT(!it.has_next(&it));
    ASSERT_NULL(it.get(&it));
    ASSERT_NULL(it.next(&it));

    it.destroy(&it);
    dsc_stack_destroy(stack, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test iterator validity with invalid stack
int test_stack_iterator_invalid(void)
{
    DSCIterator it = dsc_stack_iterator(NULL);
    ASSERT(!it.is_valid(&it));
    return TEST_SUCCESS;
}

// Test iterator state after stack modifications
int test_stack_iterator_modification(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCStack* stack = dsc_stack_create(alloc);

    // Add initial data
    for (int i = 0; i < 3; i++)
    {
        int* data = malloc(sizeof(int));
        *data     = i * 10;
        ASSERT_EQ(dsc_stack_push(stack, data), 0);
    }

    DSCIterator it = dsc_stack_iterator(stack);
    ASSERT(it.is_valid(&it));

    // Get first element
    void* first = it.next(&it);
    ASSERT_EQ(*(int*)first, 20); // Should be top element (2*10)

    // Modify stack while iterator exists (implementation detail: iterator may become invalid)
    int* new_data = malloc(sizeof(int));
    *new_data     = 999;
    ASSERT_EQ(dsc_stack_push(stack, new_data), 0);

    // Iterator should still be valid but may not reflect new state
    ASSERT(it.is_valid(&it));

    it.destroy(&it);
    dsc_stack_destroy(stack, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

typedef struct
{
    int (*func)(void);
    const char* name;
} TestCase;

int main(void)
{
    const TestCase tests[] = {
        {test_stack_iterator, "test_stack_iterator"},
        {test_stack_from_iterator, "test_stack_from_iterator"},
        {test_stack_iterator_empty, "test_stack_iterator_empty"},
        {test_stack_iterator_invalid, "test_stack_iterator_invalid"},
        {test_stack_iterator_modification, "test_stack_iterator_modification"},
    };

    printf("Running Stack iterator tests...\n");

    int failed          = 0;
    const int num_tests = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < num_tests; i++)
    {
        if (tests[i].func() != TEST_SUCCESS)
        {
            printf("%s failed\n", tests[i].name);
            failed++;
        }
    }

    if (failed == 0)
    {
        printf("All Stack iterator tests passed!\n");
        return 0;
    }

    printf("%d Stack iterator tests failed.\n", failed);
    return 1;
}
