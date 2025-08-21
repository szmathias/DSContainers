#include "DString.h"
#include "TestAssert.h"

#include <stdio.h>
#include <string.h>

int test_create_and_assign() {
    String str = str_create_empty(32);
    ASSERT_EQ(str_size(&str), 0);
    str_assign_cstring(&str, "Hello");
    ASSERT_EQ_STR(str_data(&str), "Hello");
    ASSERT_EQ(str_size(&str), 5);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_append_and_insert() {
    String str = str_create_empty(16);
    str_assign_cstring(&str, "abc");
    str_append_cstring(&str, "def");
    ASSERT_EQ_STR(str_data(&str), "abcdef");
    str_insert_cstring(&str, 3, "XYZ");
    ASSERT_EQ_STR(str_data(&str), "abcXYZdef");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_push_pop_erase() {
    String str = str_create_empty(8);
    str_assign_cstring(&str, "hi");
    str_push_back(&str, '!');
    ASSERT_EQ_STR(str_data(&str), "hi!");
    str_pop_back(&str);
    ASSERT_EQ_STR(str_data(&str), "hi");
    str_erase(&str, 0);
    ASSERT_EQ_STR(str_data(&str), "i");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_find_and_compare() {
    String str = str_create_from_cstring("abcdefgabc");
    ASSERT_EQ(str_find_cstring(&str, "abc"), 0);
    ASSERT_EQ(str_find_cstring(&str, "fg"), 5);
    ASSERT_EQ(str_find_cstring(&str, "xyz"), STR_NPOS);
    ASSERT_EQ(str_compare_cstring(&str, "abcdefgabc"), 0);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_trim_and_case() {
    String str = str_create_from_cstring("   Hello World!   ");
    str_trim_front(&str);
    ASSERT_EQ_STR(str_data(&str), "Hello World!   ");
    str_trim_back(&str);
    ASSERT_EQ_STR(str_data(&str), "Hello World!");
    str_to_lower(&str);
    ASSERT_EQ_STR(str_data(&str), "hello world!");
    str_to_upper(&str);
    ASSERT_EQ_STR(str_data(&str), "HELLO WORLD!");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_substr() {
    String str = str_create_from_cstring("abcdef");
    String sub = str_substr_create_string(&str, 2, 3);
    ASSERT_EQ_STR(str_data(&sub), "cde");
    str_free(&sub);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_empty_string() {
    String str = str_create_empty(0);
    ASSERT_EQ(str_size(&str), 0);
    ASSERT_TRUE(str_empty(&str));
    ASSERT_EQ_STR(str_data(&str), "");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_assign_empty_cstring() {
    String str = str_create_empty(8);
    str_assign_cstring(&str, "");
    ASSERT_EQ(str_size(&str), 0);
    ASSERT_TRUE(str_empty(&str));
    ASSERT_EQ_STR(str_data(&str), "");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_append_empty_cstring() {
    String str = str_create_from_cstring("abc");
    str_append_cstring(&str, "");
    ASSERT_EQ_STR(str_data(&str), "abc");
    ASSERT_EQ(str_size(&str), 3);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_insert_at_bounds() {
    String str = str_create_from_cstring("abc");
    str_insert_cstring(&str, 0, "X");
    ASSERT_EQ_STR(str_data(&str), "Xabc");
    str_insert_cstring(&str, str_size(&str), "Y");
    ASSERT_EQ_STR(str_data(&str), "XabcY");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_erase_out_of_bounds() {
    String str = str_create_from_cstring("abc");
    str_erase(&str, 10); // Should do nothing
    ASSERT_EQ_STR(str_data(&str), "abc");
    str_erase(&str, -1); // Should do nothing (size_t -1 is large)
    ASSERT_EQ_STR(str_data(&str), "abc");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_substr_out_of_bounds() {
    String str = str_create_from_cstring("abc");
    String sub1 = str_substr_create_string(&str, 10, 2);
    ASSERT_EQ(str_size(&sub1), 0);
    ASSERT_EQ_STR(str_data(&sub1), "");
    String sub2 = str_substr_create_string(&str, 1, 10);
    ASSERT_EQ_STR(str_data(&sub2), "bc");
    str_free(&sub1);
    str_free(&sub2);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_reserve_and_shrink() {
    String str = str_create_empty(4);
    const size_t old_capacity = str_capacity(&str);
    ASSERT_TRUE(str_reserve(&str, 128));
    ASSERT_GT(str_capacity(&str), old_capacity);
    str_assign_cstring(&str, "abc");
    ASSERT_TRUE(str_shrink_to_fit(&str));
    ASSERT_GTE(str_capacity(&str), str_size(&str) + 1);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_buffer_growth() {
    String str = str_create_empty(4);
    size_t initial_capacity = str_capacity(&str);
    // Append enough characters to force buffer growth
    for (int i = 0; i < 100; ++i) {
        str_push_back(&str, 'x');
    }
    ASSERT_GT(str_capacity(&str), initial_capacity);
    ASSERT_EQ(str_size(&str), 100);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_embedded_null() {
    String str = str_create_empty(16);
    char data[] = {'a', 'b', '\0', 'c', 'd', '\0'};
    str_assign_cstring(&str, data);
    ASSERT_EQ(str_size(&str), 2); // Only up to first null
    ASSERT_EQ_STR(str_data(&str), "ab");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_large_string() {
    String str = str_create_empty(1024);
    for (int i = 0; i < 1000; ++i) {
        str_push_back(&str, 'x');
    }
    ASSERT_EQ(str_size(&str), 1000);
    ASSERT_GT(str_capacity(&str), 1000);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_self_assign_and_append() {
    String str = str_create_from_cstring("abc");
    str_assign_string(&str, &str);
    ASSERT_EQ_STR(str_data(&str), "abc");
    str_append_string(&str, &str);
    ASSERT_EQ_STR(str_data(&str), "abcabc");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_trim_all_whitespace() {
    String str = str_create_from_cstring("    \t\n  ");
    str_trim_front(&str);
    str_trim_back(&str);
    ASSERT_TRUE(str_empty(&str) || str_size(&str) == 0);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_reserve_and_shrink_optimal() {
    String str = str_create_from_cstring("abc");
    size_t cap = str_capacity(&str);
    ASSERT_FALSE(str_reserve(&str, cap));
    ASSERT_TRUE(str_shrink_to_fit(&str));
    str_free(&str);
    return TEST_SUCCESS;
}

int test_compare_different_lengths() {
    String str1 = str_create_from_cstring("abc");
    String str2 = str_create_from_cstring("abcd");
    ASSERT_LT(str_compare_string(&str1, &str2), 0);
    ASSERT_GT(str_compare_string(&str2, &str1), 0);
    str_free(&str1);
    str_free(&str2);
    return TEST_SUCCESS;
}

int test_compare_different_contents() {
    String str1 = str_create_from_cstring("abc");
    String str2 = str_create_from_cstring("abd");
    ASSERT_LT(str_compare_string(&str1, &str2), 0);
    ASSERT_GT(str_compare_string(&str2, &str1), 0);
    str_free(&str1);
    str_free(&str2);
    return TEST_SUCCESS;
}

int test_find_first_of_no_match() {
    String str = str_create_from_cstring("abcdef");
    ASSERT_EQ(str_find_first_of(&str, "xyz"), STR_NPOS);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_find_first_of_multiple_matches() {
    String str = str_create_from_cstring("abcdef");
    ASSERT_EQ(str_find_first_of(&str, "fa"), 0); // 'a' at pos 0
    ASSERT_EQ(str_find_first_of(&str, "f"), 5);  // 'f' at pos 5
    str_free(&str);
    return TEST_SUCCESS;
}

int test_remove_extra_ws() {
    String str = str_create_from_cstring("  a   b\t\tc  ");
    str_remove_extra_ws(&str);
    ASSERT_EQ_STR(str_data(&str), "a b c");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_clear_non_empty() {
    String str = str_create_from_cstring("not empty");
    str_clear(&str);
    ASSERT_TRUE(str_empty(&str));
    ASSERT_EQ(str_size(&str), 0);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_pop_back_empty() {
    String str = str_create_empty(8);
    str_pop_back(&str); // Should not crash
    ASSERT_TRUE(str_empty(&str));
    str_free(&str);
    return TEST_SUCCESS;
}

int test_erase_empty() {
    String str = str_create_empty(8);
    str_erase(&str, 0); // Should not crash
    ASSERT_TRUE(str_empty(&str));
    str_free(&str);
    return TEST_SUCCESS;
}

int test_to_lower_upper_already() {
    String str1 = str_create_from_cstring("abc");
    str_to_lower(&str1);
    ASSERT_EQ_STR(str_data(&str1), "abc");
    str_to_upper(&str1);
    ASSERT_EQ_STR(str_data(&str1), "ABC");
    str_free(&str1);

    String str2 = str_create_from_cstring("XYZ");
    str_to_upper(&str2);
    ASSERT_EQ_STR(str_data(&str2), "XYZ");
    str_to_lower(&str2);
    ASSERT_EQ_STR(str_data(&str2), "xyz");
    str_free(&str2);
    return TEST_SUCCESS;
}

int test_substr_create_zero_count() {
    String str = str_create_from_cstring("abcdef");
    String sub = str_substr_create_string(&str, 2, 0);
    ASSERT_EQ(str_size(&sub), 0);
    ASSERT_EQ_STR(str_data(&sub), "");
    str_free(&sub);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_compare_string_equality() {
    String str1 = str_create_from_cstring("abc");
    String str2 = str_create_from_cstring("abc");
    ASSERT_EQ(str_compare_string(&str1, &str2), 0);
    ASSERT_EQ(str_compare_string(&str2, &str1), 0);
    str_free(&str1);
    str_free(&str2);
    return TEST_SUCCESS;
}

int test_assign_char() {
    String str = str_create_empty(8);
    str_assign_char(&str, 'A');
    ASSERT_EQ_STR(str_data(&str), "A");
    ASSERT_EQ(str_size(&str), 1);
    str_assign_char(&str, '\0');
    ASSERT_EQ(str_size(&str), 1);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_insert_char_positions() {
    String str = str_create_from_cstring("ac");
    str_insert_char(&str, 1, 'b'); // Insert in middle
    ASSERT_EQ_STR(str_data(&str), "abc");
    str_insert_char(&str, 0, 'X'); // Insert at start
    ASSERT_EQ_STR(str_data(&str), "Xabc");
    str_insert_char(&str, str_size(&str), 'Y'); // Insert at end
    ASSERT_EQ_STR(str_data(&str), "XabcY");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_append_char_multiple() {
    String str = str_create_empty(4);
    str_append_char(&str, 'a');
    str_append_char(&str, 'b');
    str_append_char(&str, 'c');
    ASSERT_EQ_STR(str_data(&str), "abc");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_substr_create_cstring_cases() {
    const char *src = "abcdef";
    String sub1 = str_substr_create_cstring(src, 0, 2);
    ASSERT_EQ_STR(str_data(&sub1), "ab");
    String sub2 = str_substr_create_cstring(src, 4, 10); // count > length
    ASSERT_EQ_STR(str_data(&sub2), "ef");
    String sub3 = str_substr_create_cstring(src, 10, 2); // pos > length
    ASSERT_EQ(str_size(&sub3), 0);
    str_free(&sub1);
    str_free(&sub2);
    str_free(&sub3);
    return TEST_SUCCESS;
}

int test_compare_cstring_prefix_suffix() {
    String str = str_create_from_cstring("abc");
    ASSERT_LT(str_compare_cstring(&str, "abcd"), 0);
    ASSERT_GT(str_compare_cstring(&str, "ab"), 0);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_getline_ch_simulated() {
    // Use a temporary file on disk for portability
    FILE *fp = fopen("test_tmpfile.txt", "w+");
    ASSERT_NOT_NULL(fp);
    fputs("hello\nworld", fp);
    fflush(fp);
    rewind(fp);
    String line = str_create_empty(16);
    int status = str_getline_ch(fp, &line, '\n');
    ASSERT_EQ_STR(str_data(&line), "hello");
    ASSERT_EQ(status, 0);
    str_free(&line);
    fclose(fp);
    remove("test_tmpfile.txt");
    return TEST_SUCCESS;
}

int test_getline_cstring_simulated() {
    FILE *fp = fopen("test_tmpfile2.txt", "w+");
    ASSERT_NOT_NULL(fp);
    fputs("foo,bar,baz", fp);
    fflush(fp);
    rewind(fp);
    String line = str_create_empty(16);
    int status = str_getline_cstring(fp, &line, ",");
    ASSERT_EQ_STR(str_data(&line), "foo");
    ASSERT_EQ(status, 0);
    str_free(&line);
    fclose(fp);
    remove("test_tmpfile2.txt");
    return TEST_SUCCESS;
}

int test_clear_already_empty() {
    String str = str_create_empty(8);
    str_clear(&str);
    ASSERT_TRUE(str_empty(&str));
    str_free(&str);
    return TEST_SUCCESS;
}

int test_assign_string_different() {
    String str1 = str_create_from_cstring("foo");
    String str2 = str_create_from_cstring("bar");
    str_assign_string(&str1, &str2);
    ASSERT_EQ_STR(str_data(&str1), "bar");
    str_free(&str1);
    str_free(&str2);
    return TEST_SUCCESS;
}

int test_insert_cstring_empty() {
    String str = str_create_from_cstring("abc");
    str_insert_cstring(&str, 1, "");
    ASSERT_EQ_STR(str_data(&str), "abc");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_insert_string_empty() {
    String str = str_create_from_cstring("abc");
    String empty = str_create_empty(4);
    str_insert_string(&str, 1, &empty);
    ASSERT_EQ_STR(str_data(&str), "abc");
    str_free(&str);
    str_free(&empty);
    return TEST_SUCCESS;
}

int test_append_string_empty() {
    String str = str_create_from_cstring("abc");
    String empty = str_create_empty(4);
    str_append_string(&str, &empty);
    ASSERT_EQ_STR(str_data(&str), "abc");
    str_free(&str);
    str_free(&empty);
    return TEST_SUCCESS;
}

int test_find_cstring_empty_search() {
    String str = str_create_from_cstring("abc");
    ASSERT_EQ(str_find_cstring(&str, ""), STR_NPOS);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_find_string_empty_search() {
    String str = str_create_from_cstring("abc");
    String empty = str_create_empty(4);
    ASSERT_EQ(str_find_string(&str, &empty), STR_NPOS);
    str_free(&str);
    str_free(&empty);
    return TEST_SUCCESS;
}

int test_substr_create_string_count_exceeds() {
    String str = str_create_from_cstring("abc");
    String sub = str_substr_create_string(&str, 1, 10);
    ASSERT_EQ_STR(str_data(&sub), "bc");
    str_free(&sub);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_substr_create_string_pos_at_size() {
    String str = str_create_from_cstring("abc");
    String sub = str_substr_create_string(&str, str_size(&str), 2);
    ASSERT_EQ(str_size(&sub), 0);
    ASSERT_EQ_STR(str_data(&sub), "");
    str_free(&sub);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_substr_create_string_pos_gt_size() {
    String str = str_create_from_cstring("abc");
    String sub = str_substr_create_string(&str, str_size(&str) + 1, 2);
    ASSERT_EQ(str_size(&sub), 0);
    ASSERT_EQ_STR(str_data(&sub), "");
    str_free(&sub);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_substr_cstring_pos_at_length() {
    const char *src = "abc";
    char buf[8] = {0};
    str_substr_cstring(src, strlen(src), 2, buf);
    ASSERT_EQ_STR(buf, "");
    return TEST_SUCCESS;
}

int test_substr_cstring_pos_gt_length() {
    const char *src = "abc";
    char buf[8] = {0};
    str_substr_cstring(src, strlen(src) + 1, 2, buf);
    ASSERT_EQ_STR(buf, "");
    return TEST_SUCCESS;
}

int test_compare_string_empty() {
    String str1 = str_create_empty(4);
    String str2 = str_create_empty(4);
    ASSERT_EQ(str_compare_string(&str1, &str2), 0);
    str_free(&str1);
    str_free(&str2);
    return TEST_SUCCESS;
}

int test_compare_cstring_empty() {
    String str = str_create_empty(4);
    ASSERT_EQ(str_compare_cstring(&str, ""), 0);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_getline_ch_empty_file() {
    FILE *fp = fopen("test_tmpfile_empty.txt", "w+");
    ASSERT_NOT_NULL(fp);
    String line = str_create_empty(8);
    int status = str_getline_ch(fp, &line, '\n');
    ASSERT_EQ(status, EOF);
    ASSERT_TRUE(str_empty(&line));
    str_free(&line);
    fclose(fp);
    remove("test_tmpfile_empty.txt");
    return TEST_SUCCESS;
}

int test_getline_cstring_empty_file() {
    FILE *fp = fopen("test_tmpfile_empty2.txt", "w+");
    ASSERT_NOT_NULL(fp);
    String line = str_create_empty(8);
    int status = str_getline_cstring(fp, &line, ",");
    ASSERT_EQ(status, EOF);
    ASSERT_TRUE(str_empty(&line));
    str_free(&line);
    fclose(fp);
    remove("test_tmpfile_empty2.txt");
    return TEST_SUCCESS;
}

int test_push_back_null_char() {
    String str = str_create_empty(8);
    str_push_back(&str, '\0');
    ASSERT_EQ(str_size(&str), 1);
    ASSERT_EQ(str_data(&str)[0], '\0');
    str_free(&str);
    return TEST_SUCCESS;
}

int test_append_char_null_char() {
    String str = str_create_empty(8);
    str_append_char(&str, '\0');
    ASSERT_EQ(str_size(&str), 1);
    ASSERT_EQ(str_data(&str)[0], '\0');
    str_free(&str);
    return TEST_SUCCESS;
}

int test_insert_char_out_of_bounds() {
    String str = str_create_from_cstring("abc");
    str_insert_char(&str, 10, 'X'); // Should do nothing
    ASSERT_EQ_STR(str_data(&str), "abc");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_insert_cstring_out_of_bounds() {
    String str = str_create_from_cstring("abc");
    str_insert_cstring(&str, 10, "XYZ"); // Should do nothing
    ASSERT_EQ_STR(str_data(&str), "abc");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_insert_string_out_of_bounds() {
    String str = str_create_from_cstring("abc");
    String other = str_create_from_cstring("XYZ");
    str_insert_string(&str, 10, &other); // Should do nothing
    ASSERT_EQ_STR(str_data(&str), "abc");
    str_free(&str);
    str_free(&other);
    return TEST_SUCCESS;
}

int test_erase_at_size() {
    String str = str_create_from_cstring("abc");
    str_erase(&str, str_size(&str)); // Should do nothing
    ASSERT_EQ_STR(str_data(&str), "abc");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_trim_front_already_trimmed() {
    String str = str_create_from_cstring("abc");
    str_trim_front(&str);
    ASSERT_EQ_STR(str_data(&str), "abc");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_trim_back_already_trimmed() {
    String str = str_create_from_cstring("abc");
    str_trim_back(&str);
    ASSERT_EQ_STR(str_data(&str), "abc");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_remove_extra_ws_only_spaces() {
    String str = str_create_from_cstring("     ");
    str_remove_extra_ws(&str);
    ASSERT_TRUE(str_empty(&str) || str_size(&str) == 0);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_to_lower_empty() {
    String str = str_create_empty(8);
    str_to_lower(&str);
    ASSERT_TRUE(str_empty(&str));
    str_free(&str);
    return TEST_SUCCESS;
}

int test_to_upper_empty() {
    String str = str_create_empty(8);
    str_to_upper(&str);
    ASSERT_TRUE(str_empty(&str));
    str_free(&str);
    return TEST_SUCCESS;
}

int test_substr_create_string_zero_count_zero_pos() {
    String str = str_create_from_cstring("abc");
    String sub = str_substr_create_string(&str, 0, 0);
    ASSERT_EQ(str_size(&sub), 0);
    ASSERT_EQ_STR(str_data(&sub), "");
    str_free(&sub);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_substr_create_cstring_zero_count_zero_pos() {
    const char *src = "abc";
    String sub = str_substr_create_cstring(src, 0, 0);
    ASSERT_EQ(str_size(&sub), 0);
    ASSERT_EQ_STR(str_data(&sub), "");
    str_free(&sub);
    return TEST_SUCCESS;
}

int test_substr_string_zero_count_zero_pos() {
    String str = str_create_from_cstring("abc");
    char buf[8] = {0};
    str_substr_string(&str, 0, 0, buf);
    ASSERT_EQ_STR(buf, "");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_substr_cstring_zero_count_zero_pos() {
    const char *src = "abc";
    char buf[8] = {0};
    str_substr_cstring(src, 0, 0, buf);
    ASSERT_EQ_STR(buf, "");
    return TEST_SUCCESS;
}

int test_compare_string_one_empty() {
    String str1 = str_create_empty(4);
    String str2 = str_create_from_cstring("abc");
    ASSERT_LT(str_compare_string(&str1, &str2), 0);
    ASSERT_GT(str_compare_string(&str2, &str1), 0);
    str_free(&str1);
    str_free(&str2);
    return TEST_SUCCESS;
}

int test_compare_cstring_one_empty() {
    String str = str_create_empty(4);
    ASSERT_LT(str_compare_cstring(&str, "abc"), 0);
    String str2 = str_create_from_cstring("abc");
    ASSERT_GT(str_compare_cstring(&str2, ""), 0);
    str_free(&str);
    str_free(&str2);
    return TEST_SUCCESS;
}

int test_getline_ch_delim_not_present() {
    FILE *fp = fopen("test_tmpfile3.txt", "w+");
    ASSERT_NOT_NULL(fp);
    fputs("abcdef", fp);
    fflush(fp);
    rewind(fp);
    String line = str_create_empty(8);
    int status = str_getline_ch(fp, &line, ';'); // Delimiter not present
    ASSERT_EQ_STR(str_data(&line), "abcdef");
    ASSERT_EQ(status, EOF);
    str_free(&line);
    fclose(fp);
    remove("test_tmpfile3.txt");
    return TEST_SUCCESS;
}

int test_getline_cstring_delim_not_present() {
    FILE *fp = fopen("test_tmpfile4.txt", "w+");
    ASSERT_NOT_NULL(fp);
    fputs("abcdef", fp);
    fflush(fp);
    rewind(fp);
    String line = str_create_empty(8);
    int status = str_getline_cstring(fp, &line, ";"); // Delimiter not present
    ASSERT_EQ_STR(str_data(&line), "abcdef");
    ASSERT_EQ(status, EOF);
    str_free(&line);
    fclose(fp);
    remove("test_tmpfile4.txt");
    return TEST_SUCCESS;
}

int test_assign_string_self() {
    String str = str_create_from_cstring("self");
    str_assign_string(&str, &str);
    ASSERT_EQ_STR(str_data(&str), "self");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_append_string_self() {
    String str = str_create_from_cstring("dup");
    str_append_string(&str, &str);
    ASSERT_EQ_STR(str_data(&str), "dupdup");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_insert_char_at_0_and_size() {
    String str = str_create_from_cstring("bc");
    str_insert_char(&str, 0, 'A');
    ASSERT_EQ_STR(str_data(&str), "Abc");
    str_insert_char(&str, str_size(&str), 'Z');
    ASSERT_EQ_STR(str_data(&str), "AbcZ");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_insert_cstring_at_0_and_size() {
    String str = str_create_from_cstring("bc");
    str_insert_cstring(&str, 0, "A");
    ASSERT_EQ_STR(str_data(&str), "Abc");
    str_insert_cstring(&str, str_size(&str), "Z");
    ASSERT_EQ_STR(str_data(&str), "AbcZ");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_insert_string_at_0_and_size() {
    String str = str_create_from_cstring("bc");
    String sA = str_create_from_cstring("A");
    String sZ = str_create_from_cstring("Z");
    str_insert_string(&str, 0, &sA);
    ASSERT_EQ_STR(str_data(&str), "Abc");
    str_insert_string(&str, str_size(&str), &sZ);
    ASSERT_EQ_STR(str_data(&str), "AbcZ");
    str_free(&str);
    str_free(&sA);
    str_free(&sZ);
    return TEST_SUCCESS;
}

int test_trim_front_back_only_ws() {
    String str = str_create_from_cstring("   \t\n  ");
    str_trim_front(&str);
    ASSERT_TRUE(str_empty(&str) || str_size(&str) == 0);
    str_assign_cstring(&str, "   \t\n  ");
    str_trim_back(&str);
    ASSERT_TRUE(str_empty(&str) || str_size(&str) == 0);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_remove_extra_ws_tabs_newlines() {
    String str = str_create_from_cstring("\t\t\n\n\t");
    str_remove_extra_ws(&str);
    ASSERT_TRUE(str_empty(&str) || str_size(&str) == 0);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_to_lower_upper_mixed() {
    String str = str_create_from_cstring("AbC123xYz");
    str_to_lower(&str);
    ASSERT_EQ_STR(str_data(&str), "abc123xyz");
    str_to_upper(&str);
    ASSERT_EQ_STR(str_data(&str), "ABC123XYZ");
    str_free(&str);
    return TEST_SUCCESS;
}

int test_find_first_of_empty_value() {
    String str = str_create_from_cstring("abc");
    ASSERT_EQ(str_find_first_of(&str, ""), STR_NPOS);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_find_cstring_at_end() {
    String str = str_create_from_cstring("abcdef");
    ASSERT_EQ(str_find_cstring(&str, "ef"), 4);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_find_string_at_end() {
    String str = str_create_from_cstring("abcdef");
    String find = str_create_from_cstring("ef");
    ASSERT_EQ(str_find_string(&str, &find), 4);
    str_free(&str);
    str_free(&find);
    return TEST_SUCCESS;
}

int test_substr_create_string_count_0_pos_end() {
    String str = str_create_from_cstring("abc");
    String sub = str_substr_create_string(&str, str_size(&str), 0);
    ASSERT_EQ(str_size(&sub), 0);
    ASSERT_EQ_STR(str_data(&sub), "");
    str_free(&sub);
    str_free(&str);
    return TEST_SUCCESS;
}

int test_substr_create_cstring_count_0_pos_end() {
    const char *src = "abc";
    String sub = str_substr_create_cstring(src, strlen(src), 0);
    ASSERT_EQ(str_size(&sub), 0);
    ASSERT_EQ_STR(str_data(&sub), "");
    str_free(&sub);
    return TEST_SUCCESS;
}

int test_null_pointer_handling() {
    // Should not crash, should return error or handle gracefully
    ASSERT_EQ(str_empty(nullptr), true);
    ASSERT_EQ(str_size(nullptr), 0);
    ASSERT_EQ(str_capacity(nullptr), 0);
    ASSERT_EQ_PTR(str_data(nullptr), NULL);
    // Add more NULL pointer checks for other API functions as needed
    return TEST_SUCCESS;
}

int test_invalid_values() {
    String str = str_create_empty(8);
    str_insert_char(&str, (size_t)-1, 'X'); // Out-of-bounds
    ASSERT_EQ_STR(str_data(&str), "");
    str_erase(&str, (size_t)-1); // Out-of-bounds
    ASSERT_TRUE(str_empty(&str));
    str_free(&str);
    return TEST_SUCCESS;
}

int main(void)
{
    int failed = 0;
    // Boundary and edge case tests
    if (test_empty_string() != TEST_SUCCESS) { printf("test_empty_string failed\n"); failed++; }
    if (test_assign_empty_cstring() != TEST_SUCCESS) { printf("test_assign_empty_cstring failed\n"); failed++; }
    if (test_append_empty_cstring() != TEST_SUCCESS) { printf("test_append_empty_cstring failed\n"); failed++; }
    if (test_insert_at_bounds() != TEST_SUCCESS) { printf("test_insert_at_bounds failed\n"); failed++; }
    if (test_erase_out_of_bounds() != TEST_SUCCESS) { printf("test_erase_out_of_bounds failed\n"); failed++; }
    if (test_substr_out_of_bounds() != TEST_SUCCESS) { printf("test_substr_out_of_bounds failed\n"); failed++; }
    // Regular tests
    if (test_create_and_assign() != TEST_SUCCESS) { printf("test_create_and_assign failed\n"); failed++; }
    if (test_append_and_insert() != TEST_SUCCESS) { printf("test_append_and_insert failed\n"); failed++; }
    if (test_push_pop_erase() != TEST_SUCCESS) { printf("test_push_pop_erase failed\n"); failed++; }
    if (test_find_and_compare() != TEST_SUCCESS) { printf("test_find_and_compare failed\n"); failed++; }
    if (test_trim_and_case() != TEST_SUCCESS) { printf("test_trim_and_case failed\n"); failed++; }
    if (test_substr() != TEST_SUCCESS) { printf("test_substr failed\n"); failed++; }
    // Capacity and buffer management tests
    if (test_reserve_and_shrink() != TEST_SUCCESS) { printf("test_reserve_and_shrink failed\n"); failed++; }
    if (test_buffer_growth() != TEST_SUCCESS) { printf("test_buffer_growth failed\n"); failed++; }
    // Additional tests
    if (test_embedded_null() != TEST_SUCCESS) { printf("test_embedded_null failed\n"); failed++; }
    if (test_large_string() != TEST_SUCCESS) { printf("test_large_string failed\n"); failed++; }
    if (test_self_assign_and_append() != TEST_SUCCESS) { printf("test_self_assign_and_append failed\n"); failed++; }
    if (test_trim_all_whitespace() != TEST_SUCCESS) { printf("test_trim_all_whitespace failed\n"); failed++; }
    if (test_reserve_and_shrink_optimal() != TEST_SUCCESS) { printf("test_reserve_and_shrink_optimal failed\n"); failed++; }
    if (test_compare_different_lengths() != TEST_SUCCESS) { printf("test_compare_different_lengths failed\n"); failed++; }
    if (test_compare_different_contents() != TEST_SUCCESS) { printf("test_compare_different_contents failed\n"); failed++; }
    if (test_find_first_of_no_match() != TEST_SUCCESS) { printf("test_find_first_of_no_match failed\n"); failed++; }
    if (test_find_first_of_multiple_matches() != TEST_SUCCESS) { printf("test_find_first_of_multiple_matches failed\n"); failed++; }
    if (test_remove_extra_ws() != TEST_SUCCESS) { printf("test_remove_extra_ws failed\n"); failed++; }
    if (test_clear_non_empty() != TEST_SUCCESS) { printf("test_clear_non_empty failed\n"); failed++; }
    if (test_pop_back_empty() != TEST_SUCCESS) { printf("test_pop_back_empty failed\n"); failed++; }
    if (test_erase_empty() != TEST_SUCCESS) { printf("test_erase_empty failed\n"); failed++; }
    if (test_to_lower_upper_already() != TEST_SUCCESS) { printf("test_to_lower_upper_already failed\n"); failed++; }
    if (test_substr_create_zero_count() != TEST_SUCCESS) { printf("test_substr_create_zero_count failed\n"); failed++; }
    if (test_compare_string_equality() != TEST_SUCCESS) { printf("test_compare_string_equality failed\n"); failed++; }
    // Even more edge case tests
    if (test_clear_already_empty() != TEST_SUCCESS) { printf("test_clear_already_empty failed\n"); failed++; }
    if (test_assign_string_different() != TEST_SUCCESS) { printf("test_assign_string_different failed\n"); failed++; }
    if (test_insert_cstring_empty() != TEST_SUCCESS) { printf("test_insert_cstring_empty failed\n"); failed++; }
    if (test_insert_string_empty() != TEST_SUCCESS) { printf("test_insert_string_empty failed\n"); failed++; }
    if (test_append_string_empty() != TEST_SUCCESS) { printf("test_append_string_empty failed\n"); failed++; }
    if (test_find_cstring_empty_search() != TEST_SUCCESS) { printf("test_find_cstring_empty_search failed\n"); failed++; }
    if (test_find_string_empty_search() != TEST_SUCCESS) { printf("test_find_string_empty_search failed\n"); failed++; }
    if (test_substr_create_string_count_exceeds() != TEST_SUCCESS) { printf("test_substr_create_string_count_exceeds failed\n"); failed++; }
    if (test_substr_create_string_pos_at_size() != TEST_SUCCESS) { printf("test_substr_create_string_pos_at_size failed\n"); failed++; }
    if (test_substr_create_string_pos_gt_size() != TEST_SUCCESS) { printf("test_substr_create_string_pos_gt_size failed\n"); failed++; }
    if (test_substr_cstring_pos_at_length() != TEST_SUCCESS) { printf("test_substr_cstring_pos_at_length failed\n"); failed++; }
    if (test_substr_cstring_pos_gt_length() != TEST_SUCCESS) { printf("test_substr_cstring_pos_gt_length failed\n"); failed++; }
    if (test_compare_string_empty() != TEST_SUCCESS) { printf("test_compare_string_empty failed\n"); failed++; }
    if (test_compare_cstring_empty() != TEST_SUCCESS) { printf("test_compare_cstring_empty failed\n"); failed++; }
    if (test_getline_ch_empty_file() != TEST_SUCCESS) { printf("test_getline_ch_empty_file failed\n"); failed++; }
    if (test_getline_cstring_empty_file() != TEST_SUCCESS) { printf("test_getline_cstring_empty_file failed\n"); failed++; }
    if (test_assign_char() != TEST_SUCCESS) { printf("test_assign_char failed\n"); failed++; }
    if (test_insert_char_positions() != TEST_SUCCESS) { printf("test_insert_char_positions failed\n"); failed++; }
    if (test_append_char_multiple() != TEST_SUCCESS) { printf("test_append_char_multiple failed\n"); failed++; }
    if (test_substr_create_cstring_cases() != TEST_SUCCESS) { printf("test_substr_create_cstring_cases failed\n"); failed++; }
    if (test_compare_cstring_prefix_suffix() != TEST_SUCCESS) { printf("test_compare_cstring_prefix_suffix failed\n"); failed++; }
    if (test_getline_ch_simulated() != TEST_SUCCESS) { printf("test_getline_ch_simulated failed\n"); failed++; }
    if (test_getline_cstring_simulated() != TEST_SUCCESS) { printf("test_getline_cstring_simulated failed\n"); failed++; }
    if (test_push_back_null_char() != TEST_SUCCESS) { printf("test_push_back_null_char failed\n"); failed++; }
    if (test_append_char_null_char() != TEST_SUCCESS) { printf("test_append_char_null_char failed\n"); failed++; }
    if (test_insert_char_out_of_bounds() != TEST_SUCCESS) { printf("test_insert_char_out_of_bounds failed\n"); failed++; }
    if (test_insert_cstring_out_of_bounds() != TEST_SUCCESS) { printf("test_insert_cstring_out_of_bounds failed\n"); failed++; }
    if (test_insert_string_out_of_bounds() != TEST_SUCCESS) { printf("test_insert_string_out_of_bounds failed\n"); failed++; }
    if (test_erase_at_size() != TEST_SUCCESS) { printf("test_erase_at_size failed\n"); failed++; }
    if (test_trim_front_already_trimmed() != TEST_SUCCESS) { printf("test_trim_front_already_trimmed failed\n"); failed++; }
    if (test_trim_back_already_trimmed() != TEST_SUCCESS) { printf("test_trim_back_already_trimmed failed\n"); failed++; }
    if (test_remove_extra_ws_only_spaces() != TEST_SUCCESS) { printf("test_remove_extra_ws_only_spaces failed\n"); failed++; }
    if (test_to_lower_empty() != TEST_SUCCESS) { printf("test_to_lower_empty failed\n"); failed++; }
    if (test_to_upper_empty() != TEST_SUCCESS) { printf("test_to_upper_empty failed\n"); failed++; }
    if (test_substr_create_string_zero_count_zero_pos() != TEST_SUCCESS) { printf("test_substr_create_string_zero_count_zero_pos failed\n"); failed++; }
    if (test_substr_create_cstring_zero_count_zero_pos() != TEST_SUCCESS) { printf("test_substr_create_cstring_zero_count_zero_pos failed\n"); failed++; }
    if (test_substr_string_zero_count_zero_pos() != TEST_SUCCESS) { printf("test_substr_string_zero_count_zero_pos failed\n"); failed++; }
    if (test_substr_cstring_zero_count_zero_pos() != TEST_SUCCESS) { printf("test_substr_cstring_zero_count_zero_pos failed\n"); failed++; }
    if (test_compare_string_one_empty() != TEST_SUCCESS) { printf("test_compare_string_one_empty failed\n"); failed++; }
    if (test_compare_cstring_one_empty() != TEST_SUCCESS) { printf("test_compare_cstring_one_empty failed\n"); failed++; }
    if (test_getline_ch_delim_not_present() != TEST_SUCCESS) { printf("test_getline_ch_delim_not_present failed\n"); failed++; }
    if (test_getline_cstring_delim_not_present() != TEST_SUCCESS) { printf("test_getline_cstring_delim_not_present failed\n"); failed++; }
    if (test_assign_string_self() != TEST_SUCCESS) { printf("test_assign_string_self failed\n"); failed++; }
    if (test_append_string_self() != TEST_SUCCESS) { printf("test_append_string_self failed\n"); failed++; }
    if (test_insert_char_at_0_and_size() != TEST_SUCCESS) { printf("test_insert_char_at_0_and_size failed\n"); failed++; }
    if (test_insert_cstring_at_0_and_size() != TEST_SUCCESS) { printf("test_insert_cstring_at_0_and_size failed\n"); failed++; }
    if (test_insert_string_at_0_and_size() != TEST_SUCCESS) { printf("test_insert_string_at_0_and_size failed\n"); failed++; }
    if (test_trim_front_back_only_ws() != TEST_SUCCESS) { printf("test_trim_front_back_only_ws failed\n"); failed++; }
    if (test_remove_extra_ws_tabs_newlines() != TEST_SUCCESS) { printf("test_remove_extra_ws_tabs_newlines failed\n"); failed++; }
    if (test_to_lower_upper_mixed() != TEST_SUCCESS) { printf("test_to_lower_upper_mixed failed\n"); failed++; }
    if (test_find_first_of_empty_value() != TEST_SUCCESS) { printf("test_find_first_of_empty_value failed\n"); failed++; }
    if (test_find_cstring_at_end() != TEST_SUCCESS) { printf("test_find_cstring_at_end failed\n"); failed++; }
    if (test_find_string_at_end() != TEST_SUCCESS) { printf("test_find_string_at_end failed\n"); failed++; }
    if (test_substr_create_string_count_0_pos_end() != TEST_SUCCESS) { printf("test_substr_create_string_count_0_pos_end failed\n"); failed++; }
    if (test_substr_create_cstring_count_0_pos_end() != TEST_SUCCESS) { printf("test_substr_create_cstring_count_0_pos_end failed\n"); failed++; }

    // Add robustness tests for NULL pointer and invalid values
    if (test_null_pointer_handling() != TEST_SUCCESS) { printf("test_null_pointer_handling failed\n"); failed++; }
    if (test_invalid_values() != TEST_SUCCESS) { printf("test_invalid_values failed\n"); failed++; }

    if (failed == 0) {
        printf("All DString tests passed.\n");
        return 0;
    }

    printf("%d DString tests failed.\n", failed);
    return 1;

}
