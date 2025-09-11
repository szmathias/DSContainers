//
// Created by zack on 9/9/25.
//

#include "TestAssert.h"
#include "TestHelpers.h"
#include "Queue.h"
#include <stdio.h>
#include <stdlib.h>

// Test queue with iterator
int test_queue_iterator(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCQueue* queue = dsc_queue_create(&alloc);

    // Add some test data
    for (int i = 0; i < 5; i++)
    {
        const int values[] = {10, 20, 30, 40, 50};
        int* data          = malloc(sizeof(int));
        *data              = values[i];
        ASSERT_EQ(dsc_queue_enqueue(queue, data), 0);
    }

    // Create iterator
    DSCIterator it = dsc_queue_iterator(queue);
    ASSERT(it.is_valid(&it));

    // Iterate through queue (should be in FIFO order: 10, 20, 30, 40, 50)
    int index = 0;

    while (it.has_next(&it))
    {
        const int expected[] = {10, 20, 30, 40, 50};
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
    ASSERT_EQ(*(int*)first, 10); // Should be front element again

    // Test get without advancing
    it.reset(&it);
    void* peek_data = it.get(&it);
    ASSERT_EQ(*(int*)peek_data, 10);
    ASSERT(it.has_next(&it)); // Should still have next

    it.destroy(&it);
    dsc_queue_destroy(queue, true);
    return TEST_SUCCESS;
}

// Test creating queue from iterator
int test_queue_from_iterator(void)
{
    DSCAllocator alloc = create_int_allocator();

    // Create a range iterator (0, 1, 2, 3, 4)
    DSCIterator range_it = dsc_iterator_range(0, 5, 1, &alloc);

    // Create queue from iterator
    DSCQueue* queue = dsc_queue_from_iterator(&range_it, &alloc);
    ASSERT_NOT_NULL(queue);
    ASSERT_EQ(dsc_queue_size(queue), 5);

    range_it.destroy(&range_it);

    // Verify queue has correct values in FIFO order
    // Iterator gives 0,1,2,3,4 and queue should have them as 0,1,2,3,4 (front to back)
    for (int expected = 0; expected < 5; expected++)
    {
        void* data = dsc_queue_dequeue_data(queue);
        ASSERT_NOT_NULL(data);
        ASSERT_EQ(*(int*)data, expected);
        free(data);
    }

    dsc_queue_destroy(queue, false);
    return TEST_SUCCESS;
}

// Test iterator with empty queue
int test_queue_iterator_empty(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCQueue* queue = dsc_queue_create(&alloc);

    DSCIterator it = dsc_queue_iterator(queue);
    ASSERT(it.is_valid(&it));
    ASSERT(!it.has_next(&it));
    ASSERT_NULL(it.get(&it));
    ASSERT_NULL(it.next(&it));

    it.destroy(&it);
    dsc_queue_destroy(queue, false);
    return TEST_SUCCESS;
}

// Test iterator validity with invalid queue
int test_queue_iterator_invalid(void)
{
    DSCIterator it = dsc_queue_iterator(NULL);
    ASSERT(!it.is_valid(&it));
    return TEST_SUCCESS;
}

// Test iterator state after queue modifications
int test_queue_iterator_modification(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCQueue* queue = dsc_queue_create(&alloc);

    // Add initial data
    for (int i = 0; i < 3; i++)
    {
        int* data = malloc(sizeof(int));
        *data     = i * 10;
        ASSERT_EQ(dsc_queue_enqueue(queue, data), 0);
    }

    DSCIterator it = dsc_queue_iterator(queue);
    ASSERT(it.is_valid(&it));

    // Get first element
    void* first = it.next(&it);
    ASSERT_EQ(*(int*)first, 0); // Should be front element (0*10)

    // Modify queue while iterator exists (implementation detail: iterator may become invalid)
    int* new_data = malloc(sizeof(int));
    *new_data     = 999;
    ASSERT_EQ(dsc_queue_enqueue(queue, new_data), 0);

    // Iterator should still be valid but may not reflect new state
    ASSERT(it.is_valid(&it));

    it.destroy(&it);
    dsc_queue_destroy(queue, true);
    return TEST_SUCCESS;
}

// Test iterator traversal order
int test_queue_iterator_order(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCQueue* queue = dsc_queue_create(&alloc);

    // Enqueue elements with specific pattern
    const int pattern[] = {100, 200, 300, 400, 500};
    for (int i = 0; i < 5; i++)
    {
        int* data = malloc(sizeof(int));
        *data     = pattern[i];
        ASSERT_EQ(dsc_queue_enqueue(queue, data), 0);
    }

    // Iterator should traverse in FIFO order (same as enqueue order)
    DSCIterator it = dsc_queue_iterator(queue);
    for (int i = 0; i < 5; i++)
    {
        ASSERT(it.has_next(&it));
        void* data = it.next(&it);
        ASSERT_EQ(*(int*)data, pattern[i]);
    }
    ASSERT(!it.has_next(&it));

    it.destroy(&it);
    dsc_queue_destroy(queue, true);
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
        {test_queue_iterator, "test_queue_iterator"},
        {test_queue_from_iterator, "test_queue_from_iterator"},
        {test_queue_iterator_empty, "test_queue_iterator_empty"},
        {test_queue_iterator_invalid, "test_queue_iterator_invalid"},
        {test_queue_iterator_modification, "test_queue_iterator_modification"},
        {test_queue_iterator_order, "test_queue_iterator_order"},
    };

    printf("Running Queue iterator tests...\n");

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
        printf("All Queue iterator tests passed!\n");
        return 0;
    }

    printf("%d Queue iterator tests failed.\n", failed);
    return 1;
}
