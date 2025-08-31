//
// Created by zack on 9/1/25.
//

#include "DoublyLinkedList.h"
#include "TestAssert.h"
#include "TestHelpers.h"
#include <stdio.h>
#include <stdlib.h>

#if !defined(DS_PLATFORM_WINDOWS)
#include <threads.h> // C11 threads
#else
#include <windows.h>
#endif

#define NUM_THREADS 4
#define NUM_ITERATIONS 10000

typedef struct {
    DoublyLinkedList *list;
    int start_val;
} thread_data_t;

// Thread function to add items to the list
int add_items(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        int *val = malloc(sizeof(int));
        *val = data->start_val + i;
        dll_insert_back(data->list, val);
    }
    return 0;
}

// This test demonstrates the lack of thread safety.
// Without locks, the final size of the list will likely be incorrect
// due to race conditions when updating head/tail/size pointers.
int test_concurrent_insertions(void) {
    printf("\n[INFO] Running threaded test. This test is expected to show race conditions without thread-safety measures.\n");
    DoublyLinkedList *list = dll_create();
    thrd_t threads[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_data[i].list = list;
        thread_data[i].start_val = i * NUM_ITERATIONS;
        thrd_create(&threads[i], add_items, &thread_data[i]);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        thrd_join(threads[i], NULL);
    }

    const size_t expected_size = NUM_THREADS * NUM_ITERATIONS;
    const size_t actual_size = dll_size(list);

    if (actual_size != expected_size) {
        fprintf(stderr, "[KNOWN BEHAVIOR] Race condition detected: expected size %zu, but got %zu.\n",
                expected_size, actual_size);
    } else {
         printf("[INFO] Threaded test passed, which may indicate a single-core environment or sheer luck.\n");
    }

    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

int main(void) {
    if (test_concurrent_insertions() == TEST_SUCCESS) {
        printf("DoublyLinkedList Threaded test completed as expected.\n");
        return 0;
    }
    return 1;
}