#include "TestAssert.h"
#include "Mutex.h"
#include "Threads.h"
#include <stdio.h>
#include <stdlib.h>

int test_mutex_init_destroy(void) {
    DSCMutex m;
    ASSERT_EQ(dsc_mutex_init(&m), 0);
    ASSERT_EQ(dsc_mutex_destroy(&m), 0);
    return TEST_SUCCESS;
}

int test_mutex_trylock_behavior(void) {
    DSCMutex m;
    ASSERT_EQ(dsc_mutex_init(&m), 0);

    // Lock the mutex
    ASSERT_EQ(dsc_mutex_lock(&m), 0);

    // Try-lock should fail (non-zero) for non-recursive mutexes; accept any non-zero as "would block/error"
    const int rc = dsc_mutex_trylock(&m);
    ASSERT(rc != 0);

    ASSERT_EQ(dsc_mutex_unlock(&m), 0);
    ASSERT_EQ(dsc_mutex_destroy(&m), 0);
    return TEST_SUCCESS;
}

#define NUM_THREADS 4
#define INCREMENTS 50000

typedef struct {
    int *counter;
    DSCMutex *m;
} inc_arg_t;

static void *inc_thread(void *arg) {
    const inc_arg_t *a = (inc_arg_t*)arg;
    for (int i = 0; i < INCREMENTS; ++i) {
        dsc_mutex_lock(a->m);
        ++*(a->counter);
        dsc_mutex_unlock(a->m);
    }
    return NULL;
}

int test_mutex_threaded_increment(void) {
    int counter = 0;
    DSCMutex m;
    ASSERT_EQ(dsc_mutex_init(&m), 0);

    DSCThread threads[NUM_THREADS];
    inc_arg_t args[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i) {
        args[i].counter = &counter;
        args[i].m = &m;
        const int rc = dsc_thread_create(&threads[i], inc_thread, &args[i]);
        ASSERT_EQ(rc, 0);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        ASSERT_EQ(dsc_thread_join(threads[i], NULL), 0);
    }

    ASSERT_EQ(counter, NUM_THREADS * INCREMENTS);

    ASSERT_EQ(dsc_mutex_destroy(&m), 0);
    return TEST_SUCCESS;
}

typedef struct {
    int (*func)(void);
    const char *name;
} TestCase;

int main(void) {
    TestCase tests[] = {
        {test_mutex_init_destroy, "test_mutex_init_destroy"},
        {test_mutex_trylock_behavior, "test_mutex_trylock_behavior"},
        {test_mutex_threaded_increment, "test_mutex_threaded_increment"},
    };

    int failed = 0;
    const int num_tests = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < num_tests; i++) {
        if (tests[i].func() != TEST_SUCCESS) {
            printf("%s failed\n", tests[i].name);
            failed++;
        }
    }

    if (failed == 0) {
        printf("All mutex unit tests passed.\n");
        return 0;
    }

    printf("%d mutex unit tests failed.\n", failed);
    return 1;
}
