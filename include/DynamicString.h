//
// Created by zack on 8/19/2025.
//

#ifndef DSCONTAINERS_DYNAMICSTRING_H
#define DSCONTAINERS_DYNAMICSTRING_H
#include <stdint.h>
#include <stdio.h>
#include"CStandardCompatibility.h"
#include"PlatformDefs.h"

// Minimum initial string capacity
#define STR_MIN_INIT_CAP 16

// Represents position not found
#define STR_NPOS SIZE_MAX

typedef struct DSCString
{
    size_t capacity;
    size_t size;

    union
    {
        char* data;
        char small_data[STR_MIN_INIT_CAP];
    };
} DSCString;

// Convenience macros to hopefully save some typing
// The generic implementation of these macros are towards the bottom of the
// header file so they don't clog up the rest of the API
#define dsc_str_create(value) dsc_str_create_impl(value)
#define dsc_str_assign(str, value) dsc_str_assign_impl(str, value)
#define dsc_str_insert(str, pos, value) dsc_str_insert_impl(str, pos, value)
#define dsc_str_append(str, value) dsc_str_append_impl(str, value)
#define dsc_str_find_str(str, find) dsc_str_find_str_impl(str, find)
#define dsc_str_substr_create(str, pos, count) dsc_str_substr_create_impl(str, pos, count)
#define dsc_str_substr(str, pos, count, buffer) dsc_str_substr_impl(str, pos, count, buffer)
#define dsc_str_compare(lhs, rhs) dsc_str_compare_impl(lhs, rhs)
#define dsc_str_getline(stream, line, delim) dsc_str_getline_impl(stream, line, delim)

// Different creation functions
DSC_API DSCString dsc_str_create_empty(size_t initial_capacity);

DSC_API DSCString dsc_str_create_from_cstring(const char* cstr);

DSC_API DSCString dsc_str_create_from_string(const DSCString* str);

// String free method
DSC_API void dsc_str_destroy(DSCString* str);

DSC_API void dsc_str_destroy_split(DSCString** str, size_t count);

// Clears the content of your string and assigns the value given
DSC_API void dsc_str_assign_char(DSCString* str, char value);

DSC_API void dsc_str_assign_cstring(DSCString* str, const char* cstr);

DSC_API void dsc_str_assign_string(DSCString* str, const DSCString* from);

// Appends a character to the end of your string
DSC_API void dsc_str_push_back(DSCString* str, char value);

// Appends a cstring or another String to the end of your string
DSC_API void dsc_str_append_char(DSCString* str, char value);

DSC_API void dsc_str_append_cstring(DSCString* str, const char* cstr);

DSC_API void dsc_str_append_string(DSCString* str, const DSCString* from);

// Inserts the value into any valid position within the string
DSC_API void dsc_str_insert_char(DSCString* str, size_t pos, char value);

DSC_API void dsc_str_insert_cstring(DSCString* str, size_t pos, const char* cstr);

DSC_API void dsc_str_insert_string(DSCString* str, size_t pos, const DSCString* from);

// Pop back gets rid of the last char in the string
// Erase gets rid of a character at the position given then shifts the strings
// content
DSC_API void dsc_str_pop_back(DSCString* str);

DSC_API void dsc_str_erase(DSCString* str, size_t pos);

// Empty returns whether the string is empty or not
// Clear resets the strings content and size to zero
DSC_API bool dsc_str_empty(const DSCString* str);

DSC_API void dsc_str_clear(DSCString* str);

DSC_API bool dsc_str_reserve(DSCString* str, size_t new_capacity);

DSC_API bool dsc_str_shrink_to_fit(DSCString* str);

// Helper functions for internal data
DSC_API char* dsc_str_data(DSCString* str);

DSC_API size_t dsc_str_capacity(const DSCString* str);

DSC_API size_t dsc_str_size(const DSCString* str);

// Finds the first character that matches any character in value and
// returns it's position or STR_NPOS
DSC_API size_t dsc_str_find_first_of(const DSCString* str, const char* value);

// Searches for the string given and returns the position of the beginning
// of that string or STR_NPOS //
DSC_API size_t dsc_str_find_cstring(const DSCString* str, const char* find);

DSC_API size_t dsc_str_find_string(const DSCString* str, const DSCString* find);

DSC_API void dsc_str_trim_front(DSCString* str);

DSC_API void dsc_str_trim_back(DSCString* str);

