//
// Created by zack on 9/1/25.
//

#include "DoublyLinkedList.h"
#include "TestAssert.h"
#include "TestHelpers.h"
#include <stdio.h>
#include <stdlib.h>
#include "Threads.h"

#define NUM_THREADS 4
#define NUM_ITERATIONS 10000

typedef struct
{
    DSCDoublyLinkedList* list;
    int start_val;
    void** allocated; // array to keep pointers allocated by this thread
} thread_data;

// Thread function to add items to the list
void* add_items(void* arg)
{
    const thread_data* data = (thread_data*)arg;
    for (int i = 0; i < NUM_ITERATIONS; ++i)
    {
        int* val           = malloc(sizeof(int));
        *val               = data->start_val + i;
        data->allocated[i] = val; // record pointer so main can free it later
        dsc_dll_insert_back(data->list, val);
    }
    return 0;
}

// This test demonstrates the lack of thread safety.
// Without locks, the final size of the list will likely be incorrect
// due to race conditions when updating head/tail/size pointers.
int test_concurrent_insertions(void)
{
    printf("\n[INFO] Running threaded test. This test is expected to show race conditions without thread-safety measures.\n");
    DSCAlloc* alloc           = create_std_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(alloc);
    DSCThread threads[NUM_THREADS];
    thread_data thread_data[NUM_THREADS];

    // allocate per-thread storage for allocated pointers
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        thread_data[i].list      = list;
        thread_data[i].start_val = i * NUM_ITERATIONS;
        thread_data[i].allocated = malloc(NUM_ITERATIONS * sizeof(void*));
        if (!thread_data[i].allocated)
        {
            fprintf(stderr, "failed to allocate tracking array\n");
            return TEST_FAILURE;
        }
    }

    for (int i = 0; i < NUM_THREADS; ++i)
    {
        dsc_thread_create(&threads[i], add_items, &thread_data[i]);
    }

    for (int i = 0; i < NUM_THREADS; ++i)
    {
        dsc_thread_join(threads[i], NULL);
    }

    const size_t expected_size = NUM_THREADS * NUM_ITERATIONS;
    const size_t actual_size   = dsc_dll_size(list);

    if (actual_size != expected_size)
    {
        fprintf(stderr, "[KNOWN BEHAVIOR] Race condition detected: expected size %zu, but got %zu.\n",
                expected_size, actual_size);
    }
    else
    {
        printf("[INFO] Threaded test passed, which may indicate a single-core environment or sheer luck.\n");
    }

    // Destroy list WITHOUT freeing data (we will free tracked pointers ourselves)
    dsc_dll_destroy(list, false);

    // Free all allocated pointers recorded by threads to avoid leaks
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        for (int j = 0; j < NUM_ITERATIONS; ++j)
        {
            if (thread_data[i].allocated[j])
                free(thread_data[i].allocated[j]);
        }
        free(thread_data[i].allocated);
    }

    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int main(void)
{
    if (test_concurrent_insertions() == TEST_SUCCESS)
    {
        printf("DoublyLinkedList Threaded test completed as expected.\n");
        return 0;
    }
    return 1;
}