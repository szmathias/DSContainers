//
// Created by szmat on 8/19/2025.
//
#include "DynamicString.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SAFE_FREE(ptr) do { if ((ptr)) { free((ptr)); (ptr) = NULL; } } while(0)
#define GROW_CAPACITY(cap) ((cap) + ((cap) >> 1))

#define STR_DATA(str) ((str)->capacity == STR_MIN_INIT_CAP ? (str)->small_data : (str)->data)
#define ZERO_MEM(ptr, size) memset((ptr), 0, (size))

// Helper function to allocate zeroed memory
static char *mem_calloc(const size_t num_elements)
{
    return calloc(num_elements, sizeof(char));
}

// Centralized buffer management helper
static bool str_realloc(String *str, const size_t new_capacity)
{
    const char *old_data = STR_DATA(str);
    const size_t copy_size = str->size;

    if (new_capacity <= STR_MIN_INIT_CAP)
    {
        char *temp = calloc(copy_size + 1, sizeof(char));
        if (!temp && copy_size > 0) {
            return false; // Failed to allocate temporary buffer
        }
        
        memcpy(temp, old_data, copy_size);
        SAFE_FREE(str->data);

        // Move to small buffer
        ZERO_MEM(str->small_data, STR_MIN_INIT_CAP);
        memcpy(str->small_data, temp, copy_size);
        str->capacity = STR_MIN_INIT_CAP;
        free(temp);
    }
    else
    {
        char *new_data = mem_calloc(new_capacity);
        if (!new_data) {
            return false; // Failed to allocate new buffer
        }
        
        memcpy(new_data, old_data, copy_size);
        if (str->capacity > STR_MIN_INIT_CAP && str->data)
        {
            SAFE_FREE(str->data);
        }

        str->data = new_data;
        str->capacity = new_capacity;
    }
    return true;
}

// Helper function to ensure enough capacity for data
static bool str_ensure_capacity(String *str, const size_t required_capacity)
{
    if (required_capacity <= str->capacity)
    {
        return true; // Already have enough capacity
    }

    size_t new_capacity = GROW_CAPACITY(str->capacity);
    if (new_capacity < required_capacity)
    {
        new_capacity = required_capacity;
    }

    return str_realloc(str, new_capacity);
}

String str_create_empty(const size_t initial_capacity)
{
    String result;

    const size_t capacity = (initial_capacity >= STR_MIN_INIT_CAP) ? initial_capacity : STR_MIN_INIT_CAP;

    result.capacity = capacity;
    result.size     = 0;

    if (capacity > STR_MIN_INIT_CAP)
    {
        result.data = mem_calloc(capacity);
        if (!result.data) {
            // Fallback to small buffer on allocation failure
            result.capacity = STR_MIN_INIT_CAP;
            ZERO_MEM(result.small_data, STR_MIN_INIT_CAP);
        }
    }
    else
    {
        // Use the small_data buffer for small initial capacities
        ZERO_MEM(result.small_data, STR_MIN_INIT_CAP);
        result.data = NULL;
    }

    return result;
}

String str_create_from_cstring(const char *cstr)
{
    if (!cstr)
    {
        return str_create_empty(0);
    }

    const size_t length = strlen(cstr);
    String result       = str_create_empty(length + 1); // +1 for the null-terminator
    char *data_to_use   = STR_DATA(&result);
    memcpy(data_to_use, cstr, length);
    result.size = length;

    return result;
}

String str_create_from_string(const String *str)
{
    if (!str)
    {
        return str_create_empty(0);
    }

    const char *data_to_use = STR_DATA(str);
    return str_create_from_cstring(data_to_use);
}

void str_destroy(String *str)
{
    if (!str)
    {
        return;
    }

    if (str->capacity != STR_MIN_INIT_CAP)
    {
        free(str->data);
    }
    str->size     = 0;
    str->capacity = 0;
    str->data     = NULL;
}

void str_destroy_split(String **str, const size_t count)
{
    if (!str)
    {
        return;
    }

    if (count == 0)
    {
        free(*str);
        *str = NULL;
        return;
    }

    for (size_t i = 0; i < count; i++)
    {
        String temp = (*str)[i];
        str_destroy(&temp);
    }
    free(*str);
    *str = NULL;
}

void str_assign_char(String *str, const char value)
{
    if (!str)
    {
        return;
    }

    str_clear(str);
    char *data_to_use = STR_DATA(str);
    data_to_use[0]    = value;
    str->size         = 1;
}

