#ifndef _WIN32
#define _POSIX_C_SOURCE 199309L
#endif

#include "TestAssert.h"
#include "Threads.h"
#include "Mutex.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
static void portable_sleep_ms(unsigned int ms) { Sleep(ms); }
#else
#include <unistd.h>
#include <time.h>

static void portable_sleep_ms(unsigned int ms)
{
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}
#endif

int test_mutex_null_args(void)
{
    // All mutex APIs should return non-zero for NULL argument per implementation
    ASSERT(dsc_mutex_init(NULL) != 0);
    ASSERT(dsc_mutex_lock(NULL) != 0);
    ASSERT(dsc_mutex_trylock(NULL) != 0);
    ASSERT(dsc_mutex_unlock(NULL) != 0);
    ASSERT(dsc_mutex_destroy(NULL) != 0);
    return TEST_SUCCESS;
}

// Thread that locks the mutex for a short period
typedef struct
{
    DSCMutex* m;
    unsigned int hold_ms;
} hold_arg_t;

static void* hold_thread(void* arg)
{
    hold_arg_t* ha = arg;
    if (!ha || !ha->m)
        return NULL;
    dsc_mutex_lock(ha->m);
    portable_sleep_ms(ha->hold_ms);
    dsc_mutex_unlock(ha->m);
    return NULL;
}

int test_mutex_trylock_contention(void)
{
    DSCMutex m;
    ASSERT_EQ(dsc_mutex_init(&m), 0);

    DSCThread t;
    hold_arg_t ha = {.m = &m, .hold_ms = 200};
    ASSERT_EQ(dsc_thread_create(&t, hold_thread, &ha), 0);

    // give the thread time to acquire the lock
    portable_sleep_ms(20);

    // trylock should fail (non-zero) while other thread holds it
    const int rc = dsc_mutex_trylock(&m);
    ASSERT(rc != 0);

    ASSERT_EQ(dsc_thread_join(t, NULL), 0);
    ASSERT_EQ(dsc_mutex_destroy(&m), 0);
    return TEST_SUCCESS;
}

int test_thread_create_null(void)
{
    DSCThread t;
    // NULL thread pointer
    ASSERT(dsc_thread_create(NULL, hold_thread, NULL) != 0);
    // NULL function pointer
    ASSERT(dsc_thread_create(&t, NULL, NULL) != 0);
    return TEST_SUCCESS;
}

static void* retval_thread(void* arg)
{
    (void)arg;
    int* p = malloc(sizeof(int));
    if (!p)
        return NULL;
    *p = 12345;
    return p;
}

int test_thread_join_retrieves_result(void)
{
    DSCThread t;
    ASSERT_EQ(dsc_thread_create(&t, retval_thread, NULL), 0);
    void* ret = NULL;
    ASSERT_EQ(dsc_thread_join(t, &ret), 0);
    ASSERT(ret != NULL);
    ASSERT_EQ(*(int*)ret, 12345);
    free(ret);
    return TEST_SUCCESS;
}

static void* short_sleep_thread(void* arg)
{
    (void)arg;
    portable_sleep_ms(50);
    return NULL;
}

int test_thread_detach_then_join_fails(void)
{
    DSCThread t;
    ASSERT_EQ(dsc_thread_create(&t, short_sleep_thread, NULL), 0);
    // detach should succeed
    ASSERT_EQ(dsc_thread_detach(t), 0);

    #ifdef DSC_SANITIZERS_ENABLED
    // ThreadSanitizer/ASan interceptors will abort the process if pthread_join is
    // called on a detached thread. In sanitizer-enabled builds we skip calling
    // join and treat detach success as the test passing condition.
    return TEST_SUCCESS;
    #else
    // join after detach should fail (non-zero)
    const int rc = dsc_thread_join(t, NULL);
    ASSERT(rc != 0);
    // platform implementations may close handles on detach; nothing further to cleanup
    return TEST_SUCCESS;
    #endif
}

typedef struct
{
    int (*func)(void);
    const char* name;
} TestCase;

int main(void)
{
    TestCase tests[] = {
        {test_mutex_null_args, "test_mutex_null_args"},
        {test_mutex_trylock_contention, "test_mutex_trylock_contention"},
        {test_thread_create_null, "test_thread_create_null"},
        {test_thread_join_retrieves_result, "test_thread_join_retrieves_result"},
        {test_thread_detach_then_join_fails, "test_thread_detach_then_join_fails"},
    };

    int failed = 0;
    const int num_tests = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < num_tests; ++i)
    {
        if (tests[i].func() != TEST_SUCCESS)
        {
            printf("%s failed\n", tests[i].name);
            failed++;
        }
    }

    if (failed == 0)
    {
        printf("All extra thread/mutex tests passed.\n");
        return 0;
    }

    printf("%d extra tests failed.\n", failed);
    return 1;
}