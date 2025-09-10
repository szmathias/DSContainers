//
// HashSet Iterator test
//

#include "TestAssert.h"
#include "TestHelpers.h"
#include "HashSet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Test basic iterator functionality
int test_hashset_iterator_basic(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashSet* set = dsc_hashset_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    for (int i = 0; i < 3; i++)
    {
        char* elements[] = {"apple", "banana", "cherry"};
        ASSERT_EQ(dsc_hashset_add(set, elements[i]), 0);
    }

    DSCIterator it = dsc_hashset_iterator(set);
    ASSERT(it.is_valid(&it));

    // Count elements through iteration
    int count = 0;
    while (it.has_next(&it))
    {
        const void* key = it.next(&it);
        ASSERT_NOT_NULL(key);
        count++;
    }
    ASSERT_EQ(count, 3);

    it.destroy(&it);
    dsc_hashset_destroy(set, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test iterator with empty set
int test_hashset_iterator_empty(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashSet* set = dsc_hashset_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    DSCIterator it = dsc_hashset_iterator(set);
    ASSERT(it.is_valid(&it));
    ASSERT(!it.has_next(&it));

    void* key = it.next(&it);
    ASSERT_NULL(key);

    it.destroy(&it);
    dsc_hashset_destroy(set, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test iterator reset functionality
int test_hashset_iterator_reset(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashSet* set = dsc_hashset_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    ASSERT_EQ(dsc_hashset_add(set, "key1"), 0);
    ASSERT_EQ(dsc_hashset_add(set, "key2"), 0);

    DSCIterator it = dsc_hashset_iterator(set);

    // First iteration
    int count1 = 0;
    while (it.has_next(&it))
    {
        it.next(&it);
        count1++;
    }
    ASSERT_EQ(count1, 2);

    // Reset and iterate again
    it.reset(&it);
    int count2 = 0;
    while (it.has_next(&it))
    {
        it.next(&it);
        count2++;
    }
    ASSERT_EQ(count2, 2);

    it.destroy(&it);
    dsc_hashset_destroy(set, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test iterator get current element
int test_hashset_iterator_get(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashSet* set = dsc_hashset_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    char* key = "test_key";
    ASSERT_EQ(dsc_hashset_add(set, key), 0);

    DSCIterator it = dsc_hashset_iterator(set);

    // Before calling next, get should return NULL
    ASSERT_NULL(it.get(&it));

    // After calling next, get should return the current element
    ASSERT(it.has_next(&it));
    void* next_key = it.next(&it);
    ASSERT_NOT_NULL(next_key);

    void* current_key = it.get(&it);
    ASSERT_EQ(next_key, current_key);
    ASSERT_EQ_STR((char*)current_key, key);

    it.destroy(&it);
    dsc_hashset_destroy(set, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test iterator backward operations (should not be supported)
int test_hashset_iterator_backward(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashSet* set = dsc_hashset_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    ASSERT_EQ(dsc_hashset_add(set, "key"), 0);

    DSCIterator it = dsc_hashset_iterator(set);

    // HashSet iterator should not support backward iteration
    ASSERT(!it.has_prev(&it));
    ASSERT_NULL(it.prev(&it));

    it.destroy(&it);
    dsc_hashset_destroy(set, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test iterator with NULL set
int test_hashset_iterator_null_set(void)
{
    const DSCIterator it = dsc_hashset_iterator(NULL);

    // Iterator should be invalid for NULL set
    ASSERT(!it.is_valid(&it));
    ASSERT(!it.has_next(&it));
    ASSERT_NULL(it.next(&it));
    ASSERT_NULL(it.get(&it));

    return TEST_SUCCESS;
}

// Test from_iterator functionality
int test_hashset_from_iterator(void)
{
    DSCAlloc* alloc = create_std_allocator();
    // Set the copy function to string_copy for proper string handling
    alloc->copy_func = string_copy;

    DSCHashSet* original_set = dsc_hashset_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    char* elements[] = {"one", "two", "three"};
    for (int i = 0; i < 3; i++)
    {
        ASSERT_EQ(dsc_hashset_add(original_set, elements[i]), 0);
    }

    // Create iterator and new set from it
    DSCIterator it = dsc_hashset_iterator(original_set);
    DSCHashSet* new_set = dsc_hashset_from_iterator(&it, alloc, dsc_hash_string, dsc_key_equals_string);

    ASSERT_NOT_NULL(new_set);
    ASSERT_EQ(dsc_hashset_size(new_set), 3);

    // Verify all elements are in the new set
    for (int i = 0; i < 3; i++)
    {
        ASSERT(dsc_hashset_contains(new_set, elements[i]));
    }

    it.destroy(&it);
    dsc_hashset_destroy(original_set, false);
    dsc_hashset_destroy(new_set, true);  // Free the copied string keys
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test from_iterator with NULL parameters
int test_hashset_from_iterator_null_params(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashSet* set = dsc_hashset_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);
    DSCIterator it = dsc_hashset_iterator(set);

    // Test NULL iterator
    ASSERT_NULL(dsc_hashset_from_iterator(NULL, alloc, dsc_hash_string, dsc_key_equals_string));

    // Test NULL allocator
    ASSERT_NULL(dsc_hashset_from_iterator(&it, NULL, dsc_hash_string, dsc_key_equals_string));

    // Test NULL hash function
    ASSERT_NULL(dsc_hashset_from_iterator(&it, alloc, NULL, dsc_key_equals_string));

    // Test NULL key_equals function
    ASSERT_NULL(dsc_hashset_from_iterator(&it, alloc, dsc_hash_string, NULL));

    it.destroy(&it);
    dsc_hashset_destroy(set, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test iterator multiple instances
int test_hashset_iterator_multiple(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashSet* set = dsc_hashset_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    ASSERT_EQ(dsc_hashset_add(set, "key1"), 0);
    ASSERT_EQ(dsc_hashset_add(set, "key2"), 0);
    ASSERT_EQ(dsc_hashset_add(set, "key3"), 0);

    // Create multiple iterators
    DSCIterator it1 = dsc_hashset_iterator(set);
    DSCIterator it2 = dsc_hashset_iterator(set);

    // Both should be valid and independent
    ASSERT(it1.is_valid(&it1));
    ASSERT(it2.is_valid(&it2));

    // Advance first iterator
    ASSERT(it1.has_next(&it1));
    it1.next(&it1);

    // Second iterator should still be at the beginning
    ASSERT(it2.has_next(&it2));

    // Both should iterate through all elements
    int count1 = 1; // Already advanced once
    while (it1.has_next(&it1))
    {
        it1.next(&it1);
        count1++;
    }
    ASSERT_EQ(count1, 3);

    int count2 = 0;
    while (it2.has_next(&it2))
    {
        it2.next(&it2);
        count2++;
    }
    ASSERT_EQ(count2, 3);

    it1.destroy(&it1);
    it2.destroy(&it2);
    dsc_hashset_destroy(set, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Main test runner
typedef struct
{
    int (*func)(void);
    const char* name;
} TestCase;

int main(void)
{
    const TestCase tests[] = {
        {test_hashset_iterator_basic, "test_hashset_iterator_basic"},
        {test_hashset_iterator_empty, "test_hashset_iterator_empty"},
        {test_hashset_iterator_reset, "test_hashset_iterator_reset"},
        {test_hashset_iterator_get, "test_hashset_iterator_get"},
        {test_hashset_iterator_backward, "test_hashset_iterator_backward"},
        {test_hashset_iterator_null_set, "test_hashset_iterator_null_set"},
        {test_hashset_from_iterator, "test_hashset_from_iterator"},
        {test_hashset_from_iterator_null_params, "test_hashset_from_iterator_null_params"},
        {test_hashset_iterator_multiple, "test_hashset_iterator_multiple"},
    };

    printf("Running HashSet Iterator tests...\n");

    int failed = 0;
    const int num_tests = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < num_tests; i++)
    {
        printf("Running %s... ", tests[i].name);
        fflush(stdout);

        const int result = tests[i].func();
        if (result == TEST_SUCCESS)
        {
            printf("PASSED\n");
        }
        else
        {
            printf("FAILED\n");
            failed++;
        }
    }

    if (failed == 0)
    {
        printf("All HashSet Iterator tests passed!\n");
        return 0;
    }
    printf("%d HashSet Iterator tests failed!\n", failed);
    return 1;
}