void str_assign_cstring(String *str, const char *cstr)
{
    if (!str || !cstr)
    {
        return;
    }

    str_clear(str);
    const size_t length = strlen(cstr);
    str_ensure_capacity(str, length + 1); // +1 for the null-terminator
    char *data_to_use = STR_DATA(str);
    memcpy(data_to_use, cstr, length + 1);
    str->size = length;
}

void str_assign_string(String *str, const String *from)
{
    if (!str || !from)
    {
        return;
    }

    if (str == from)
    {
        return; // No need to assign if it's the same string
    }

    const char *data_to_use = STR_DATA(from);
    str_assign_cstring(str, data_to_use);
}

void str_push_back(String *str, const char value)
{
    if (!str)
    {
        return;
    }

    str_ensure_capacity(str, str->size + 1); // Ensure enough capacity for the new character

    if (str->capacity == STR_MIN_INIT_CAP)
    {
        str->small_data[str->size] = value;
    }
    else
    {
        str->data[str->size] = value;
    }

    ++str->size;
}

void str_append_char(String *str, const char value)
{
    if (!str)
    {
        return;
    }

    str_push_back(str, value);
}

void str_append_cstring(String *str, const char *cstr)
{
    if (!str || !cstr)
    {
        return;
    }

    const size_t length = strlen(cstr);
    str_ensure_capacity(str, str->size + length + 1); // Ensure enough capacity for the entire C-string

    char *data_to_use = STR_DATA(str);
    memcpy(data_to_use + str->size, cstr, length);

    str->size += length;
}

void str_append_string(String *str, const String *from)
{
    if (!str || !from)
    {
        return;
    }

    const char *data_to_use = STR_DATA(from);
    str_append_cstring(str, data_to_use);
}

void str_insert_char(String *str, const size_t pos, const char value)
{
    if (!str)
    {
        return;
    }

    if (pos > str->size)
    {
        return;
    }

    if (str->size + 1 > str->capacity - 1)
    {
        str_realloc(str, GROW_CAPACITY(str->capacity));
    }

    char *data_to_use = STR_DATA(str);
    if (pos == str->size)
    {
        data_to_use[str->size] = value;
    }
    else
    {
        for (size_t i = str->size; i > pos; i--)
        {
            data_to_use[i] = data_to_use[i - 1];
        }
        data_to_use[pos] = value;
    }

    str->size++;
}

void str_insert_cstring(String *str, const size_t pos, const char *cstr)
{
    if (!str || !cstr)
    {
        return;
    }

    if (pos > str->size)
    {
        return;
    }

    if (pos == str->size)
    {
        str_append_cstring(str, cstr);
    }
    else
    {
        for (size_t i = 0; i < strlen(cstr); i++)
        {
            str_insert_char(str, pos + i, cstr[i]);
        }
    }
}

void str_insert_string(String *str, const size_t pos, const String *from)
{
    if (!str || !from)
    {
        return;
    }

    const char *data_to_use = STR_DATA(from);
    str_insert_cstring(str, pos, data_to_use);
}

void str_pop_back(String *str)
{
    if (!str)
    {
        return;
    }

    if (str->size > 0)
    {
        char *data_to_use = STR_DATA(str);
        str->size--;
        data_to_use[str->size] = '\0';
    }
}

void str_erase(String *str, const size_t pos)
{
    if (!str)
    {
        return;
    }

    if (str->size == 0 || pos >= str->size)
    {
        return;
    }

    if (pos == str->size - 1)
    {
        str_pop_back(str);
    }
    else
    {
        char *data_to_use = STR_DATA(str);

        for (size_t i = pos; i < str->size - 1; i++)
        {
            data_to_use[i] = data_to_use[i + 1];
        }
        str->size--;
        data_to_use[str->size] = '\0';
    }
}

bool str_empty(const String *str)
{
    if (!str)
    {
        return true;
    }

    return (str->size == 0);
}

void str_clear(String *str)
{
    if (!str)
    {
        return;
    }

    char *data_to_use = STR_DATA(str);

    ZERO_MEM(data_to_use, str->size);
    str->size = 0;
}

bool str_reserve(String *str, const size_t new_capacity)
{
    if (!str)
    {
        return false;
    }

    if (new_capacity <= str->capacity)
    {
        return false;
    }
    
    return str_realloc(str, new_capacity);
}

