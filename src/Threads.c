//
// Created by zack on 8/31/25.
//

#include "Threads.h"
#include <stdlib.h>

#ifdef DSC_PLATFORM_WINDOWS
#include <windows.h>

typedef struct thread_wrapper
{
    dscthread_func func;
    void *arg;
    void *result;
} thread_wrapper;

typedef struct handle_entry
{
    DSCThread thread;
    thread_wrapper *w;
    int done;       // set when thread finished
    int detached;   // set when detach called
    struct handle_entry *next;
} handle_entry;

static CRITICAL_SECTION g_map_lock;
static handle_entry *g_map_head = NULL;
static int g_map_initialized = 0;

static void ensure_map_init(void)
{
    if (!g_map_initialized)
    {
        InitializeCriticalSection(&g_map_lock);
        g_map_initialized = 1;
    }
}

static void add_mapping(DSCThread thread, thread_wrapper *w)
{
    ensure_map_init();
    handle_entry *e = (handle_entry*)malloc(sizeof(handle_entry));
    if (!e)
    {
        return;
    }

    e->thread = thread;
    e->w = w;
    e->done = 0;
    e->detached = 0;
    EnterCriticalSection(&g_map_lock);
    e->next = g_map_head;
    g_map_head = e;
    LeaveCriticalSection(&g_map_lock);
}

static handle_entry* find_and_remove_mapping(DSCThread thread)
{
    handle_entry *prev = NULL;
    handle_entry *cur = NULL;
    EnterCriticalSection(&g_map_lock);
    cur = g_map_head;
    while (cur)
    {
        if (cur->thread == thread)
        {
            if (prev)
            {
                prev->next = cur->next;
            }
            else
            {
                g_map_head = cur->next;
            }

            cur->next = NULL;
            LeaveCriticalSection(&g_map_lock);
            return cur;
        }
        prev = cur;
        cur = cur->next;
    }

    LeaveCriticalSection(&g_map_lock);
    return NULL;
}

static handle_entry* find_mapping(DSCThread thread)
{
    handle_entry *cur = NULL;
    EnterCriticalSection(&g_map_lock);
    cur = g_map_head;
    while (cur)
    {
        if (cur->thread == thread)
        {
            LeaveCriticalSection(&g_map_lock);
            return cur;
        }
        cur = cur->next;
    }

    LeaveCriticalSection(&g_map_lock);
    return NULL;
}

static DWORD WINAPI thread_func_wrapper(LPVOID param)
{
    thread_wrapper *w = (thread_wrapper*)param;
    if (!w)
    {
        return 0;
    }

    w->result = w->func(w->arg);

    // mark done and free if detached
    EnterCriticalSection(&g_map_lock);
    handle_entry *e = g_map_head;
    handle_entry *prev = NULL;
    while (e)
    {
        if (e->w == w)
        {
            break;
        }

        prev = e;
        e = e->next;
    }

    if (e)
    {
        e->done = 1;
        if (e->detached)
        {
            // remove mapping from list inline while holding the lock
            if (prev)
            {
                prev->next = e->next;
            }
            else
            {
                g_map_head = e->next;
            }

            e->next = NULL;
            // capture pointers to free after leaving lock
            thread_wrapper *to_free_w = e->w;
            handle_entry *to_free_e = e;
            LeaveCriticalSection(&g_map_lock);
            if (to_free_w)
            {
                free(to_free_w);
            }

            free(to_free_e);
            return 0;
        }
    }

    LeaveCriticalSection(&g_map_lock);
    return 0;
}

int dsc_thread_create(DSCThread* thread, dscthread_func func, void* arg)
{
    if (!thread || !func)
    {
        return -1;
    }

    thread_wrapper *w = (thread_wrapper*)malloc(sizeof(thread_wrapper));
    if (!w)
    {
        return -1;
    }

    w->func = func;
    w->arg = arg;
    w->result = NULL;

    HANDLE h = CreateThread(NULL, 0, thread_func_wrapper, w, 0, NULL);
    if (!h)
    {
        free(w);
        return -1;
    }

    *thread = (DSCThread)h;
    add_mapping(*thread, w);
    return 0;
}

int dsc_thread_join(DSCThread thread, void** retval)
{
    if (!thread)
    {
        return -1;
    }

    DWORD wait = WaitForSingleObject((HANDLE)thread, INFINITE);
    if (wait != WAIT_OBJECT_0)
    {
        CloseHandle((HANDLE)thread);
        return -1;
    }

    // find and remove mapping
    handle_entry *e = find_and_remove_mapping(thread);
    if (!e)
    {
        CloseHandle((HANDLE)thread);
        return -1;
    }

    if (retval) *retval = e->w ? e->w->result : NULL;

    if (e->w)
    {
        free(e->w);
    }

    free(e);
    CloseHandle((HANDLE)thread);
    return 0;
}

int dsc_thread_detach(DSCThread thread)
{
    if (!thread)
    {
        return -1;
    }

    EnterCriticalSection(&g_map_lock);
    handle_entry *cur = g_map_head;
    while (cur)
    {
        if (cur->thread == thread)
        {
            cur->detached = 1;
            int already_done = cur->done;
            LeaveCriticalSection(&g_map_lock);
            // if already done, remove and free now
            if (already_done)
            {
                handle_entry *e = find_and_remove_mapping(thread);
                if (e)
                {
                    if (e->w)
                    {
                        free(e->w);
                    }
                    free(e);
                }
            }
            // close the handle
            CloseHandle((HANDLE)thread);
            return 0;
        }
        cur = cur->next;
    }
    LeaveCriticalSection(&g_map_lock);
    // mapping not found
    CloseHandle((HANDLE)thread);
    return 0;
}

#else
#include <pthread.h>

int dsc_thread_create(DSCThread* thread, const dscthread_func func, void* arg)
{
    if (!thread || !func)
    {
        return -1;
    }
    return pthread_create(thread, NULL, func, arg);
}

int dsc_thread_join(const DSCThread thread, void** retval)
{
    return pthread_join(thread, retval);
}

int dsc_thread_detach(const DSCThread thread)
{
    return pthread_detach(thread);
}

#endif

