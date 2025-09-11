//
// HashMap properties test - converted from HashTable properties test
//

#include "TestAssert.h"
#include "TestHelpers.h"
#include "HashMap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Test hash map size property
int test_hashmap_size_property(void)
{
    DSCAlloc alloc = dsc_alloc_default();
    DSCHashMap* map = dsc_hashmap_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);

    // Declare arrays outside loops to avoid scope issues
    char keys[10][10], values[10][10];

    // Initially empty
    ASSERT_EQ(dsc_hashmap_size(map), 0);
    ASSERT(dsc_hashmap_is_empty(map));

    // Add items and verify size increases
    for (int i = 0; i < 10; i++)
    {
        sprintf(keys[i], "key%d", i);
        sprintf(values[i], "val%d", i);

        ASSERT_EQ(dsc_hashmap_put(map, keys[i], values[i]), 0);
        ASSERT_EQ(dsc_hashmap_size(map), (size_t)i + 1);
        ASSERT(!dsc_hashmap_is_empty(map));
    }

    // Remove items and verify size decreases
    for (int i = 0; i < 5; i++)
    {
        ASSERT_EQ(dsc_hashmap_remove(map, keys[i], false, false), 0);
        ASSERT_EQ(dsc_hashmap_size(map), 10 - (size_t)i - 1);
    }

    // Clear and verify empty
    dsc_hashmap_clear(map, false, false);
    ASSERT_EQ(dsc_hashmap_size(map), 0);
    ASSERT(dsc_hashmap_is_empty(map));

    dsc_hashmap_destroy(map, false, false);
    return TEST_SUCCESS;
}

// Test hash map uniqueness property
int test_hashmap_uniqueness_property(void)
{
    DSCAlloc alloc = dsc_alloc_default();
    DSCHashMap* map = dsc_hashmap_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);

    char* key = "duplicate_key";
    char* value1 = "first_value";
    char* value2 = "second_value";
    char* value3 = "third_value";

    // Test 1: Basic uniqueness with string literals (no memory leak concern)
    ASSERT_EQ(dsc_hashmap_put(map, key, value1), 0);
    ASSERT_EQ(dsc_hashmap_size(map), 1);
    ASSERT_EQ_STR((char*)dsc_hashmap_get(map, key), value1);

    ASSERT_EQ(dsc_hashmap_put(map, key, value2), 0);
    ASSERT_EQ(dsc_hashmap_size(map), 1); // Size should remain 1
    ASSERT_EQ_STR((char*)dsc_hashmap_get(map, key), value2);

    ASSERT_EQ(dsc_hashmap_put(map, key, value3), 0);
    ASSERT_EQ(dsc_hashmap_size(map), 1); // Size should remain 1
    ASSERT_EQ_STR((char*)dsc_hashmap_get(map, key), value3);

    dsc_hashmap_clear(map, false, false);

    // Test 2: Memory-safe replacement with put_replace
    char* heap_value1 = dsc_alloc_malloc(&alloc, 20);
    char* heap_value2 = dsc_alloc_malloc(&alloc, 20);
    strcpy(heap_value1, "heap_first");
    strcpy(heap_value2, "heap_second");

    // Insert first value
    void* old_value = NULL;
    ASSERT_EQ(dsc_hashmap_put_replace(map, key, heap_value1, &old_value), 0);
    ASSERT_EQ(dsc_hashmap_size(map), 1);
    ASSERT_EQ(old_value, NULL); // No old value for new key
    ASSERT_EQ_STR((char*)dsc_hashmap_get(map, key), "heap_first");

    // Replace with second value - get old value back for proper cleanup
    ASSERT_EQ(dsc_hashmap_put_replace(map, key, heap_value2, &old_value), 0);
    ASSERT_EQ(dsc_hashmap_size(map), 1); // Size should remain 1
    ASSERT_EQ(old_value, heap_value1); // Should return the old value
    ASSERT_EQ_STR((char*)dsc_hashmap_get(map, key), "heap_second");

    // Clean up the old value (no memory leak!)
    dsc_alloc_free(&alloc, old_value);

    // Clean up remaining value
    char* final_value = (char*)dsc_hashmap_get(map, key);
    dsc_hashmap_clear(map, false, false);
    dsc_alloc_free(&alloc, final_value);

    // Test 3: Automatic cleanup with put_with_free
    // Note: This test uses string literals since we can't easily test with malloc'd values
    // in a unit test that uses the map's allocator for cleanup
    ASSERT_EQ(dsc_hashmap_put_with_free(map, key, "auto_first", false), 0);
    ASSERT_EQ(dsc_hashmap_size(map), 1);
    ASSERT_EQ_STR((char*)dsc_hashmap_get(map, key), "auto_first");

    ASSERT_EQ(dsc_hashmap_put_with_free(map, key, "auto_second", false), 0);
    ASSERT_EQ(dsc_hashmap_size(map), 1); // Size should remain 1
    ASSERT_EQ_STR((char*)dsc_hashmap_get(map, key), "auto_second");

    dsc_hashmap_destroy(map, false, false);
    return TEST_SUCCESS;
}

