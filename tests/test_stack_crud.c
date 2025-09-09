#include "TestAssert.h"
#include "TestHelpers.h"
#include "Stack.h"
#include <stdio.h>
#include <stdlib.h>

// Test basic stack creation and destruction
int test_stack_create_destroy(void)
{
    DSCAlloc* alloc = create_std_allocator();
    ASSERT_NOT_NULL(alloc);

    DSCStack* stack = dsc_stack_create(alloc);
    ASSERT_NOT_NULL(stack);
    ASSERT_EQ(dsc_stack_size(stack), 0);
    ASSERT(dsc_stack_is_empty(stack));

    dsc_stack_destroy(stack, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test NULL parameter handling
int test_stack_null_parameters(void)
{
    // Creating with NULL allocator should fail
    ASSERT_NULL(dsc_stack_create(NULL));

    // Operations on NULL stack should be safe
    ASSERT_EQ(dsc_stack_size(NULL), 0);
    ASSERT(dsc_stack_is_empty(NULL));
    ASSERT_NULL(dsc_stack_peek(NULL));
    ASSERT_NULL(dsc_stack_top(NULL));
    ASSERT_EQ(dsc_stack_push(NULL, NULL), -1);
    ASSERT_EQ(dsc_stack_pop(NULL, false), -1);
    ASSERT_NULL(dsc_stack_pop_data(NULL));

    // Destruction should be safe
    dsc_stack_destroy(NULL, false);
    dsc_stack_clear(NULL, false);

    return TEST_SUCCESS;
}

// Test basic push and pop operations
int test_stack_push_pop(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCStack* stack = dsc_stack_create(alloc);

    int* data1 = malloc(sizeof(int));
    int* data2 = malloc(sizeof(int));
    int* data3 = malloc(sizeof(int));
    *data1     = 10;
    *data2     = 20;
    *data3     = 30;

    // Test pushing elements
    ASSERT_EQ(dsc_stack_push(stack, data1), 0);
    ASSERT_EQ(dsc_stack_size(stack), 1);
    ASSERT(!dsc_stack_is_empty(stack));
    ASSERT_EQ(*(int*)dsc_stack_peek(stack), 10);

    ASSERT_EQ(dsc_stack_push(stack, data2), 0);
    ASSERT_EQ(dsc_stack_size(stack), 2);
    ASSERT_EQ(*(int*)dsc_stack_peek(stack), 20); // LIFO - should see data2

    ASSERT_EQ(dsc_stack_push(stack, data3), 0);
    ASSERT_EQ(dsc_stack_size(stack), 3);
    ASSERT_EQ(*(int*)dsc_stack_peek(stack), 30); // LIFO - should see data3

    // Test popping elements
    ASSERT_EQ(dsc_stack_pop(stack, true), 0); // Pops data3 and frees it
    ASSERT_EQ(dsc_stack_size(stack), 2);
    ASSERT_EQ(*(int*)dsc_stack_peek(stack), 20); // Should see data2

    ASSERT_EQ(dsc_stack_pop(stack, true), 0); // Pops data2 and frees it
    ASSERT_EQ(dsc_stack_size(stack), 1);
    ASSERT_EQ(*(int*)dsc_stack_peek(stack), 10); // Should see data1

    ASSERT_EQ(dsc_stack_pop(stack, true), 0); // Pops data1 and frees it
    ASSERT_EQ(dsc_stack_size(stack), 0);
    ASSERT(dsc_stack_is_empty(stack));
    ASSERT_NULL(dsc_stack_peek(stack));

    // Test popping from empty stack
    ASSERT_EQ(dsc_stack_pop(stack, false), -1);

    dsc_stack_destroy(stack, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test pop_data function
int test_stack_pop_data(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCStack* stack = dsc_stack_create(alloc);

    int* data1 = malloc(sizeof(int));
    int* data2 = malloc(sizeof(int));
    *data1     = 42;
    *data2     = 84;

    ASSERT_EQ(dsc_stack_push(stack, data1), 0);
    ASSERT_EQ(dsc_stack_push(stack, data2), 0);

    // Pop data2 and get its value
    void* popped = dsc_stack_pop_data(stack);
    ASSERT_NOT_NULL(popped);
    ASSERT_EQ(*(int*)popped, 84);
    ASSERT_EQ(dsc_stack_size(stack), 1);
    free(popped); // Caller's responsibility to free

    // Pop data1 and get its value
    popped = dsc_stack_pop_data(stack);
    ASSERT_NOT_NULL(popped);
    ASSERT_EQ(*(int*)popped, 42);
    ASSERT_EQ(dsc_stack_size(stack), 0);
    free(popped);

    // Pop from empty stack
    ASSERT_NULL(dsc_stack_pop_data(stack));

    dsc_stack_destroy(stack, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test stack clear operation
int test_stack_clear(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCStack* stack = dsc_stack_create(alloc);

    // Add some elements
    for (int i = 0; i < 5; i++)
    {
        int* data = malloc(sizeof(int));
        *data     = i * 10;
        ASSERT_EQ(dsc_stack_push(stack, data), 0);
    }

    ASSERT_EQ(dsc_stack_size(stack), 5);

    // Clear with freeing data
    dsc_stack_clear(stack, true);
    ASSERT_EQ(dsc_stack_size(stack), 0);
    ASSERT(dsc_stack_is_empty(stack));
    ASSERT_NULL(dsc_stack_peek(stack));

    // Stack should still be usable after clear
    int* new_data = malloc(sizeof(int));
    *new_data     = 999;
    ASSERT_EQ(dsc_stack_push(stack, new_data), 0);
    ASSERT_EQ(dsc_stack_size(stack), 1);
    ASSERT_EQ(*(int*)dsc_stack_peek(stack), 999);

    dsc_stack_destroy(stack, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test stack equality
int test_stack_equals(void)
{
    DSCAlloc* alloc  = create_std_allocator();
    DSCStack* stack1 = dsc_stack_create(alloc);
    DSCStack* stack2 = dsc_stack_create(alloc);

    // Empty stacks should be equal
    ASSERT_EQ(dsc_stack_equals(stack1, stack2, int_cmp), 1);

    // Same stack should be equal to itself
    ASSERT_EQ(dsc_stack_equals(stack1, stack1, int_cmp), 1);

    // Add same elements to both stacks
    for (int i = 0; i < 3; i++)
    {
        int* data1 = malloc(sizeof(int));
        int* data2 = malloc(sizeof(int));
        *data1     = *data2 = i * 10;
        ASSERT_EQ(dsc_stack_push(stack1, data1), 0);
        ASSERT_EQ(dsc_stack_push(stack2, data2), 0);
    }

    ASSERT_EQ(dsc_stack_equals(stack1, stack2, int_cmp), 1);

    // Add different element to one stack
    int* diff_data = malloc(sizeof(int));
    *diff_data     = 999;
    ASSERT_EQ(dsc_stack_push(stack1, diff_data), 0);

    ASSERT_EQ(dsc_stack_equals(stack1, stack2, int_cmp), 0);

    // Test with NULL parameters
    ASSERT_EQ(dsc_stack_equals(NULL, stack2, int_cmp), -1);
    ASSERT_EQ(dsc_stack_equals(stack1, NULL, int_cmp), -1);
    ASSERT_EQ(dsc_stack_equals(stack1, stack2, NULL), -1);

    dsc_stack_destroy(stack1, true);
    dsc_stack_destroy(stack2, true);
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
        {test_stack_create_destroy, "test_stack_create_destroy"},
        {test_stack_null_parameters, "test_stack_null_parameters"},
        {test_stack_push_pop, "test_stack_push_pop"},
        {test_stack_pop_data, "test_stack_pop_data"},
        {test_stack_clear, "test_stack_clear"},
        {test_stack_equals, "test_stack_equals"},
    };

    printf("Running Stack CRUD tests...\n");

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
        printf("All Stack CRUD tests passed!\n");
        return 0;
    }

    printf("%d Stack CRUD tests failed.\n", failed);
    return 1;
}
