//
// Created by zack on 9/4/25.
//

#include "DoublyLinkedList.h"
#include "TestAssert.h"
#include "TestHelpers.h"
#include <stdlib.h>

// Property: The size of the list should be consistent after a series of insertions and removals.
int test_dll_size_after_insert_and_remove(void) {
    DoublyLinkedList *list = dll_create();
    ASSERT_EQ(dll_size(list), 0);

    int *a = malloc(sizeof(int)); *a = 1;
    dll_insert_back(list, a);
    ASSERT_EQ(dll_size(list), 1);

    int *b = malloc(sizeof(int)); *b = 2;
    dll_insert_front(list, b);
    ASSERT_EQ(dll_size(list), 2);

    dll_remove_at(list, 0, int_free);
    ASSERT_EQ(dll_size(list), 1);

    dll_remove_back(list, int_free);
    ASSERT_EQ(dll_size(list), 0);

    dll_destroy(list, int_free);
    return TEST_SUCCESS;
}

// Property: Sorting an already sorted list should not change it.
int test_dll_sort_is_idempotent(void) {
    DoublyLinkedList *list = dll_create();
    for (int i = 0; i < 10; i++) {
        int *val = malloc(sizeof(int)); *val = i;
        dll_insert_back(list, val);
    }

    dll_sort(list, int_cmp); // First sort
    DoublyLinkedList *copy = dll_copy_deep(list, int_copy, int_free);

    dll_sort(list, int_cmp); // Second sort

    ASSERT_EQ(dll_equals(list, copy, int_cmp), 1);

    dll_destroy(list, int_free);
    dll_destroy(copy, int_free);
    return TEST_SUCCESS;
}

// Property: Reversing a list twice should return it to its original state.
int test_dll_reverse_is_involution(void) {
    DoublyLinkedList *list = dll_create();
    for (int i = 0; i < 10; i++) {
        int *val = malloc(sizeof(int)); *val = i;
        dll_insert_back(list, val);
    }

    DoublyLinkedList *copy = dll_copy_deep(list, int_copy, int_free);

    dll_reverse(list);
    dll_reverse(list);

    ASSERT_EQ(dll_equals(list, copy, int_cmp), 1);

    dll_destroy(list, int_free);
    dll_destroy(copy, int_free);
    return TEST_SUCCESS;
}

typedef struct {
    int (*func)(void);
    const char *name;
} TestCase;

TestCase tests[] = {
    {test_dll_size_after_insert_and_remove, "test_dll_size_after_insert_and_remove"},
    {test_dll_sort_is_idempotent, "test_dll_sort_is_idempotent"},
    {test_dll_reverse_is_involution, "test_dll_reverse_is_involution"},
};

int main(void) {
    int failed = 0;
    const int num_tests = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < num_tests; i++) {
        if (tests[i].func() != TEST_SUCCESS) {
            printf("%s failed\n", tests[i].name);
            failed++;
        }
    }

    if (failed == 0) {
        printf("All DoublyLinkedList Property tests passed.\n");
        return 0;
    }

    printf("%d DoublyLinkedList Property tests failed.\n", failed);
    return 1;
}