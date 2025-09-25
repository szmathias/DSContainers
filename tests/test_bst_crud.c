//
// Created by zack on 9/25/25.
//
// Binary Search Tree CRUD operations test

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BinarySearchTree.h"
#include "TestAssert.h"
#include "TestHelpers.h"

// Test basic BST creation and destruction
int test_bst_create_destroy(void)
{
    DSCAllocator alloc = create_int_allocator();

    DSCBinarySearchTree* bst = dsc_bst_create(&alloc, int_cmp);
    ASSERT_NOT_NULL(bst);
    ASSERT_EQ(dsc_bst_size(bst), 0);
    ASSERT(dsc_bst_is_empty(bst));
    ASSERT_EQ(dsc_bst_height(bst), 0);

    dsc_bst_destroy(bst, false);
    return TEST_SUCCESS;
}

// Test NULL parameter handling
int test_bst_null_parameters(void)
{
    DSCAllocator alloc = create_int_allocator();

    // Creating with NULL compare function should fail
    ASSERT_NULL(dsc_bst_create(&alloc, NULL));

    // Creating with NULL allocator should fail
    ASSERT_NULL(dsc_bst_create(NULL, int_cmp));

    // Operations on NULL tree should be safe
    ASSERT_EQ(dsc_bst_size(NULL), 0);
    ASSERT(dsc_bst_is_empty(NULL));
    ASSERT_EQ(dsc_bst_height(NULL), 0);
    ASSERT(!dsc_bst_contains(NULL, NULL));
    ASSERT_NULL(dsc_bst_min(NULL));
    ASSERT_NULL(dsc_bst_max(NULL));
    ASSERT_EQ(dsc_bst_insert(NULL, NULL), -1);
    ASSERT_EQ(dsc_bst_remove(NULL, NULL, false), -1);

    // Destruction should be safe
    dsc_bst_destroy(NULL, false);
    dsc_bst_clear(NULL, false);

    return TEST_SUCCESS;
}

// Test basic insertion operations
int test_bst_insert(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCBinarySearchTree* bst = dsc_bst_create(&alloc, int_cmp);

    int* data1 = malloc(sizeof(int));
    int* data2 = malloc(sizeof(int));
    int* data3 = malloc(sizeof(int));
    *data1 = 50;
    *data2 = 30;
    *data3 = 70;

    // Test inserting root
    ASSERT_EQ(dsc_bst_insert(bst, data1), 0);
    ASSERT_EQ(dsc_bst_size(bst), 1);
    ASSERT(!dsc_bst_is_empty(bst));
    ASSERT_EQ(dsc_bst_height(bst), 1);

    // Test inserting left child
    ASSERT_EQ(dsc_bst_insert(bst, data2), 0);
    ASSERT_EQ(dsc_bst_size(bst), 2);
    ASSERT_EQ(dsc_bst_height(bst), 2);

    // Test inserting right child
    ASSERT_EQ(dsc_bst_insert(bst, data3), 0);
    ASSERT_EQ(dsc_bst_size(bst), 3);
    ASSERT_EQ(dsc_bst_height(bst), 2);

    // Test duplicate insertion
    int* duplicate = malloc(sizeof(int));
    *duplicate = 50;
    ASSERT_EQ(dsc_bst_insert(bst, duplicate), 1); // Should return 1 for duplicate
    ASSERT_EQ(dsc_bst_size(bst), 3); // Size should not change
    free(duplicate);

    // Test NULL data insertion
    ASSERT_EQ(dsc_bst_insert(bst, NULL), -1);

    dsc_bst_destroy(bst, true);
    return TEST_SUCCESS;
}