// Test hash map load factor property
int test_hashmap_load_factor_property(void)
{
    DSCAlloc alloc = dsc_alloc_default();
    DSCHashMap* map = dsc_hashmap_create(&alloc, dsc_hash_string, dsc_key_equals_string, 8);

    // Declare arrays outside loop to avoid scope issues
    char keys[4][10], values[4][10];

    // Initially empty, load factor should be 0
    ASSERT_EQ(dsc_hashmap_load_factor(map), 0.0);

    // Add items and check load factor increases
    for (int i = 0; i < 4; i++)
    {
        sprintf(keys[i], "key%d", i);
        sprintf(values[i], "val%d", i);

        ASSERT_EQ(dsc_hashmap_put(map, keys[i], values[i]), 0);

        double expected_lf = (double)(i + 1) / 8.0;
        double actual_lf = dsc_hashmap_load_factor(map);
        ASSERT(actual_lf >= expected_lf - 0.01 && actual_lf <= expected_lf + 0.01);
    }

    // Load factor should be 0.5 with 4 items in 8 buckets
    double lf = dsc_hashmap_load_factor(map);
    ASSERT(lf >= 0.49 && lf <= 0.51);

    dsc_hashmap_destroy(map, false, false);
    return TEST_SUCCESS;
}

// Test hash map automatic resizing property
int test_hashmap_resize_property(void)
{
    DSCAlloc alloc = dsc_alloc_default();
    DSCHashMap* map = dsc_hashmap_create(&alloc, dsc_hash_string, dsc_key_equals_string, 4);

    // Declare arrays outside loops to avoid scope issues
    char keys[4][10], values[4][10];

    // Add items until resize triggers (load factor > 0.75)
    int items_added = 0;
    double initial_lf = 0.0;

    // Add 3 items (load factor = 0.75, at threshold)
    for (int i = 0; i < 3; i++)
    {
        sprintf(keys[i], "key%d", i);
        sprintf(values[i], "val%d", i);

        ASSERT_EQ(dsc_hashmap_put(map, keys[i], values[i]), 0);
        items_added++;
    }

    initial_lf = dsc_hashmap_load_factor(map);
    ASSERT(initial_lf >= 0.74 && initial_lf <= 0.76); // Should be ~0.75

    // Add one more item to trigger resize
    ASSERT_EQ(dsc_hashmap_put(map, "trigger", "resize"), 0);
    items_added++;

    // After resize, load factor should be lower
    double post_resize_lf = dsc_hashmap_load_factor(map);
    ASSERT(post_resize_lf < initial_lf);
    ASSERT_EQ(dsc_hashmap_size(map), (size_t)items_added);

    // All original items should still be accessible
    for (int i = 0; i < 3; i++)
    {
        ASSERT_NOT_NULL(dsc_hashmap_get(map, keys[i]));
    }
    ASSERT_NOT_NULL(dsc_hashmap_get(map, "trigger"));

    dsc_hashmap_destroy(map, false, false);
    return TEST_SUCCESS;
}

// Test hash map key equality property
int test_hashmap_key_equality_property(void)
{
    DSCAlloc alloc = dsc_alloc_default();
    DSCHashMap* map = dsc_hashmap_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);

    // String literals might have different addresses but same content
    char* key1 = "test_key";
    char key2[] = "test_key";
    char* value1 = "value1";
    char* value2 = "value2";

    // Insert with first key
    ASSERT_EQ(dsc_hashmap_put(map, key1, value1), 0);
    ASSERT_EQ(dsc_hashmap_size(map), 1);

    // Update with equivalent key (different pointer, same content)
    ASSERT_EQ(dsc_hashmap_put(map, key2, value2), 0);
    ASSERT_EQ(dsc_hashmap_size(map), 1); // Should be updated, not new entry

    // Both keys should retrieve the updated value
    ASSERT_EQ_STR((char*)dsc_hashmap_get(map, key1), value2);
    ASSERT_EQ_STR((char*)dsc_hashmap_get(map, key2), value2);

    dsc_hashmap_destroy(map, false, false);
    return TEST_SUCCESS;
}

