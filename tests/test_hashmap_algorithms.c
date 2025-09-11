//
// HashMap algorithms test - converted from HashTable algorithms test
//

#include "TestAssert.h"
#include "TestHelpers.h"
#include "HashMap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Test hash map copying (shallow)
int test_hashmap_copy_shallow(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCHashMap* original = dsc_hashmap_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);

    // Add some test data
    const char* keys[] = {"apple", "banana", "cherry", "date"};
    const char* values[] = {"red", "yellow", "red", "brown"};

    for (int i = 0; i < 4; i++)
    {
        ASSERT_EQ(dsc_hashmap_put(original, (void*)keys[i], (void*)values[i]), 0);
    }

    // Create shallow copy
    DSCHashMap* copy = dsc_hashmap_copy(original);
    ASSERT_NOT_NULL(copy);
    ASSERT_EQ(dsc_hashmap_size(copy), 4);

    // Verify data is shared (same pointers)
    for (int i = 0; i < 4; i++)
    {
        void* orig_value = dsc_hashmap_get(original, keys[i]);
        void* copy_value = dsc_hashmap_get(copy, keys[i]);
        ASSERT_EQ_PTR(orig_value, copy_value); // Should be same pointer
        ASSERT_EQ_STR((char*)orig_value, values[i]);
    }

    dsc_hashmap_destroy(original, false, false);
    dsc_hashmap_destroy(copy, false, false);
    return TEST_SUCCESS;
}

// Test hash map copying (deep)
int test_hashmap_copy_deep(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCHashMap* original = dsc_hashmap_create(&alloc, dsc_hash_int, dsc_key_equals_int, 0);

    // Add some test data
    for (int i = 0; i < 3; i++)
    {
        int* key = malloc(sizeof(int));
        int* value = malloc(sizeof(int));
        *key = i;
        *value = i * 10;
        ASSERT_EQ(dsc_hashmap_put(original, key, value), 0);
    }

    // Create deep copy
    DSCHashMap* copy = dsc_hashmap_copy_deep(original, int_copy, int_copy);
    ASSERT_NOT_NULL(copy);
    ASSERT_EQ(dsc_hashmap_size(copy), 3);

    // Verify data is different (different pointers, same values)
    for (int i = 0; i < 3; i++)
    {
        int* orig_value = (int*)dsc_hashmap_get(original, &i);
        int* copy_value = (int*)dsc_hashmap_get(copy, &i);
        ASSERT_NOT_EQ_PTR(orig_value, copy_value); // Should be different pointers
        ASSERT_EQ(*orig_value, *copy_value); // Same values
        ASSERT_EQ(*orig_value, i * 10);
    }

    dsc_hashmap_destroy(original, true, true);
    dsc_hashmap_destroy(copy, true, true);
    return TEST_SUCCESS;
}

// Helper function for for_each test
static void increment_value(void* key, void* value) {
    (void)key; // Unused
    int* val = (int*)value;
    (*val)++;
}

// Test for_each functionality
int test_hashmap_for_each(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCHashMap* map = dsc_hashmap_create(&alloc, dsc_hash_int, dsc_key_equals_int, 0);

    // Add some test data
    for (int i = 1; i <= 5; i++)
    {
        int* key = malloc(sizeof(int));
        int* value = malloc(sizeof(int));
        *key = i;
        *value = i * 10;
        ASSERT_EQ(dsc_hashmap_put(map, key, value), 0);
    }

    // Test for_each with action that increments values
    dsc_hashmap_for_each(map, increment_value);

    // Verify values were incremented
    for (int i = 1; i <= 5; i++)
    {
        int* value = (int*)dsc_hashmap_get(map, &i);
        ASSERT_EQ(*value, i * 10 + 1);
    }

    // Test with NULL parameters
    dsc_hashmap_for_each(NULL, increment_value); // Should be safe
    dsc_hashmap_for_each(map, NULL);           // Should be safe

    dsc_hashmap_destroy(map, true, true);
    return TEST_SUCCESS;
}