// Removes whitespace from the front and back of the string then gets rid
// of duplicate whitespace in the middle
DSC_API void dsc_str_remove_extra_ws(DSCString* str);

// Sets the entire string to lowercase or uppercase
DSC_API void dsc_str_to_lower(DSCString* str);

DSC_API void dsc_str_to_upper(DSCString* str);

// Returns a new string that is a sub string of the original or an empty
// string if pos is invalid
DSC_API DSCString dsc_str_substr_create_cstring(const char* cstr, size_t pos, size_t count);

DSC_API DSCString dsc_str_substr_create_string(const DSCString* str, size_t pos, size_t count);

// Sets buffer to the substring at pos that is count long.
// Buffer must be count + 1 long to append the null terminator at the end.
DSC_API char* dsc_str_substr_cstring(const char* cstr, size_t pos, size_t count, char* buffer);

DSC_API char* dsc_str_substr_string(const DSCString* str, size_t pos, size_t count, char* buffer);

// Splits the string at each occurrence of delim and returns an array of
// Strings. The number of strings created is returned by the function.
DSC_API size_t dsc_str_split(const DSCString* str, const char* delim, DSCString** out);

// Compares lhs to rhs. If they are equivalent then we evaluate the
// length of each string.
// Returns:
//  -1 if lhs  < rhs
//   0 if lhs == rhs
//   1 if lhs  > rhs
DSC_API int dsc_str_compare_cstring(const DSCString* lhs, const char* rhs);

DSC_API int dsc_str_compare_string(const DSCString* lhs, const DSCString* rhs);

// Reads characters from stream into line until a delimiter or EOF is read.
// Returns 0 if delim was succesfully reached otherwise returns EOF.
DSC_API int dsc_str_getline_ch(FILE* stream, DSCString* line, int delim);

DSC_API int dsc_str_getline_cstring(FILE* stream, DSCString* line, const char* delim);

DSC_API int dsc_str_getline_string(FILE* stream, DSCString* line, const DSCString* delim);

// The generic macro implementations for convenience
#define dsc_str_create_impl(value)\
    _Generic(value,\
        char *: dsc_str_create_from_cstring,\
        const char *: dsc_str_create_from_cstring,\
        String *: dsc_str_create_from_string,\
        default: dsc_str_create_empty)(value)

#define dsc_str_assign_impl(str, value)\
    _Generic(value,\
        char *: dsc_str_assign_cstring,\
        const char *: dsc_str_assign_cstring,\
        String *: dsc_str_assign_string,\
        default: dsc_str_assign_char)(str, value)

#define dsc_str_append_impl(str, value)\
    _Generic(value,\
        char *: dsc_str_append_cstring,\
        const char *: dsc_str_append_cstring,\
        String *: dsc_str_append_string,\
        default: dsc_str_append_char)(str, value)

#define dsc_str_insert_impl(str, pos, value)\
    _Generic(value,\
        char *: dsc_str_insert_cstring,\
        const char *: dsc_str_insert_cstring,\
        String *: dsc_str_insert_string,\
        default: dsc_str_insert_char)(str, pos, value)

#define dsc_str_find_str_impl(str, find)\
    _Generic(find,\
            char *: dsc_str_find_cstring,\
            const char *: dsc_str_find_cstring,\
            default: dsc_str_find_string)(str, find)

#define dsc_str_substr_create_impl(str, pos, count)\
    _Generic(str,\
        char *: dsc_str_substr_create_cstring,\
        const char *: dsc_str_substr_create_cstring,\
        default: dsc_str_substr_create_string)(str, pos, count)

#define dsc_str_substr_impl(str, pos, count, buffer)\
    _Generic(str,\
        char *: dsc_str_substr_cstring,\
        const char *: dsc_str_substr_cstring,\
        default: dsc_str_substr_string)(str, pos, count, buffer)

#define dsc_str_compare_impl(lhs, rhs)\
    _Generic(rhs,\
            char *: dsc_str_compare_cstring,\
            const char *: dsc_str_compare_cstring,\
            default: dsc_str_compare_string)(lhs, rhs)

#define dsc_str_getline_impl(stream, line, delim)\
    _Generic(delim,\
        char *: dsc_str_getline_cstring,\
        const char *: dsc_str_getline_cstring,\
        String *: dsc_str_getline_string,\
        default: dsc_str_getline_ch)(stream, line, delim)
#endif //DSCONTAINERS_DYNAMICSTRING_H