bool str_shrink_to_fit(String *str)
{
    if (!str)
    {
        return false;
    }

    size_t new_capacity = str->size + 1;
    if (new_capacity < STR_MIN_INIT_CAP)
    {
        new_capacity = STR_MIN_INIT_CAP;
    }

    if (new_capacity == str->capacity)
    {
        return true;
    }

    str_realloc(str, new_capacity);
    return true;
}

char *str_data(String *str)
{
    if (!str)
    {
        return NULL;
    }

    return STR_DATA(str);
}

size_t str_capacity(const String *str)
{
    if (!str)
    {
        return 0;
    }

    return str->capacity;
}

size_t str_size(const String *str)
{
    if (!str)
    {
        return 0;
    }

    return str->size;
}

size_t str_find_first_of(const String *str, const char *value)
{
    if (!str || !value)
    {
        return STR_NPOS;
    }

    const char *data_to_use = STR_DATA(str);

    const char *result = strpbrk(data_to_use, value);
    if (result != NULL)
    {
        return (size_t) (result - data_to_use);
    }
    return STR_NPOS;
}

size_t str_find_cstring(const String *str, const char *find)
{
    if (!str || !find)
    {
        return STR_NPOS;
    }

    const size_t length = strlen(find);
    if (length == 0)
    {
        return STR_NPOS;
    }

    const char *data_to_use = STR_DATA(str);

    size_t pos = STR_NPOS;
    for (size_t i = 0; i < str->size; i++)
    {
        if (data_to_use[i] == find[0])
        {
            if (strncmp(data_to_use + i, find, length) == 0)
            {
                pos = i;
                break;
            }
        }
    }
    return pos;
}

size_t str_find_string(const String *str, const String *find)
{
    if (!str || !find)
    {
        return STR_NPOS;
    }

    const char *data_to_use = STR_DATA(find);
    return str_find_cstring(str, data_to_use);
}

void str_trim_front(String *str)
{
    if (!str)
    {
        return;
    }

    char *data = STR_DATA(str);
    size_t i   = 0;

    while (i < str->size && isspace((unsigned char) data[i]))
    {
        i++;
    }

    if (i == 0)
    {
        return;
    }

    const size_t count = str->size - i;
    if (count == 0)
    {
        str_clear(str);
    }
    else
    {
        memmove(data, data + i, count);
        for (size_t j = 0; j < i; ++j)
        {
            str_pop_back(str);
        }
    }
}

void str_trim_back(String *str)
{
    if (!str)
    {
        return;
    }

    if (str->size == 0)
    {
        return;
    }

    const char *data = STR_DATA(str);
    while (isspace(data[str->size - 1]))
    {
        str_pop_back(str);
    }
}

void str_remove_extra_ws(String *str)
{
    if (!str)
    {
        return;
    }

    char *data_to_use = STR_DATA(str);

    str_trim_back(str);
    str_trim_front(str);
    for (size_t i = 0; i < str->size; ++i)
    {
        if (isspace((unsigned char) data_to_use[i]) != 0 && isspace((unsigned char) data_to_use[i + 1]) != 0)
        {
            if (data_to_use[i] != ' ')
            {
                data_to_use[i] = ' ';
            }
            str_erase(str, i);
            --i;
        }
        else if (isspace((unsigned char) data_to_use[i]) != 0 && data_to_use[i] != ' ')
        {
            data_to_use[i] = ' ';
        }
    }
}

void str_to_lower(String *str)
{
    if (!str)
    {
        return;
    }

    char *data_to_use = STR_DATA(str);
    for (size_t i = 0; i < str->size; i++)
    {
        data_to_use[i] = (char) tolower((unsigned char) data_to_use[i]);
    }
}

void str_to_upper(String *str)
{
    if (!str)
    {
        return;
    }

    char *data_to_use = STR_DATA(str);
    for (size_t i = 0; i < str->size; i++)
    {
        data_to_use[i] = (char) toupper((unsigned char) data_to_use[i]);
    }
}

String str_substr_create_cstring(const char *cstr, const size_t pos, size_t count)
{
    if (!cstr)
    {
        return str_create_empty(0);
    }

    const size_t size = strlen(cstr);

    if (count + pos >= size)
    {
        count = size - pos;
    }

    if (size - count > size)
    {
        return str_create_empty(0);
    }

    if (pos >= size)
    {
        return str_create_empty(0);
    }

    String result = str_create_empty(count);
    for (size_t i = 0; i < count; i++)
    {
        str_push_back(&result, cstr[i + pos]);
    }

    return result;
}

