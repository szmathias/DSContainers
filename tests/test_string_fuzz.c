//
// Created by zack on 9/3/25.
//

#include "DynamicString.h"
#include "TestAssert.h"
#include <stdlib.h>
#include <time.h>

#define NUM_FUZZ_OPERATIONS 100000

static void perform_random_operation(String* str) {
    const int op = rand() % 8;

    switch (op) {
        case 0: // push_back
            str_push_back(str, (char)(rand() % 95 + 32)); // Printable ASCII
            break;
        case 1: // pop_back
            if (!str_empty(str)) {
                str_pop_back(str);
            }
            break;
        case 2: // insert
            if (str_size(str) > 0) {
                str_insert_char(str, rand() % str_size(str), 'X');
            }
            break;
        case 3: // erase
            if (str_size(str) > 0) {
                str_erase(str, rand() % str_size(str));
            }
            break;
        case 4: // assign
            str_assign_cstring(str, "fuzz");
            break;
        case 5: // clear
            str_clear(str);
            break;
        case 6: // trim
            str_trim_front(str);
            str_trim_back(str);
            break;
        case 7: // reserve
            str_reserve(str, rand() % 256);
            break;

        default:
            break;
    }
}

int test_string_fuzz(void) {
    srand(time(NULL));
    String str = str_create_empty(0);

    for (int i = 0; i < NUM_FUZZ_OPERATIONS; ++i) {
        perform_random_operation(&str);
    }

    str_destroy(&str);
    printf("DString fuzz test completed %d operations without crashing.\n", NUM_FUZZ_OPERATIONS);
    return TEST_SUCCESS;
}

int main(void) {
    return test_string_fuzz() == TEST_SUCCESS ? 0 : 1;
}