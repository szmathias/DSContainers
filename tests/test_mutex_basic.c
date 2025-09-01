#include "Threads.h"
#include "Mutex.h"
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 8
#define INCREMENTS 100000

typedef struct {
    int *counter;
    DSCMutex *m;
} thread_arg_t;

static void *inc_thread(void *arg) {
    const thread_arg_t *t = (thread_arg_t*)arg;
    for (int i = 0; i < INCREMENTS; ++i) {
        dsc_mutex_lock(t->m);
        ++*(t->counter);
        dsc_mutex_unlock(t->m);
    }
    return NULL;
}

int main(void) {
    int counter = 0;
    DSCMutex m;
    if (dsc_mutex_init(&m) != 0) {
        fprintf(stderr, "dsc_mutex_init failed\n");
        return 1;
    }

    DSCThread threads[NUM_THREADS];
    thread_arg_t args[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i) {
        args[i].counter = &counter;
        args[i].m = &m;
        if (dsc_thread_create(&threads[i], inc_thread, &args[i]) != 0) {
            fprintf(stderr, "thread_create failed\n");
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        dsc_thread_join(threads[i], NULL);
    }

    if (counter != NUM_THREADS * INCREMENTS) {
        fprintf(stderr, "Counter mismatch: expected %d, got %d\n", NUM_THREADS * INCREMENTS, counter);
        return 1;
    }

    dsc_mutex_destroy(&m);
    printf("test_dsc_mutex_basic passed\n");
    return 0;
}

