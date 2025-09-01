// Mutex.c
// Cross-platform mutex implementation

#include "Mutex.h"
#include <stdlib.h>

#ifdef DSC_PLATFORM_WINDOWS
#include <windows.h>

int dsc_mutex_init(DSCMutex* m)
{
    if (!m)
    {
        return -1;
    }
    InitializeCriticalSection(m);
    return 0;
}

int dsc_mutex_lock(DSCMutex* m)
{
    if (!m)
    {
        return -1;
    }
    EnterCriticalSection(m);
    return 0;
}

int dsc_mutex_trylock(DSCMutex* m)
{
    if (!m)
    {
        return -1;
    }
    return TryEnterCriticalSection(m) ? 0 : 1;
}

int dsc_mutex_unlock(DSCMutex* m)
{
    if (!m)
    {
        return -1;
    }
    LeaveCriticalSection(m);
    return 0;
}

int dsc_mutex_destroy(DSCMutex* m)
{
    if (!m)
    {
        return -1;
    }
    DeleteCriticalSection(m);
    return 0;
}

#else
#include <pthread.h>

int dsc_mutex_init(DSCMutex* m)
{
    if (!m)
    {
        return -1;
    }
    return pthread_mutex_init(m, NULL);
}

int dsc_mutex_lock(DSCMutex* m)
{
    if (!m)
    {
        return -1;
    }
    return pthread_mutex_lock(m);
}

int dsc_mutex_trylock(DSCMutex* m)
{
    if (!m)
    {
        return -1;
    }
    const int rc = pthread_mutex_trylock(m);
    return rc == 0 ? 0 : rc; // return 0 on success, error code otherwise
}

int dsc_mutex_unlock(DSCMutex* m)
{
    if (!m)
    {
        return -1;
    }
    return pthread_mutex_unlock(m);
}

int dsc_mutex_destroy(DSCMutex* m)
{
    if (!m)
    {
        return -1;
    }
    return pthread_mutex_destroy(m);
}

#endif