//
// Created by zack on 9/4/25.
//

#include "DoublyLinkedList.h"
#include "TestAssert.h"
#include "TestHelpers.h"
#include <stdlib.h>

// Property: The size of the list should be consistent after a series of insertions and removals.
int test_dsc_dll_size_after_insert_and_remove(void)
{
    DSCAlloc alloc = create_int_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(&alloc);
    ASSERT_EQ(dsc_dll_size(list), 0);

    int* a = malloc(sizeof(int));
    *a     = 1;
    dsc_dll_insert_back(list, a);
    ASSERT_EQ(dsc_dll_size(list), 1);

    int* b = malloc(sizeof(int));
    *b     = 2;
    dsc_dll_insert_front(list, b);
    ASSERT_EQ(dsc_dll_size(list), 2);

    dsc_dll_remove_at(list, 0, true);
    ASSERT_EQ(dsc_dll_size(list), 1);

    dsc_dll_remove_back(list, true);
    ASSERT_EQ(dsc_dll_size(list), 0);

    dsc_dll_destroy(list, false);
    return TEST_SUCCESS;
}

// Property: Sorting an already sorted list should not change it.
int test_dsc_dll_sort_is_idempotent(void)
{
    DSCAlloc alloc = create_int_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(&alloc);
    for (int i = 0; i < 10; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_dll_insert_back(list, val);
    }

    dsc_dll_sort(list, int_cmp); // First sort
    DSCDoublyLinkedList* copy = dsc_dll_copy_deep(list, true);

    dsc_dll_sort(list, int_cmp); // Second sort

    ASSERT_EQ(dsc_dll_equals(list, copy, int_cmp), 1);

    dsc_dll_destroy(list, true);
    dsc_dll_destroy(copy, true);
    return TEST_SUCCESS;
}

// Property: Reversing a list twice should return it to its original state.
int test_dsc_dll_reverse_is_involution(void)
{
    DSCAlloc alloc = create_int_allocator();
    DSCDoublyLinkedList* list = dsc_dll_create(&alloc);
    for (int i = 0; i < 10; i++)
    {
        int* val = malloc(sizeof(int));
        *val     = i;
        dsc_dll_insert_back(list, val);
    }

    DSCDoublyLinkedList* copy = dsc_dll_copy_deep(list, true);

    dsc_dll_reverse(list);
    dsc_dll_reverse(list);

    ASSERT_EQ(dsc_dll_equals(list, copy, int_cmp), 1);

    dsc_dll_destroy(list, true);
    dsc_dll_destroy(copy, true);
    return TEST_SUCCESS;
}

typedef struct
{
    int (*func)(void);
    const char* name;
} TestCase;

TestCase tests[] = {
    {test_dsc_dll_size_after_insert_and_remove, "test_dsc_dll_size_after_insert_and_remove"},
    {test_dsc_dll_sort_is_idempotent, "test_dsc_dll_sort_is_idempotent"},
    {test_dsc_dll_reverse_is_involution, "test_dsc_dll_reverse_is_involution"},
};

int main(void)
{
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
        printf("All DoublyLinkedList Property tests passed.\n");
        return 0;
    }

    printf("%d DoublyLinkedList Property tests failed.\n", failed);
    return 1;
}