// Test get_keys functionality
int test_hashmap_get_keys(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCHashMap* map = dsc_hashmap_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);

    const char* keys[] = {"alpha", "beta", "gamma"};
    const char* values[] = {"1", "2", "3"};

    // Add test data
    for (int i = 0; i < 3; i++)
    {
        ASSERT_EQ(dsc_hashmap_put(map, (void*)keys[i], (void*)values[i]), 0);
    }

    // Get all keys
    void** retrieved_keys;
    size_t count;
    ASSERT_EQ(dsc_hashmap_get_keys(map, &retrieved_keys, &count), 0);
    ASSERT_EQ(count, 3);
    ASSERT_NOT_NULL(retrieved_keys);

    // Verify all keys are present (order may vary)
    int found_count = 0;
    for (size_t i = 0; i < count; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (strcmp((char*)retrieved_keys[i], keys[j]) == 0)
            {
                found_count++;
                break;
            }
        }
    }
    ASSERT_EQ(found_count, 3);

    free(retrieved_keys);
    dsc_hashmap_destroy(map, false, false);
    return TEST_SUCCESS;
}

// Test get_values functionality
int test_hashmap_get_values(void)
{
    DSCAllocator alloc = create_int_allocator();
    DSCHashMap* map = dsc_hashmap_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);

    const char* keys[] = {"x", "y", "z"};
    const char* values[] = {"10", "20", "30"};

    // Add test data
    for (int i = 0; i < 3; i++)
    {
        ASSERT_EQ(dsc_hashmap_put(map, (void*)keys[i], (void*)values[i]), 0);
    }

    // Get all values
    void** retrieved_values;
    size_t count;
    ASSERT_EQ(dsc_hashmap_get_values(map, &retrieved_values, &count), 0);
    ASSERT_EQ(count, 3);
    ASSERT_NOT_NULL(retrieved_values);

    // Verify all values are present (order may vary)
    int found_count = 0;
    for (size_t i = 0; i < count; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (strcmp((char*)retrieved_values[i], values[j]) == 0)
            {
                found_count++;
                break;
            }
        }
    }
    ASSERT_EQ(found_count, 3);

    free(retrieved_values);
    dsc_hashmap_destroy(map, false, false);
    return TEST_SUCCESS;
}

// Test from_iterator functionality
int test_hashmap_from_iterator(void)
{
    DSCAllocator alloc = create_string_allocator();
    DSCHashMap* original = dsc_hashmap_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);

    const char* keys[] = {"key1", "key2", "key3"};
    const char* values[] = {"val1", "val2", "val3"};

    // Add test data to original
    for (int i = 0; i < 3; i++)
    {
        ASSERT_EQ(dsc_hashmap_put(original, (void*)keys[i], (void*)values[i]), 0);
    }

    // Create iterator from original
    DSCIterator it = dsc_hashmap_iterator(original);

    // Create new map from iterator
    DSCHashMap* new_map = dsc_hashmap_from_iterator(&it, &alloc, dsc_hash_string, dsc_key_equals_string);
    ASSERT_NOT_NULL(new_map);
    ASSERT_EQ(dsc_hashmap_size(new_map), 3);

    // Verify all data was copied
    for (int i = 0; i < 3; i++)
    {
        void* value = dsc_hashmap_get(new_map, keys[i]);
        ASSERT_NOT_NULL(value);
        ASSERT_EQ_STR((char*)value, values[i]);
    }

    it.destroy(&it);
    dsc_hashmap_destroy(original, false, false);
    dsc_hashmap_destroy(new_map, false, false);
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
        {test_hashmap_copy_shallow, "test_hashmap_copy_shallow"},
        {test_hashmap_copy_deep, "test_hashmap_copy_deep"},
        {test_hashmap_for_each, "test_hashmap_for_each"},
        {test_hashmap_get_keys, "test_hashmap_get_keys"},
        {test_hashmap_get_values, "test_hashmap_get_values"},
        {test_hashmap_from_iterator, "test_hashmap_from_iterator"},
    };

    printf("Running HashMap algorithms tests...\n");

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
        printf("All HashMap algorithms tests passed!\n");
        return 0;
    }

    printf("%d HashMap algorithms tests failed.\n", failed);
    return 1;
}
