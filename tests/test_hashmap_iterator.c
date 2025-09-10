//
// HashMap iterator test - converted from HashTable iterator test
//

#include "TestAssert.h"
#include "TestHelpers.h"
#include "HashMap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Test basic iterator functionality
int test_hashmap_iterator_basic(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashMap* map = dsc_hashmap_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    const char* keys[] = {"key1", "key2", "key3", "key4", "key5"};
    const char* values[] = {"val1", "val2", "val3", "val4", "val5"};
    const int num_items = 5;

    // Add test data
    for (int i = 0; i < num_items; i++)
    {
        ASSERT_EQ(dsc_hashmap_put(map, (void*)keys[i], (void*)values[i]), 0);
    }

    // Test iterator
    DSCIterator it = dsc_hashmap_iterator(map);
    ASSERT(it.is_valid(&it));

    int visited_count = 0;
    bool found[5] = {false, false, false, false, false};

    while (it.has_next(&it))
    {
        DSCKeyValuePair* pair = (DSCKeyValuePair*)it.next(&it);
        ASSERT_NOT_NULL(pair);
        ASSERT_NOT_NULL(pair->key);
        ASSERT_NOT_NULL(pair->value);

        // Find which item this is
        for (int i = 0; i < num_items; i++)
        {
            if (strcmp((char*)pair->key, keys[i]) == 0)
            {
                ASSERT_EQ_STR((char*)pair->value, values[i]);
                ASSERT(!found[i]); // Should not have seen this before
                found[i] = true;
                break;
            }
        }
        visited_count++;
    }

    // Verify we visited all items exactly once
    ASSERT_EQ(visited_count, num_items);
    for (int i = 0; i < num_items; i++)
    {
        ASSERT(found[i]);
    }

    it.destroy(&it);
    dsc_hashmap_destroy(map, false, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test iterator with empty map
int test_hashmap_iterator_empty(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashMap* map = dsc_hashmap_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    DSCIterator it = dsc_hashmap_iterator(map);
    ASSERT(it.is_valid(&it));
    ASSERT(!it.has_next(&it));
    ASSERT_NULL(it.next(&it));

    it.destroy(&it);
    dsc_hashmap_destroy(map, false, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test iterator reset functionality
int test_hashmap_iterator_reset(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashMap* map = dsc_hashmap_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    const char* keys[] = {"a", "b", "c"};
    const char* values[] = {"1", "2", "3"};

    for (int i = 0; i < 3; i++)
    {
        ASSERT_EQ(dsc_hashmap_put(map, (void*)keys[i], (void*)values[i]), 0);
    }

    DSCIterator it = dsc_hashmap_iterator(map);

    // First iteration
    int first_count = 0;
    while (it.has_next(&it))
    {
        it.next(&it);
        first_count++;
    }
    ASSERT_EQ(first_count, 3);

    // Reset and iterate again
    it.reset(&it);
    int second_count = 0;
    while (it.has_next(&it))
    {
        it.next(&it);
        second_count++;
    }
    ASSERT_EQ(second_count, 3);

    it.destroy(&it);
    dsc_hashmap_destroy(map, false, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test iterator with single item
int test_hashmap_iterator_single(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashMap* map = dsc_hashmap_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    char* key = "single";
    char* value = "item";

    ASSERT_EQ(dsc_hashmap_put(map, key, value), 0);

    DSCIterator it = dsc_hashmap_iterator(map);
    ASSERT(it.has_next(&it));

    DSCKeyValuePair* pair = (DSCKeyValuePair*)it.next(&it);
    ASSERT_NOT_NULL(pair);
    ASSERT_EQ_STR((char*)pair->key, key);
    ASSERT_EQ_STR((char*)pair->value, value);

    ASSERT(!it.has_next(&it));

    it.destroy(&it);
    dsc_hashmap_destroy(map, false, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test iterator get method
int test_hashmap_iterator_get(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashMap* map = dsc_hashmap_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    char* key = "test";
    char* value = "data";

    ASSERT_EQ(dsc_hashmap_put(map, key, value), 0);

    DSCIterator it = dsc_hashmap_iterator(map);

    // Test get before calling next
    DSCKeyValuePair* pair = (DSCKeyValuePair*)it.get(&it);
    ASSERT_NOT_NULL(pair);
    ASSERT_EQ_STR((char*)pair->key, key);
    ASSERT_EQ_STR((char*)pair->value, value);

    // Move to next (which should be NULL for single item)
    it.next(&it);
    ASSERT_NULL(it.get(&it));

    it.destroy(&it);
    dsc_hashmap_destroy(map, false, false);
    destroy_allocator(alloc);
    return TEST_SUCCESS;
}

// Test iterator backward operations (should not be supported)
int test_hashmap_iterator_backward(void)
{
    DSCAlloc* alloc = create_std_allocator();
    DSCHashMap* map = dsc_hashmap_create(alloc, dsc_hash_string, dsc_key_equals_string, 0);

    ASSERT_EQ(dsc_hashmap_put(map, "key", "value"), 0);

    DSCIterator it = dsc_hashmap_iterator(map);

    // HashMap iterator should not support backward iteration
    ASSERT(!it.has_prev(&it));
    ASSERT_NULL(it.prev(&it));

    it.destroy(&it);
    dsc_hashmap_destroy(map, false, false);
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
        {test_hashmap_iterator_basic, "test_hashmap_iterator_basic"},
        {test_hashmap_iterator_empty, "test_hashmap_iterator_empty"},
        {test_hashmap_iterator_reset, "test_hashmap_iterator_reset"},
        {test_hashmap_iterator_single, "test_hashmap_iterator_single"},
        {test_hashmap_iterator_get, "test_hashmap_iterator_get"},
        {test_hashmap_iterator_backward, "test_hashmap_iterator_backward"},
    };

    printf("Running HashMap iterator tests...\n");

    int failed = 0;
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
        printf("All HashMap iterator tests passed!\n");
        return 0;
    }

    printf("%d HashMap iterator tests failed.\n", failed);
    return 1;
}