String str_substr_create_string(const String *str, const size_t pos, const size_t count)
{
    if (!str)
    {
        return str_create_empty(0);
    }

    const char *data_to_use = STR_DATA(str);
    return str_substr_create_cstring(data_to_use, pos, count);
}

char *str_substr_cstring(const char *cstr, const size_t pos, size_t count, char *buffer)
{
    if (!cstr || !buffer)
    {
        return buffer;
    }

    const size_t size = strlen(cstr);
    if (count + pos >= size)
    {
        count = size - pos;
    }

    if (size - count > size)
    {
        return buffer;
    }

    if (pos >= size)
    {
        return buffer;
    }

    for (size_t i = 0; i < count; i++)
    {
        buffer[i] = cstr[i + pos];
    }
    buffer[count] = '\0';
    return buffer;
}

char *str_substr_string(const String *str, const size_t pos, const size_t count, char *buffer)
{
    if (!str || !buffer)
    {
        return buffer;
    }

    const char *data_to_use = STR_DATA(str);
    return str_substr_cstring(data_to_use, pos, count, buffer);
}

size_t str_split(const String *str, const char *delim, String **out)
{
    if (!str || !delim)
    {
        return 0;
    }

    if (str->size == 0)
    {
        return 0;
    }

    size_t num_strings = 0;
    char *buffer       = mem_calloc(str->size + 1);
    memcpy(buffer, STR_DATA(str), str->size);

    String *temp = malloc(sizeof(String));
    if (!temp)
    {
        free(buffer);
        return 0;
    }

    const char *token = strtok(buffer, delim);
    while (token != NULL)
    {
        String *new_temp = realloc(temp, sizeof(String) * (num_strings + 1));
        if (!new_temp)
        {
            fprintf(stderr, "Error: Unable to allocate memory\n");
            free(buffer);
            free(temp);
            return 0;
        }
        temp              = new_temp;
        temp[num_strings] = str_create_from_cstring(token);
        num_strings++;
        token = strtok(NULL, delim);
    }
    *out = temp;
    free(buffer);

    return num_strings;
}

int str_compare_cstring(const String *lhs, const char *rhs)
{
    if (!lhs || !rhs)
    {
        return -1;
    }

    const char *data_to_use = STR_DATA(lhs);

    const size_t rhs_size = strlen(rhs);
    const size_t min      = (lhs->size < strlen(rhs)) ? lhs->size : rhs_size;

    int result = strncmp(data_to_use, rhs, min);
    if (result != 0)
    {
        return result;
    }
    if (lhs->size < rhs_size)
    {
        result = -1;
    }
    else if (lhs->size > rhs_size)
    {
        result = 1;
    }
    return result;
}

int str_compare_string(const String *lhs, const String *rhs)
{
    if (!lhs || !rhs)
    {
        return -1;
    }

    const char *rhs_data_to_use = STR_DATA(rhs);
    return str_compare_cstring(lhs, rhs_data_to_use);
}

int str_getline_ch(FILE *stream, String *line, int delim)
{
    if (line == NULL || stream == NULL)
    {
        return EOF; // Invalid parameters
    }

    if (delim < 0 || delim > 255)
    {
        return EOF; // Invalid delimiter
    }

    str_clear(line);
    int ch     = 0;
    int status = 0;

    if (delim == '\0')
    {
        delim = '\n';
    }

    while ((ch = fgetc(stream)) != EOF && ch != delim)
    {
        str_push_back(line, (char) ch);
    }

    if (feof(stream))
    {
        status = EOF;
    }
    return status;
}

int str_getline_cstring(FILE *stream, String *line, const char *delim)
{
    if (line == NULL || stream == NULL)
    {
        return EOF; // Invalid parameters
    }

    if (delim == NULL || strlen(delim) == 0)
    {
        delim = "\n"; // Default delimiter is newline
    }

    str_clear(line);

    int ch     = 0;
    int status = 0;

    while (((ch = fgetc(stream))) && ch != EOF)
    {
        if (strchr(delim, ch) != NULL)
        {
            break;
        }
        str_push_back(line, (char) ch);
    }

    if (ch == EOF)
    {
        status = EOF;
    }
    return status;
}

int str_getline_string(FILE *stream, String *line, const String *delim)
{
    if (!line || !stream || !delim)
    {
        return EOF;
    }

    const char *delim_data = STR_DATA(delim);
    return str_getline_cstring(stream, line, delim_data);
}
