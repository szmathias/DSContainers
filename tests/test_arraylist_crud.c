//
// Created by zack on 9/2/25.
//

#include "ArrayList.h"
#include "TestAssert.h"
#include "TestHelpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_create_destroy(void) {
    DSCAlloc *alloc = create_std_allocator();
    DSCArrayList *list = dsc_arraylist_create(alloc, 0);
    ASSERT_NOT_NULL(list);
    ASSERT_EQ(dsc_arraylist_size(list), 0);
    ASSERT_EQ(dsc_arraylist_capacity(list), 0);
    ASSERT(dsc_arraylist_is_empty(list));
    dsc_arraylist_destroy(list, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_create_with_capacity(void) {
    DSCAlloc *alloc = create_std_allocator();
    DSCArrayList *list = dsc_arraylist_create(alloc, 10);
    ASSERT_NOT_NULL(list);
    ASSERT_EQ(dsc_arraylist_size(list), 0);
    ASSERT_GT(dsc_arraylist_capacity(list), 0);
    ASSERT(dsc_arraylist_is_empty(list));
    dsc_arraylist_destroy(list, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_push_back(void) {
    DSCAlloc *alloc = create_std_allocator();
    DSCArrayList *list = dsc_arraylist_create(alloc, 0);

    int *a = malloc(sizeof(int)); *a = 1;
    int *b = malloc(sizeof(int)); *b = 2;
    int *c = malloc(sizeof(int)); *c = 3;

    ASSERT_EQ(dsc_arraylist_push_back(list, a), 0);
    ASSERT_EQ(dsc_arraylist_size(list), 1);
    ASSERT(!dsc_arraylist_is_empty(list));

    ASSERT_EQ(dsc_arraylist_push_back(list, b), 0);
    ASSERT_EQ(dsc_arraylist_push_back(list, c), 0);
    ASSERT_EQ(dsc_arraylist_size(list), 3);

    // Verify elements
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 0), 1);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 1), 2);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 2), 3);

    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_push_front(void) {
    DSCAlloc *alloc = create_std_allocator();
    DSCArrayList *list = dsc_arraylist_create(alloc, 0);

    int *a = malloc(sizeof(int)); *a = 1;
    int *b = malloc(sizeof(int)); *b = 2;
    int *c = malloc(sizeof(int)); *c = 3;

    ASSERT_EQ(dsc_arraylist_push_front(list, a), 0);
    ASSERT_EQ(dsc_arraylist_push_front(list, b), 0);
    ASSERT_EQ(dsc_arraylist_push_front(list, c), 0);
    ASSERT_EQ(dsc_arraylist_size(list), 3);

    // Verify elements (should be in reverse order)
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 0), 3);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 1), 2);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 2), 1);

    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_insert_at(void) {
    DSCAlloc *alloc = create_std_allocator();
    DSCArrayList *list = dsc_arraylist_create(alloc, 0);

    int *a = malloc(sizeof(int)); *a = 1;
    int *b = malloc(sizeof(int)); *b = 2;
    int *c = malloc(sizeof(int)); *c = 3;
    int *d = malloc(sizeof(int)); *d = 4;

    // Insert at beginning (empty list)
    ASSERT_EQ(dsc_arraylist_insert(list, 0, a), 0);
    ASSERT_EQ(dsc_arraylist_size(list), 1);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 0), 1);

    // Insert at end
    ASSERT_EQ(dsc_arraylist_insert(list, 1, c), 0);
    ASSERT_EQ(dsc_arraylist_size(list), 2);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 1), 3);

    // Insert in middle
    ASSERT_EQ(dsc_arraylist_insert(list, 1, b), 0);
    ASSERT_EQ(dsc_arraylist_size(list), 3);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 0), 1);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 1), 2);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 2), 3);

    // Insert at beginning
    ASSERT_EQ(dsc_arraylist_insert(list, 0, d), 0);
    ASSERT_EQ(dsc_arraylist_size(list), 4);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 0), 4);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 1), 1);

    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_get_set(void) {
    DSCAlloc *alloc = create_std_allocator();
    DSCArrayList *list = dsc_arraylist_create(alloc, 0);

    int *a = malloc(sizeof(int)); *a = 1;
    int *b = malloc(sizeof(int)); *b = 2;
    int *c = malloc(sizeof(int)); *c = 3;
    int *d = malloc(sizeof(int)); *d = 42;

    dsc_arraylist_push_back(list, a);
    dsc_arraylist_push_back(list, b);
    dsc_arraylist_push_back(list, c);

    // Test get
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 0), 1);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 1), 2);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 2), 3);
    ASSERT_NULL(dsc_arraylist_get(list, 3)); // Out of bounds

    // Test set
    ASSERT_EQ(dsc_arraylist_set(list, 1, d, true), 0);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 1), 42);
    ASSERT_EQ(dsc_arraylist_set(list, 5, d, false), -1); // Out of bounds

    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_front_back(void) {
    DSCAlloc *alloc = create_std_allocator();
    DSCArrayList *list = dsc_arraylist_create(alloc, 0);

    // Empty list
    ASSERT_NULL(dsc_arraylist_front(list));
    ASSERT_NULL(dsc_arraylist_back(list));

    int *a = malloc(sizeof(int)); *a = 1;
    int *b = malloc(sizeof(int)); *b = 2;
    int *c = malloc(sizeof(int)); *c = 3;

    // Single element
    dsc_arraylist_push_back(list, a);
    ASSERT_EQ(*(int*)dsc_arraylist_front(list), 1);
    ASSERT_EQ(*(int*)dsc_arraylist_back(list), 1);

    // Multiple elements
    dsc_arraylist_push_back(list, b);
    dsc_arraylist_push_back(list, c);
    ASSERT_EQ(*(int*)dsc_arraylist_front(list), 1);
    ASSERT_EQ(*(int*)dsc_arraylist_back(list), 3);

    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_remove_at(void) {
    DSCAlloc *alloc = create_std_allocator();
    DSCArrayList *list = dsc_arraylist_create(alloc, 0);

    int *a = malloc(sizeof(int)); *a = 1;
    int *b = malloc(sizeof(int)); *b = 2;
    int *c = malloc(sizeof(int)); *c = 3;
    int *d = malloc(sizeof(int)); *d = 4;

    dsc_arraylist_push_back(list, a);
    dsc_arraylist_push_back(list, b);
    dsc_arraylist_push_back(list, c);
    dsc_arraylist_push_back(list, d);

    // Remove from middle
    ASSERT_EQ(dsc_arraylist_remove_at(list, 1, true), 0);
    ASSERT_EQ(dsc_arraylist_size(list), 3);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 0), 1);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 1), 3);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 2), 4);

    // Remove from beginning
    ASSERT_EQ(dsc_arraylist_remove_at(list, 0, true), 0);
    ASSERT_EQ(dsc_arraylist_size(list), 2);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 0), 3);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 1), 4);

    // Remove from end
    ASSERT_EQ(dsc_arraylist_remove_at(list, 1, true), 0);
    ASSERT_EQ(dsc_arraylist_size(list), 1);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 0), 3);

    // Test invalid index
    ASSERT_EQ(dsc_arraylist_remove_at(list, 5, false), -1);

    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_pop_back_front(void) {
    DSCAlloc *alloc = create_std_allocator();
    DSCArrayList *list = dsc_arraylist_create(alloc, 0);

    int *a = malloc(sizeof(int)); *a = 1;
    int *b = malloc(sizeof(int)); *b = 2;
    int *c = malloc(sizeof(int)); *c = 3;

    dsc_arraylist_push_back(list, a);
    dsc_arraylist_push_back(list, b);
    dsc_arraylist_push_back(list, c);

    // Pop back
    ASSERT_EQ(dsc_arraylist_pop_back(list, true), 0);
    ASSERT_EQ(dsc_arraylist_size(list), 2);
    ASSERT_EQ(*(int*)dsc_arraylist_back(list), 2);

    // Pop front
    ASSERT_EQ(dsc_arraylist_pop_front(list, true), 0);
    ASSERT_EQ(dsc_arraylist_size(list), 1);
    ASSERT_EQ(*(int*)dsc_arraylist_front(list), 2);

    // Pop remaining element
    ASSERT_EQ(dsc_arraylist_pop_back(list, true), 0);
    ASSERT_EQ(dsc_arraylist_size(list), 0);
    ASSERT(dsc_arraylist_is_empty(list));

    // Test pop from empty list
    ASSERT_EQ(dsc_arraylist_pop_back(list, false), -1);
    ASSERT_EQ(dsc_arraylist_pop_front(list, false), -1);

    dsc_arraylist_destroy(list, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_find(void) {
    DSCAlloc *alloc = create_std_allocator();
    DSCArrayList *list = dsc_arraylist_create(alloc, 0);

    int *a = malloc(sizeof(int)); *a = 1;
    int *b = malloc(sizeof(int)); *b = 2;
    int *c = malloc(sizeof(int)); *c = 3;

    dsc_arraylist_push_back(list, a);
    dsc_arraylist_push_back(list, b);
    dsc_arraylist_push_back(list, c);

    const int key = 2;
    size_t index = dsc_arraylist_find(list, &key, int_cmp);
    ASSERT_EQ(index, 1);

    const int not_found = 42;
    index = dsc_arraylist_find(list, &not_found, int_cmp);
    ASSERT_EQ(index, SIZE_MAX);

    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_remove(void) {
    DSCAlloc *alloc = create_std_allocator();
    DSCArrayList *list = dsc_arraylist_create(alloc, 0);

    int *a = malloc(sizeof(int)); *a = 1;
    int *b = malloc(sizeof(int)); *b = 2;
    int *c = malloc(sizeof(int)); *c = 3;

    dsc_arraylist_push_back(list, a);
    dsc_arraylist_push_back(list, b);
    dsc_arraylist_push_back(list, c);

    const int key = 2;
    ASSERT_EQ(dsc_arraylist_remove(list, &key, int_cmp, true), 0);
    ASSERT_EQ(dsc_arraylist_size(list), 2);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 0), 1);
    ASSERT_EQ(*(int*)dsc_arraylist_get(list, 1), 3);

    const int not_found = 42;
    ASSERT_EQ(dsc_arraylist_remove(list, &not_found, int_cmp, false), -1);

    dsc_arraylist_destroy(list, true);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

int test_clear(void) {
    DSCAlloc *alloc = create_std_allocator();
    DSCArrayList *list = dsc_arraylist_create(alloc, 0);

    int *a = malloc(sizeof(int)); *a = 1;
    int *b = malloc(sizeof(int)); *b = 2;
    int *c = malloc(sizeof(int)); *c = 3;

    dsc_arraylist_push_back(list, a);
    dsc_arraylist_push_back(list, b);
    dsc_arraylist_push_back(list, c);

    dsc_arraylist_clear(list, true);
    ASSERT_EQ(dsc_arraylist_size(list), 0);
    ASSERT(dsc_arraylist_is_empty(list));
    ASSERT_GT(dsc_arraylist_capacity(list), 0); // Capacity should remain

    dsc_arraylist_destroy(list, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

typedef struct {
    int (*func)(void);
    const char *name;
} TestCase;

int main(void) {
    TestCase tests[] = {
        {test_create_destroy, "test_create_destroy"},
        {test_create_with_capacity, "test_create_with_capacity"},
        {test_push_back, "test_push_back"},
        {test_push_front, "test_push_front"},
        {test_insert_at, "test_insert_at"},
        {test_get_set, "test_get_set"},
        {test_front_back, "test_front_back"},
        {test_remove_at, "test_remove_at"},
        {test_pop_back_front, "test_pop_back_front"},
        {test_find, "test_find"},
        {test_remove, "test_remove"},
        {test_clear, "test_clear"},
    };

    int failed = 0;
    const int num_tests = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < num_tests; i++) {
        if (tests[i].func() != TEST_SUCCESS) {
            printf("%s failed\n", tests[i].name);
            failed++;
        }
    }

    if (failed == 0) {
        printf("All ArrayList CRUD tests passed.\n");
        return 0;
    }

    printf("%d ArrayList CRUD tests failed.\n", failed);
    return 1;
}