// Test contains functionality
int test_bst_contains(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCBinarySearchTree* bst = dsc_bst_create(&alloc, int_cmp);

    int values[] = {50, 30, 70, 20, 40, 60, 80};
    int* data[7];

    // Insert test data
    for (int i = 0; i < 7; i++)
    {
        data[i] = malloc(sizeof(int));
        *(data[i]) = values[i];
        ASSERT_EQ(dsc_bst_insert(bst, data[i]), 0);
    }

    // Test existing values
    for (int i = 0; i < 7; i++)
    {
        ASSERT(dsc_bst_contains(bst, &values[i]));
    }

    // Test non-existing values
    int non_existing[] = {10, 25, 35, 55, 75, 90};
    for (int i = 0; i < 6; i++)
    {
        ASSERT(!dsc_bst_contains(bst, &non_existing[i]));
    }

    // Test with NULL
    ASSERT(!dsc_bst_contains(bst, NULL));

    dsc_bst_destroy(bst, true);
    return TEST_SUCCESS;
}

// Test min and max functionality
int test_bst_min_max(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCBinarySearchTree* bst = dsc_bst_create(&alloc, int_cmp);

    // Empty tree
    ASSERT_NULL(dsc_bst_min(bst));
    ASSERT_NULL(dsc_bst_max(bst));

    int values[] = {50, 30, 70, 20, 40, 60, 80, 10, 90};
    int* data[9];

    // Insert test data
    for (int i = 0; i < 9; i++)
    {
        data[i] = malloc(sizeof(int));
        *(data[i]) = values[i];
        ASSERT_EQ(dsc_bst_insert(bst, data[i]), 0);
    }

    // Test min and max
    int* min_val = (int*)dsc_bst_min(bst);
    int* max_val = (int*)dsc_bst_max(bst);

    ASSERT_NOT_NULL(min_val);
    ASSERT_NOT_NULL(max_val);
    ASSERT_EQ(*min_val, 10);
    ASSERT_EQ(*max_val, 90);

    dsc_bst_destroy(bst, true);
    return TEST_SUCCESS;
}

// Test removal operations
int test_bst_remove(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCBinarySearchTree* bst = dsc_bst_create(&alloc, int_cmp);

    int values[] = {50, 30, 70, 20, 40, 60, 80};
    int* data[7];

    // Insert test data
    for (int i = 0; i < 7; i++)
    {
        data[i] = malloc(sizeof(int));
        *(data[i]) = values[i];
        ASSERT_EQ(dsc_bst_insert(bst, data[i]), 0);
    }

    ASSERT_EQ(dsc_bst_size(bst), 7);

    // Test removing leaf node
    int leaf_val = 20;
    ASSERT_EQ(dsc_bst_remove(bst, &leaf_val, true), 0);
    ASSERT_EQ(dsc_bst_size(bst), 6);
    ASSERT(!dsc_bst_contains(bst, &leaf_val));

    // Test removing node with one child
    int one_child_val = 30;
    ASSERT_EQ(dsc_bst_remove(bst, &one_child_val, true), 0);
    ASSERT_EQ(dsc_bst_size(bst), 5);
    ASSERT(!dsc_bst_contains(bst, &one_child_val));

    // Test removing node with two children (root)
    int root_val = 50;
    ASSERT_EQ(dsc_bst_remove(bst, &root_val, true), 0);
    ASSERT_EQ(dsc_bst_size(bst), 4);
    ASSERT(!dsc_bst_contains(bst, &root_val));

    // Test removing non-existent value
    int non_existent = 99;
    ASSERT_EQ(dsc_bst_remove(bst, &non_existent, false), -1);
    ASSERT_EQ(dsc_bst_size(bst), 4);

    // Test with NULL
    ASSERT_EQ(dsc_bst_remove(bst, NULL, false), -1);

    dsc_bst_destroy(bst, true); // Remaining nodes were not freed individually
    return TEST_SUCCESS;
}