// Test hash map contains property
int test_hashmap_contains_property(void)
{
    DSCAlloc alloc = dsc_alloc_default();
    DSCHashMap* map = dsc_hashmap_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);

    char* keys[] = {"apple", "banana", "cherry"};
    char* values[] = {"red", "yellow", "red"};

    // Initially no keys present
    for (int i = 0; i < 3; i++)
    {
        ASSERT(!dsc_hashmap_contains_key(map, keys[i]));
    }

    // Add keys one by one and verify contains property
    for (int i = 0; i < 3; i++)
    {
        ASSERT_EQ(dsc_hashmap_put(map, keys[i], values[i]), 0);

        // Current key should be contained
        ASSERT(dsc_hashmap_contains_key(map, keys[i]));

        // All previously added keys should still be contained
        for (int j = 0; j <= i; j++)
        {
            ASSERT(dsc_hashmap_contains_key(map, keys[j]));
        }

        // Keys not yet added should not be contained
        for (int j = i + 1; j < 3; j++)
        {
            ASSERT(!dsc_hashmap_contains_key(map, keys[j]));
        }
    }

    // Remove keys and verify contains property
    for (int i = 0; i < 3; i++)
    {
        ASSERT_EQ(dsc_hashmap_remove(map, keys[i], false, false), 0);

        // Removed key should not be contained
        ASSERT(!dsc_hashmap_contains_key(map, keys[i]));

        // Remaining keys should still be contained
        for (int j = i + 1; j < 3; j++)
        {
            ASSERT(dsc_hashmap_contains_key(map, keys[j]));
        }
    }

    dsc_hashmap_destroy(map, false, false);
    return TEST_SUCCESS;
}

// Test hash map iterator completeness property
int test_hashmap_iterator_completeness(void)
{
    DSCAlloc alloc = dsc_alloc_default();
    DSCHashMap* map = dsc_hashmap_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);

    const int num_items = 20;
    char keys[20][10];
    char values[20][10];

    // Add test data
    for (int i = 0; i < num_items; i++)
    {
        sprintf(keys[i], "key%d", i);
        sprintf(values[i], "val%d", i);
        ASSERT_EQ(dsc_hashmap_put(map, keys[i], values[i]), 0);
    }

    // Use iterator to visit all items
    DSCIterator it = dsc_hashmap_iterator(map);
    int visited_count = 0;
    int found_flags[20] = {0}; // Track which items we've seen

    while (it.has_next(&it))
    {
        DSCKeyValuePair* pair = (DSCKeyValuePair*)it.next(&it);
        ASSERT_NOT_NULL(pair);

        // Find which item this is
        int found_index = -1;
        for (int i = 0; i < num_items; i++)
        {
            if (strcmp((char*)pair->key, keys[i]) == 0)
            {
                found_index = i;
                break;
            }
        }

        ASSERT(found_index >= 0); // Should find the key
        ASSERT(!found_flags[found_index]); // Should not have seen it before
        found_flags[found_index] = 1;

        // Verify value matches
        ASSERT_EQ_STR((char*)pair->value, values[found_index]);
        visited_count++;
    }

    // Should have visited exactly all items
    ASSERT_EQ(visited_count, num_items);

    // Should have seen each item exactly once
    for (int i = 0; i < num_items; i++)
    {
        ASSERT(found_flags[i]);
    }

    it.destroy(&it);
    dsc_hashmap_destroy(map, false, false);
    return TEST_SUCCESS;
}

// Test hash map with different hash functions
int test_hashmap_hash_function_property(void)
{
    DSCAlloc alloc = dsc_alloc_default();

    // Test with string hash
    DSCHashMap* str_map = dsc_hashmap_create(&alloc, dsc_hash_string, dsc_key_equals_string, 0);
    ASSERT_EQ(dsc_hashmap_put(str_map, "test", "value"), 0);
    ASSERT_EQ_STR((char*)dsc_hashmap_get(str_map, "test"), "value");

    // Test with integer hash
    DSCHashMap* int_map = dsc_hashmap_create(&alloc, dsc_hash_int, dsc_key_equals_int, 0);
    int key = 42;
    ASSERT_EQ(dsc_hashmap_put(int_map, &key, "forty-two"), 0);
    ASSERT_EQ_STR((char*)dsc_hashmap_get(int_map, &key), "forty-two");

    // Test with pointer hash
    DSCHashMap* ptr_map = dsc_hashmap_create(&alloc, dsc_hash_pointer, dsc_key_equals_pointer, 0);
    void* ptr_key = (void*)0x12345678;
    ASSERT_EQ(dsc_hashmap_put(ptr_map, ptr_key, "pointer_value"), 0);
    ASSERT_EQ_STR((char*)dsc_hashmap_get(ptr_map, ptr_key), "pointer_value");

    dsc_hashmap_destroy(str_map, false, false);
    dsc_hashmap_destroy(int_map, false, false);
    dsc_hashmap_destroy(ptr_map, false, false);
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
        {test_hashmap_size_property, "test_hashmap_size_property"},
        {test_hashmap_uniqueness_property, "test_hashmap_uniqueness_property"},
        {test_hashmap_load_factor_property, "test_hashmap_load_factor_property"},
        {test_hashmap_resize_property, "test_hashmap_resize_property"},
        {test_hashmap_key_equality_property, "test_hashmap_key_equality_property"},
        {test_hashmap_contains_property, "test_hashmap_contains_property"},
        {test_hashmap_iterator_completeness, "test_hashmap_iterator_completeness"},
        {test_hashmap_hash_function_property, "test_hashmap_hash_function_property"},
    };

    printf("Running HashMap properties tests...\n");

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
        printf("All HashMap properties tests passed!\n");
        return 0;
    }

    printf("%d HashMap properties tests failed.\n", failed);
    return 1;
}
