//
// Created by zack on 9/4/25.
//

#include "SinglyLinkedList.h"
#include "TestAssert.h"
#include "TestHelpers.h"
#include <stdlib.h>

// Property: The size of the list should be consistent after a series of insertions and removals.
int test_sll_size_after_insert_and_remove(void) {
    SinglyLinkedList *list = sll_create();
    ASSERT_EQ(sll_size(list), 0);

    int *a = malloc(sizeof(int)); *a = 1;
    sll_insert_back(list, a);
    ASSERT_EQ(sll_size(list), 1);

    int *b = malloc(sizeof(int)); *b = 2;
    sll_insert_back(list, b);
    ASSERT_EQ(sll_size(list), 2);

    sll_remove_at(list, 0, int_free);
    ASSERT_EQ(sll_size(list), 1);

    sll_remove_at(list, 0, int_free);
    ASSERT_EQ(sll_size(list), 0);

    sll_destroy(list, int_free);
    return TEST_SUCCESS;
}

// Property: Sorting an already sorted list should not change it.
int test_sll_sort_is_idempotent(void) {
    SinglyLinkedList *list = sll_create();
    for (int i = 0; i < 10; i++) {
        int *val = malloc(sizeof(int)); *val = i;
        sll_insert_back(list, val);
    }

    sll_sort(list, int_cmp); // First sort
    SinglyLinkedList *copy = sll_copy_deep(list, int_copy, int_free);

    sll_sort(list, int_cmp); // Second sort

    ASSERT_EQ(sll_equals(list, copy, int_cmp), 1);

    sll_destroy(list, int_free);
    sll_destroy(copy, int_free);
    return TEST_SUCCESS;
}

// Property: Reversing a list twice should return it to its original state.
int test_sll_reverse_is_involution(void) {
    SinglyLinkedList *list = sll_create();
    for (int i = 0; i < 10; i++) {
        int *val = malloc(sizeof(int)); *val = i;
        sll_insert_back(list, val);
    }

    SinglyLinkedList *copy = sll_copy_deep(list, int_copy, int_free);

    sll_reverse(list);
    sll_reverse(list);

    ASSERT_EQ(sll_equals(list, copy, int_cmp), 1);

    sll_destroy(list, int_free);
    sll_destroy(copy, int_free);
    return TEST_SUCCESS;
}

typedef struct {
    int (*func)(void);
    const char *name;
} TestCase;

TestCase tests[] = {
    {test_sll_size_after_insert_and_remove, "test_sll_size_after_insert_and_remove"},
    {test_sll_sort_is_idempotent, "test_sll_sort_is_idempotent"},
    {test_sll_reverse_is_involution, "test_sll_reverse_is_involution"},
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
        printf("All SinglyLinkedList Property tests passed.\n");
        return 0;
    }

    printf("%d SinglyLinkedList Property tests failed.\n", failed);
    return 1;
}