// Test clear operation
int test_bst_clear(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCBinarySearchTree* bst = dsc_bst_create(&alloc, int_cmp);

    // Add some elements
    for (int i = 0; i < 10; i++)
    {
        int* data = malloc(sizeof(int));
        *data = i * 10;
        ASSERT_EQ(dsc_bst_insert(bst, data), 0);
    }

    ASSERT_EQ(dsc_bst_size(bst), 10);
    ASSERT(!dsc_bst_is_empty(bst));

    // Clear with freeing data
    dsc_bst_clear(bst, true);
    ASSERT_EQ(dsc_bst_size(bst), 0);
    ASSERT(dsc_bst_is_empty(bst));
    ASSERT_EQ(dsc_bst_height(bst), 0);
    ASSERT_NULL(dsc_bst_min(bst));
    ASSERT_NULL(dsc_bst_max(bst));

    // Tree should still be usable after clear
    int* new_data = malloc(sizeof(int));
    *new_data = 999;
    ASSERT_EQ(dsc_bst_insert(bst, new_data), 0);
    ASSERT_EQ(dsc_bst_size(bst), 1);
    ASSERT_EQ(*(int*)dsc_bst_min(bst), 999);

    dsc_bst_destroy(bst, true);
    return TEST_SUCCESS;
}

// Test BST property maintenance
int test_bst_property(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCBinarySearchTree* bst = dsc_bst_create(&alloc, int_cmp);

    // Insert values in various orders to test BST property
    int values[] = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45};
    int* data[11];

    for (int i = 0; i < 11; i++)
    {
        data[i] = malloc(sizeof(int));
        *(data[i]) = values[i];
        ASSERT_EQ(dsc_bst_insert(bst, data[i]), 0);
    }

    // Verify all values are present
    for (int i = 0; i < 11; i++)
    {
        ASSERT(dsc_bst_contains(bst, &values[i]));
    }

    // Verify min and max are correct
    ASSERT_EQ(*(int*)dsc_bst_min(bst), 10);
    ASSERT_EQ(*(int*)dsc_bst_max(bst), 80);

    dsc_bst_destroy(bst, true);
    return TEST_SUCCESS;
}

// Test with string data
int test_bst_string_data(void)
{
    DSCAllocator alloc = create_string_allocator();
    DSCBinarySearchTree* bst = dsc_bst_create(&alloc, (cmp_func)strcmp);

    char* strings[] = {"apple", "banana", "cherry", "date", "elderberry"};
    int num_strings = 5;

    // Insert strings
    for (int i = 0; i < num_strings; i++)
    {
        char* str_copy = malloc(strlen(strings[i]) + 1);
        strcpy(str_copy, strings[i]);
        ASSERT_EQ(dsc_bst_insert(bst, str_copy), 0);
    }

    ASSERT_EQ(dsc_bst_size(bst), num_strings);

    // Test contains
    for (int i = 0; i < num_strings; i++)
    {
        ASSERT(dsc_bst_contains(bst, strings[i]));
    }

    // Test min and max (lexicographic order)
    ASSERT_EQ_STR((char*)dsc_bst_min(bst), "apple");
    ASSERT_EQ_STR((char*)dsc_bst_max(bst), "elderberry");

    dsc_bst_destroy(bst, true);
    return TEST_SUCCESS;
}

typedef struct
{
    int (*func)(void);
    const char* name;
} TestCase;

int main(void)
{
    TestCase tests[] = {
        {test_bst_create_destroy, "test_bst_create_destroy"},
        {test_bst_null_parameters, "test_bst_null_parameters"},
        {test_bst_insert, "test_bst_insert"},
        {test_bst_contains, "test_bst_contains"},
        {test_bst_min_max, "test_bst_min_max"},
        {test_bst_remove, "test_bst_remove"},
        {test_bst_clear, "test_bst_clear"},
        {test_bst_property, "test_bst_property"},
        {test_bst_string_data, "test_bst_string_data"}
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;

    printf("Running BST CRUD tests...\n");

    for (int i = 0; i < num_tests; i++)
    {
        printf("Running %s... ", tests[i].name);
        fflush(stdout);

        int result = tests[i].func();
        if (result == TEST_SUCCESS)
        {
            printf("PASSED\n");
            passed++;
        }
        else if (result == TEST_SKIPPED)
        {
            printf("SKIPPED\n");
        }
        else
        {
            printf("FAILED\n");
        }
    }

    printf("\nBST CRUD Tests: %d/%d passed\n", passed, num_tests);
    return (passed == num_tests) ? 0 : 1;
}
