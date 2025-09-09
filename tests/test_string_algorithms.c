//
// Created by zack on 9/3/25.
//

#include "DynamicString.h"
#include "TestAssert.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int test_find_and_compare(void) {
    DSCString str = dsc_str_create_from_cstring("abcdefgabc");
    ASSERT_EQ(dsc_str_find_cstring(&str, "abc"), 0);
    ASSERT_EQ(dsc_str_find_cstring(&str, "fg"), 5);
    ASSERT_EQ(dsc_str_find_cstring(&str, "xyz"), STR_NPOS);
    ASSERT_EQ(dsc_str_compare_cstring(&str, "abcdefgabc"), 0);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_trim_and_case(void) {
    DSCString str = dsc_str_create_from_cstring("   Hello World!   ");
    dsc_str_trim_front(&str);
    ASSERT_EQ_STR(dsc_str_data(&str), "Hello World!   ");
    dsc_str_trim_back(&str);
    ASSERT_EQ_STR(dsc_str_data(&str), "Hello World!");
    dsc_str_to_lower(&str);
    ASSERT_EQ_STR(dsc_str_data(&str), "hello world!");
    dsc_str_to_upper(&str);
    ASSERT_EQ_STR(dsc_str_data(&str), "HELLO WORLD!");
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_substr(void) {
    DSCString str = dsc_str_create_from_cstring("abcdef");
    DSCString sub = dsc_str_substr_create_string(&str, 2, 3);
    ASSERT_EQ_STR(dsc_str_data(&sub), "cde");
    dsc_str_destroy(&sub);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_substr_out_of_bounds(void) {
    DSCString str = dsc_str_create_from_cstring("abc");
    DSCString sub1 = dsc_str_substr_create_string(&str, 10, 2);
    ASSERT_EQ(dsc_str_size(&sub1), 0);
    ASSERT_EQ_STR(dsc_str_data(&sub1), "");
    DSCString sub2 = dsc_str_substr_create_string(&str, 1, 10);
    ASSERT_EQ_STR(dsc_str_data(&sub2), "bc");
    dsc_str_destroy(&sub1);
    dsc_str_destroy(&sub2);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_embedded_null(void) {
    DSCString str = dsc_str_create_empty(16);
    const char data[] = {'a', 'b', '\0', 'c', 'd', '\0'};
    dsc_str_assign_cstring(&str, data);
    ASSERT_EQ(dsc_str_size(&str), 2); // Only up to first null
    ASSERT_EQ_STR(dsc_str_data(&str), "ab");
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_trim_all_whitespace(void) {
    DSCString str = dsc_str_create_from_cstring("    \t\n  ");
    dsc_str_trim_front(&str);
    dsc_str_trim_back(&str);
    ASSERT_TRUE(dsc_str_empty(&str) || dsc_str_size(&str) == 0);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_compare_different_lengths(void) {
    DSCString str1 = dsc_str_create_from_cstring("abc");
    DSCString str2 = dsc_str_create_from_cstring("abcd");
    ASSERT_LT(dsc_str_compare_string(&str1, &str2), 0);
    ASSERT_GT(dsc_str_compare_string(&str2, &str1), 0);
    dsc_str_destroy(&str1);
    dsc_str_destroy(&str2);
    return TEST_SUCCESS;
}

int test_compare_different_contents(void) {
    DSCString str1 = dsc_str_create_from_cstring("abc");
    DSCString str2 = dsc_str_create_from_cstring("abd");
    ASSERT_LT(dsc_str_compare_string(&str1, &str2), 0);
    ASSERT_GT(dsc_str_compare_string(&str2, &str1), 0);
    dsc_str_destroy(&str1);
    dsc_str_destroy(&str2);
    return TEST_SUCCESS;
}

int test_find_first_of_no_match(void) {
    DSCString str = dsc_str_create_from_cstring("abcdef");
    ASSERT_EQ(dsc_str_find_first_of(&str, "xyz"), STR_NPOS);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_find_first_of_multiple_matches(void) {
    DSCString str = dsc_str_create_from_cstring("abcdef");
    ASSERT_EQ(dsc_str_find_first_of(&str, "fa"), 0); // 'a' at pos 0
    ASSERT_EQ(dsc_str_find_first_of(&str, "f"), 5); // 'f' at pos 5
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_remove_extra_ws(void) {
    DSCString str = dsc_str_create_from_cstring("  a   b\t\tc  ");
    dsc_str_remove_extra_ws(&str);
    ASSERT_EQ_STR(dsc_str_data(&str), "a b c");
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_to_lower_upper_already(void) {
    DSCString str1 = dsc_str_create_from_cstring("abc");
    dsc_str_to_lower(&str1);
    ASSERT_EQ_STR(dsc_str_data(&str1), "abc");
    dsc_str_to_upper(&str1);
    ASSERT_EQ_STR(dsc_str_data(&str1), "ABC");
    dsc_str_destroy(&str1);

    DSCString str2 = dsc_str_create_from_cstring("XYZ");
    dsc_str_to_upper(&str2);
    ASSERT_EQ_STR(dsc_str_data(&str2), "XYZ");
    dsc_str_to_lower(&str2);
    ASSERT_EQ_STR(dsc_str_data(&str2), "xyz");
    dsc_str_destroy(&str2);
    return TEST_SUCCESS;
}

int test_substr_create_zero_count(void) {
    DSCString str = dsc_str_create_from_cstring("abcdef");
    DSCString sub = dsc_str_substr_create_string(&str, 2, 0);
    ASSERT_EQ(dsc_str_size(&sub), 0);
    ASSERT_EQ_STR(dsc_str_data(&sub), "");
    dsc_str_destroy(&sub);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_compare_string_equality(void) {
    DSCString str1 = dsc_str_create_from_cstring("abc");
    DSCString str2 = dsc_str_create_from_cstring("abc");
    ASSERT_EQ(dsc_str_compare_string(&str1, &str2), 0);
    ASSERT_EQ(dsc_str_compare_string(&str2, &str1), 0);
    dsc_str_destroy(&str1);
    dsc_str_destroy(&str2);
    return TEST_SUCCESS;
}

int test_substr_create_cstring_cases(void) {
    const char *src = "abcdef";
    DSCString sub1 = dsc_str_substr_create_cstring(src, 0, 2);
    ASSERT_EQ_STR(dsc_str_data(&sub1), "ab");
    DSCString sub2 = dsc_str_substr_create_cstring(src, 4, 10); // count > length
    ASSERT_EQ_STR(dsc_str_data(&sub2), "ef");
    DSCString sub3 = dsc_str_substr_create_cstring(src, 10, 2); // pos > length
    ASSERT_EQ(dsc_str_size(&sub3), 0);
    dsc_str_destroy(&sub1);
    dsc_str_destroy(&sub2);
    dsc_str_destroy(&sub3);
    return TEST_SUCCESS;
}

int test_compare_cstring_prefix_suffix(void) {
    DSCString str = dsc_str_create_from_cstring("abc");
    ASSERT_LT(dsc_str_compare_cstring(&str, "abcd"), 0);
    ASSERT_GT(dsc_str_compare_cstring(&str, "ab"), 0);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_getline_ch_simulated(void) {
    // Use a temporary file on disk for portability
    FILE *fp = fopen("test_tmpfile.txt", "w+");
    ASSERT_NOT_NULL(fp);
    fputs("hello\nworld", fp);
    fflush(fp);
    rewind(fp);
    DSCString line = dsc_str_create_empty(16);
    const int status = dsc_str_getline_ch(fp, &line, '\n');
    ASSERT_EQ_STR(dsc_str_data(&line), "hello");
    ASSERT_EQ(status, 0);
    dsc_str_destroy(&line);
    fclose(fp);
    remove("test_tmpfile.txt");
    return TEST_SUCCESS;
}

int test_getline_cstring_simulated(void) {
    FILE *fp = fopen("test_tmpfile2.txt", "w+");
    ASSERT_NOT_NULL(fp);
    fputs("foo,bar,baz", fp);
    fflush(fp);
    rewind(fp);
    DSCString line = dsc_str_create_empty(16);
    const int status = dsc_str_getline_cstring(fp, &line, ",");
    ASSERT_EQ_STR(dsc_str_data(&line), "foo");
    ASSERT_EQ(status, 0);
    dsc_str_destroy(&line);
    fclose(fp);
    remove("test_tmpfile2.txt");
    return TEST_SUCCESS;
}

int test_find_cstring_empty_search(void) {
    DSCString str = dsc_str_create_from_cstring("abc");
    ASSERT_EQ(dsc_str_find_cstring(&str, ""), STR_NPOS);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_find_string_empty_search(void) {
    DSCString str = dsc_str_create_from_cstring("abc");
    DSCString empty = dsc_str_create_empty(4);
    ASSERT_EQ(dsc_str_find_string(&str, &empty), STR_NPOS);
    dsc_str_destroy(&str);
    dsc_str_destroy(&empty);
    return TEST_SUCCESS;
}

int test_substr_create_string_count_exceeds(void) {
    DSCString str = dsc_str_create_from_cstring("abc");
    DSCString sub = dsc_str_substr_create_string(&str, 1, 10);
    ASSERT_EQ_STR(dsc_str_data(&sub), "bc");
    dsc_str_destroy(&sub);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_substr_create_string_pos_at_size(void) {
    DSCString str = dsc_str_create_from_cstring("abc");
    DSCString sub = dsc_str_substr_create_string(&str, dsc_str_size(&str), 2);
    ASSERT_EQ(dsc_str_size(&sub), 0);
    ASSERT_EQ_STR(dsc_str_data(&sub), "");
    dsc_str_destroy(&sub);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_substr_create_string_pos_gt_size(void) {
    DSCString str = dsc_str_create_from_cstring("abc");
    DSCString sub = dsc_str_substr_create_string(&str, dsc_str_size(&str) + 1, 2);
    ASSERT_EQ(dsc_str_size(&sub), 0);
    ASSERT_EQ_STR(dsc_str_data(&sub), "");
    dsc_str_destroy(&sub);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_substr_cstring_pos_at_length(void) {
    const char *src = "abc";
    char buf[8] = {0};
    dsc_str_substr_cstring(src, strlen(src), 2, buf);
    ASSERT_EQ_STR(buf, "");
    return TEST_SUCCESS;
}

int test_substr_cstring_pos_gt_length(void) {
    const char *src = "abc";
    char buf[8] = {0};
    dsc_str_substr_cstring(src, strlen(src) + 1, 2, buf);
    ASSERT_EQ_STR(buf, "");
    return TEST_SUCCESS;
}

int test_compare_string_empty(void) {
    DSCString str1 = dsc_str_create_empty(4);
    DSCString str2 = dsc_str_create_empty(4);
    ASSERT_EQ(dsc_str_compare_string(&str1, &str2), 0);
    dsc_str_destroy(&str1);
    dsc_str_destroy(&str2);
    return TEST_SUCCESS;
}

int test_compare_cstring_empty(void) {
    DSCString str = dsc_str_create_empty(4);
    ASSERT_EQ(dsc_str_compare_cstring(&str, ""), 0);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_getline_ch_empty_file(void) {
    FILE *fp = fopen("test_tmpfile_empty.txt", "w+");
    ASSERT_NOT_NULL(fp);
    DSCString line = dsc_str_create_empty(8);
    const int status = dsc_str_getline_ch(fp, &line, '\n');
    ASSERT_EQ(status, EOF);
    ASSERT_TRUE(dsc_str_empty(&line));
    dsc_str_destroy(&line);
    fclose(fp);
    remove("test_tmpfile_empty.txt");
    return TEST_SUCCESS;
}

int test_getline_cstring_empty_file(void) {
    FILE *fp = fopen("test_tmpfile_empty2.txt", "w+");
    ASSERT_NOT_NULL(fp);
    DSCString line = dsc_str_create_empty(8);
    const int status = dsc_str_getline_cstring(fp, &line, ",");
    ASSERT_EQ(status, EOF);
    ASSERT_TRUE(dsc_str_empty(&line));
    dsc_str_destroy(&line);
    fclose(fp);
    remove("test_tmpfile_empty2.txt");
    return TEST_SUCCESS;
}

int test_trim_front_already_trimmed(void) {
    DSCString str = dsc_str_create_from_cstring("abc");
    dsc_str_trim_front(&str);
    ASSERT_EQ_STR(dsc_str_data(&str), "abc");
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_trim_back_already_trimmed(void) {
    DSCString str = dsc_str_create_from_cstring("abc");
    dsc_str_trim_back(&str);
    ASSERT_EQ_STR(dsc_str_data(&str), "abc");
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_remove_extra_ws_only_spaces(void) {
    DSCString str = dsc_str_create_from_cstring("     ");
    dsc_str_remove_extra_ws(&str);
    ASSERT_TRUE(dsc_str_empty(&str) || dsc_str_size(&str) == 0);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_to_lower_empty(void) {
    DSCString str = dsc_str_create_empty(8);
    dsc_str_to_lower(&str);
    ASSERT_TRUE(dsc_str_empty(&str));
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_to_upper_empty(void) {
    DSCString str = dsc_str_create_empty(8);
    dsc_str_to_upper(&str);
    ASSERT_TRUE(dsc_str_empty(&str));
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_substr_create_string_zero_count_zero_pos(void) {
    DSCString str = dsc_str_create_from_cstring("abc");
    DSCString sub = dsc_str_substr_create_string(&str, 0, 0);
    ASSERT_EQ(dsc_str_size(&sub), 0);
    ASSERT_EQ_STR(dsc_str_data(&sub), "");
    dsc_str_destroy(&sub);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_substr_create_cstring_zero_count_zero_pos(void) {
    const char *src = "abc";
    DSCString sub = dsc_str_substr_create_cstring(src, 0, 0);
    ASSERT_EQ(dsc_str_size(&sub), 0);
    ASSERT_EQ_STR(dsc_str_data(&sub), "");
    dsc_str_destroy(&sub);
    return TEST_SUCCESS;
}

int test_substr_string_zero_count_zero_pos(void) {
    DSCString str = dsc_str_create_from_cstring("abc");
    char buf[8] = {0};
    dsc_str_substr_string(&str, 0, 0, buf);
    ASSERT_EQ_STR(buf, "");
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_substr_cstring_zero_count_zero_pos(void) {
    const char *src = "abc";
    char buf[8] = {0};
    dsc_str_substr_cstring(src, 0, 0, buf);
    ASSERT_EQ_STR(buf, "");
    return TEST_SUCCESS;
}

int test_compare_string_one_empty(void) {
    DSCString str1 = dsc_str_create_empty(4);
    DSCString str2 = dsc_str_create_from_cstring("abc");
    ASSERT_LT(dsc_str_compare_string(&str1, &str2), 0);
    ASSERT_GT(dsc_str_compare_string(&str2, &str1), 0);
    dsc_str_destroy(&str1);
    dsc_str_destroy(&str2);
    return TEST_SUCCESS;
}

int test_compare_cstring_one_empty(void) {
    DSCString str = dsc_str_create_empty(4);
    ASSERT_LT(dsc_str_compare_cstring(&str, "abc"), 0);
    DSCString str2 = dsc_str_create_from_cstring("abc");
    ASSERT_GT(dsc_str_compare_cstring(&str2, ""), 0);
    dsc_str_destroy(&str);
    dsc_str_destroy(&str2);
    return TEST_SUCCESS;
}

int test_getline_ch_delim_not_present(void) {
    FILE *fp = fopen("test_tmpfile3.txt", "w+");
    ASSERT_NOT_NULL(fp);
    fputs("abcdef", fp);
    fflush(fp);
    rewind(fp);
    DSCString line = dsc_str_create_empty(8);
    const int status = dsc_str_getline_ch(fp, &line, ';'); // Delimiter not present
    ASSERT_EQ_STR(dsc_str_data(&line), "abcdef");
    ASSERT_EQ(status, EOF);
    dsc_str_destroy(&line);
    fclose(fp);
    remove("test_tmpfile3.txt");
    return TEST_SUCCESS;
}

int test_getline_cstring_delim_not_present(void) {
    FILE *fp = fopen("test_tmpfile4.txt", "w+");
    ASSERT_NOT_NULL(fp);
    fputs("abcdef", fp);
    fflush(fp);
    rewind(fp);
    DSCString line = dsc_str_create_empty(8);
    const int status = dsc_str_getline_cstring(fp, &line, ";"); // Delimiter not present
    ASSERT_EQ_STR(dsc_str_data(&line), "abcdef");
    ASSERT_EQ(status, EOF);
    dsc_str_destroy(&line);
    fclose(fp);
    remove("test_tmpfile4.txt");
    return TEST_SUCCESS;
}

int test_trim_front_back_only_ws(void) {
    DSCString str = dsc_str_create_from_cstring("   \t\n  ");
    dsc_str_trim_front(&str);
    ASSERT_TRUE(dsc_str_empty(&str) || dsc_str_size(&str) == 0);
    dsc_str_assign_cstring(&str, "   \t\n  ");
    dsc_str_trim_back(&str);
    ASSERT_TRUE(dsc_str_empty(&str) || dsc_str_size(&str) == 0);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_remove_extra_ws_tabs_newlines(void) {
    DSCString str = dsc_str_create_from_cstring("\t\t\n\n\t");
    dsc_str_remove_extra_ws(&str);
    ASSERT_TRUE(dsc_str_empty(&str) || dsc_str_size(&str) == 0);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_to_lower_upper_mixed(void) {
    DSCString str = dsc_str_create_from_cstring("AbC123xYz");
    dsc_str_to_lower(&str);
    ASSERT_EQ_STR(dsc_str_data(&str), "abc123xyz");
    dsc_str_to_upper(&str);
    ASSERT_EQ_STR(dsc_str_data(&str), "ABC123XYZ");
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_find_first_of_empty_value(void) {
    DSCString str = dsc_str_create_from_cstring("abc");
    ASSERT_EQ(dsc_str_find_first_of(&str, ""), STR_NPOS);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_find_cstring_at_end(void) {
    DSCString str = dsc_str_create_from_cstring("abcdef");
    ASSERT_EQ(dsc_str_find_cstring(&str, "ef"), 4);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_find_string_at_end(void) {
    DSCString str = dsc_str_create_from_cstring("abcdef");
    DSCString find = dsc_str_create_from_cstring("ef");
    ASSERT_EQ(dsc_str_find_string(&str, &find), 4);
    dsc_str_destroy(&str);
    dsc_str_destroy(&find);
    return TEST_SUCCESS;
}

int test_substr_create_string_count_0_pos_end(void) {
    DSCString str = dsc_str_create_from_cstring("abc");
    DSCString sub = dsc_str_substr_create_string(&str, dsc_str_size(&str), 0);
    ASSERT_EQ(dsc_str_size(&sub), 0);
    ASSERT_EQ_STR(dsc_str_data(&sub), "");
    dsc_str_destroy(&sub);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_substr_create_cstring_count_0_pos_end(void) {
    const char *src = "abc";
    DSCString sub = dsc_str_substr_create_cstring(src, strlen(src), 0);
    ASSERT_EQ(dsc_str_size(&sub), 0);
    ASSERT_EQ_STR(dsc_str_data(&sub), "");
    dsc_str_destroy(&sub);
    return TEST_SUCCESS;
}

int test_null_pointer_handling(void) {
    // Should not crash, should return error or handle gracefully
    ASSERT_EQ(dsc_str_empty(nullptr), true);
    ASSERT_EQ(dsc_str_size(nullptr), 0);
    ASSERT_EQ(dsc_str_capacity(nullptr), 0);
    ASSERT_EQ_PTR(dsc_str_data(nullptr), NULL);
    // Add more NULL pointer checks for other API functions as needed
    return TEST_SUCCESS;
}

int test_invalid_values(void) {
    DSCString str = dsc_str_create_empty(8);
    dsc_str_insert_char(&str, (size_t) -1, 'X'); // Out-of-bounds
    ASSERT_EQ_STR(dsc_str_data(&str), "");
    dsc_str_erase(&str, (size_t) -1); // Out-of-bounds
    ASSERT_TRUE(dsc_str_empty(&str));
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_str_free_split_basic(void) {
    const size_t count = 3;
    DSCString *arr = (DSCString *) malloc(sizeof(DSCString) * count);
    arr[0] = dsc_str_create_from_cstring("one");
    arr[1] = dsc_str_create_from_cstring("two");
    arr[2] = dsc_str_create_from_cstring("three");
    dsc_str_destroy_split(&arr, count);
    ASSERT_NULL(arr); // arr should be NULL after free
    return TEST_SUCCESS;
}

int test_str_free_split_nullptr(void) {
    dsc_str_destroy_split(nullptr, 3); // Should not crash
    return TEST_SUCCESS;
}

int test_str_free_split_zero_count(void) {
    DSCString *arr = (DSCString *) malloc(sizeof(DSCString) * 2);
    arr[0] = dsc_str_create_from_cstring("a");
    arr[1] = dsc_str_create_from_cstring("b");
    dsc_str_destroy_split(&arr, 0); // Should only free the array pointer
    ASSERT_NULL(arr);
    return TEST_SUCCESS;
}

int test_str_split_basic(void) {
    DSCString str = dsc_str_create_from_cstring("a,b,c");
    DSCString *out = nullptr;
    const size_t count = dsc_str_split(&str, ",", &out);
    ASSERT_EQ(count, 3);
    ASSERT_EQ_STR(dsc_str_data(&out[0]), "a");
    ASSERT_EQ_STR(dsc_str_data(&out[1]), "b");
    ASSERT_EQ_STR(dsc_str_data(&out[2]), "c");
    dsc_str_destroy_split(&out, count);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_str_split_no_delim(void) {
    DSCString str = dsc_str_create_from_cstring("abc");
    DSCString *out = nullptr;
    const size_t count = dsc_str_split(&str, ";", &out);
    ASSERT_EQ(count, 1);
    ASSERT_EQ_STR(dsc_str_data(&out[0]), "abc");
    dsc_str_destroy_split(&out, count);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_str_split_empty_string(void) {
    DSCString str = dsc_str_create_empty(8);
    DSCString *out = nullptr;
    const size_t count = dsc_str_split(&str, ",", &out);
    ASSERT_EQ(count, 0);
    ASSERT_NULL(out);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

int test_str_split_nullptr(void) {
    const size_t count = dsc_str_split(nullptr, ",", nullptr);
    ASSERT_EQ(count, 0);
    return TEST_SUCCESS;
}

int test_str_split_and_free_split(void) {
    DSCString str = dsc_str_create_from_cstring("alpha,beta,gamma,delta");
    DSCString *out = nullptr;
    const size_t count = dsc_str_split(&str, ",", &out);
    ASSERT_EQ(count, 4);
    ASSERT_EQ_STR(dsc_str_data(&out[0]), "alpha");
    ASSERT_EQ_STR(dsc_str_data(&out[1]), "beta");
    ASSERT_EQ_STR(dsc_str_data(&out[2]), "gamma");
    ASSERT_EQ_STR(dsc_str_data(&out[3]), "delta");
    dsc_str_destroy_split(&out, count);
    ASSERT_NULL(out);
    dsc_str_destroy(&str);
    return TEST_SUCCESS;
}

typedef struct {
    int (*func)(void);
    const char *name;
} TestCase;

TestCase tests[] = {
    {test_find_and_compare, "test_find_and_compare"},
    {test_trim_and_case, "test_trim_and_case"},
    {test_substr, "test_substr"},
    {test_substr_out_of_bounds, "test_substr_out_of_bounds"},
    {test_embedded_null, "test_embedded_null"},
    {test_trim_all_whitespace, "test_trim_all_whitespace"},
    {test_compare_different_lengths, "test_compare_different_lengths"},
    {test_compare_different_contents, "test_compare_different_contents"},
    {test_find_first_of_no_match, "test_find_first_of_no_match"},
    {test_find_first_of_multiple_matches, "test_find_first_of_multiple_matches"},
    {test_remove_extra_ws, "test_remove_extra_ws"},
    {test_to_lower_upper_already, "test_to_lower_upper_already"},
    {test_substr_create_zero_count, "test_substr_create_zero_count"},
    {test_compare_string_equality, "test_compare_string_equality"},
    {test_substr_create_cstring_cases, "test_substr_create_cstring_cases"},
    {test_compare_cstring_prefix_suffix, "test_compare_cstring_prefix_suffix"},
    {test_getline_ch_simulated, "test_getline_ch_simulated"},
    {test_getline_cstring_simulated, "test_getline_cstring_simulated"},
    {test_find_cstring_empty_search, "test_find_cstring_empty_search"},
    {test_find_string_empty_search, "test_find_string_empty_search"},
    {test_substr_create_string_count_exceeds, "test_substr_create_string_count_exceeds"},
    {test_substr_create_string_pos_at_size, "test_substr_create_string_pos_at_size"},
    {test_substr_create_string_pos_gt_size, "test_substr_create_string_pos_gt_size"},
    {test_substr_cstring_pos_at_length, "test_substr_cstring_pos_at_length"},
    {test_substr_cstring_pos_gt_length, "test_substr_cstring_pos_gt_length"},
    {test_compare_string_empty, "test_compare_string_empty"},
    {test_compare_cstring_empty, "test_compare_cstring_empty"},
    {test_getline_ch_empty_file, "test_getline_ch_empty_file"},
    {test_getline_cstring_empty_file, "test_getline_cstring_empty_file"},
    {test_trim_front_already_trimmed, "test_trim_front_already_trimmed"},
    {test_trim_back_already_trimmed, "test_trim_back_already_trimmed"},
    {test_remove_extra_ws_only_spaces, "test_remove_extra_ws_only_spaces"},
    {test_to_lower_empty, "test_to_lower_empty"},
    {test_to_upper_empty, "test_to_upper_empty"},
    {test_substr_create_string_zero_count_zero_pos, "test_substr_create_string_zero_count_zero_pos"},
    {test_substr_create_cstring_zero_count_zero_pos, "test_substr_create_cstring_zero_count_zero_pos"},
    {test_substr_string_zero_count_zero_pos, "test_substr_string_zero_count_zero_pos"},
    {test_substr_cstring_zero_count_zero_pos, "test_substr_cstring_zero_count_zero_pos"},
    {test_compare_string_one_empty, "test_compare_string_one_empty"},
    {test_compare_cstring_one_empty, "test_compare_cstring_one_empty"},
    {test_getline_ch_delim_not_present, "test_getline_ch_delim_not_present"},
    {test_getline_cstring_delim_not_present, "test_getline_cstring_delim_not_present"},
    {test_trim_front_back_only_ws, "test_trim_front_back_only_ws"},
    {test_remove_extra_ws_tabs_newlines, "test_remove_extra_ws_tabs_newlines"},
    {test_to_lower_upper_mixed, "test_to_lower_upper_mixed"},
    {test_find_first_of_empty_value, "test_find_first_of_empty_value"},
    {test_find_cstring_at_end, "test_find_cstring_at_end"},
    {test_find_string_at_end, "test_find_string_at_end"},
    {test_substr_create_string_count_0_pos_end, "test_substr_create_string_count_0_pos_end"},
    {test_substr_create_cstring_count_0_pos_end, "test_substr_create_cstring_count_0_pos_end"},
    {test_null_pointer_handling, "test_null_pointer_handling"},
    {test_invalid_values, "test_invalid_values"},
    {test_str_free_split_basic, "test_str_free_split_basic"},
    {test_str_free_split_nullptr, "test_str_free_split_nullptr"},
    {test_str_free_split_zero_count, "test_str_free_split_zero_count"},
    {test_str_split_basic, "test_str_split_basic"},
    {test_str_split_no_delim, "test_str_split_no_delim"},
    {test_str_split_empty_string, "test_str_split_empty_string"},
    {test_str_split_nullptr, "test_str_split_nullptr"},
    {test_str_split_and_free_split, "test_str_split_and_free_split"},
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
        printf("All DString Algorithm tests passed.\n");
        return 0;
    }

    printf("%d DString Algorithm tests failed.\n", failed);
    return 1;
}