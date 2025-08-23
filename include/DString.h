//
// Created by szmat on 8/19/2025.
//

#ifndef DS_STRING_H
#define DS_STRING_H
#include <stdint.h>
#include <stdio.h>

// Minimum initial string capacity
#define STR_MIN_INIT_CAP 16

// Represents position not found
#define STR_NPOS SIZE_MAX

typedef struct String
{
    size_t capacity;
    size_t size;

    union {
        char *data;
        char small_data[STR_MIN_INIT_CAP];
    };
} String;

// Convenience macros to hopefully save some typing
// The generic implementation of these macros are towards the bottom of the
// header file so they don't clog up the rest of the API
#define str_create(value) str_create_impl(value)
#define str_assign(str, value) str_assign_impl(str, value)
#define str_insert(str, pos, value) str_insert_impl(str, pos, value)
#define str_append(str, value) str_append_impl(str, value)
#define str_find_str(str, find) str_find_str_impl(str, find)
#define str_substr_create(str, pos, count) str_substr_create_impl(str, pos, count)
#define str_substr(str, pos, count, buffer) str_substr_impl(str, pos, count, buffer)
#define str_compare(lhs, rhs) str_compare_impl(lhs, rhs)
#define str_getline(stream, line, delim) str_getline_impl(stream, line, delim)

// Different creation functions
String str_create_empty(size_t initial_capacity);
String str_create_from_cstring(const char *cstr);
String str_create_from_string(const String *str);

// String free method
void str_free(String *str);
void str_free_split(String **str, size_t count);

// Clears the content of your string and assigns the value given
void str_assign_char(String *str, char value);
void str_assign_cstring(String *str, const char *cstr);
void str_assign_string(String *str, const String *from);

// Appends a character to the end of your string
void str_push_back(String *str, char value);

// Appends a cstring or another String to the end of your string
void str_append_char(String *str, char value);
void str_append_cstring(String *str, const char *cstr);
void str_append_string(String *str, const String *from);

// Inserts the value into any valid position within the string
void str_insert_char(String *str, size_t pos, char value);
void str_insert_cstring(String *str, size_t pos, const char *cstr);
void str_insert_string(String *str, size_t pos, const String *from);

// Pop back gets rid of the last char in the string
// Erase gets rid of a character at the position given then shifts the strings
// content
void str_pop_back(String *str);
void str_erase(String *str, size_t pos);

// Empty returns whether the string is empty or not
// Clear resets the strings content and size to zero
bool str_empty(const String *str);
void str_clear(String *str);

bool str_reserve(String *str, size_t new_capacity);
bool str_shrink_to_fit(String *str);

// Helper functions for internal data
char *str_data(String *str);
size_t str_capacity(const String *str);
size_t str_size(const String *str);

// Finds the first character that matches any character in value and
// returns it's position or STR_NPOS
size_t str_find_first_of(const String *str, const char *value);

// Searches for the string given and returns the position of the beginning
// of that string or STR_NPOS //
size_t str_find_cstring(const String *str, const char *find);
size_t str_find_string(const String *str, const String *find);

void str_trim_front(String *str);
void str_trim_back(String *str);

// Removes whitespace from the front and back of the string then gets rid
// of duplicate whitespace in the middle
void str_remove_extra_ws(String *str);

// Sets the entire string to lowercase or uppercase
void str_to_lower(String *str);
void str_to_upper(String *str);

// Returns a new string that is a sub string of the original or an empty
// string if pos is invalid
String str_substr_create_cstring(const char *cstr, size_t pos, size_t count);
String str_substr_create_string(const String *str, size_t pos, size_t count);

// Sets buffer to the substring at pos that is count long.
// Buffer must be count + 1 long to append the null terminator at the end.
char *str_substr_cstring(const char *cstr, size_t pos, size_t count, char *buffer);
char *str_substr_string(const String *str, size_t pos, size_t count, char *buffer);

// Splits the string at each occurrence of delim and returns an array of
// Strings. The number of strings created is returned by the function.
size_t str_split(const String *str, const char* delim, String **out);

// Compares lhs to rhs. If they are equivalent then we evaluate the
// length of each string.
// Returns:
//  -1 if lhs  < rhs
//   0 if lhs == rhs
//   1 if lhs  > rhs
int str_compare_cstring(const String *lhs, const char *rhs);
int str_compare_string(const String *lhs, const String *rhs);

// Reads characters from stream into line until a delimiter or EOF is read.
// Returns 0 if delim was succesfully reached otherwise returns EOF.
int str_getline_ch(FILE *stream, String *line, int delim);
int str_getline_cstring(FILE *stream, String *line, const char *delim);
int str_getline_string(FILE *stream, String *line, const String *delim);

// The generic macro implementations for convenience
#define str_create_impl(value)\
    _Generic(value,\
        char *: str_create_from_cstring,\
        const char *: str_create_from_cstring,\
        String *: str_create_from_string,\
        default: str_create_empty)(value)

#define str_assign_impl(str, value)\
    _Generic(value,\
        char *: str_assign_cstring,\
        const char *: str_assign_cstring,\
        String *: str_assign_string,\
        default: str_assign_char)(str, value)

#define str_append_impl(str, value)\
    _Generic(value,\
        char *: str_append_cstring,\
        const char *: str_append_cstring,\
        String *: str_append_string,\
        default: str_append_char)(str, value)

#define str_insert_impl(str, pos, value)\
    _Generic(value,\
        char *: str_insert_cstring,\
        const char *: str_insert_cstring,\
        String *: str_insert_string,\
        default: str_insert_char)(str, pos, value)

#define str_find_str_impl(str, find)\
    _Generic(find,\
            char *: str_find_cstring,\
            const char *: str_find_cstring,\
            default: str_find_string)(str, find)

#define str_substr_create_impl(str, pos, count)\
    _Generic(str,\
        char *: str_substr_create_cstring,\
        const char *: str_substr_create_cstring,\
        default: str_substr_create_string)(str, pos, count)

#define str_substr_impl(str, pos, count, buffer)\
    _Generic(str,\
        char *: str_substr_cstring,\
        const char *: str_substr_cstring,\
        default: str_substr_string)(str, pos, count, buffer)

#define str_compare_impl(lhs, rhs)\
    _Generic(rhs,\
            char *: str_compare_cstring,\
            const char *: str_compare_cstring,\
            default: str_compare_string)(lhs, rhs)

#define str_getline_impl(stream, line, delim)\
    _Generic(delim,\
        char *: str_getline_cstr,\
        const char *: str_getline_cstr,\
        String *: str_getline_str,\
        default: str_getline_ch)(stream, line, delim)
#endif //DS_STRING_H