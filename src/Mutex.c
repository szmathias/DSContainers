// Mutex.c
// Cross-platform mutex implementation
//
// Provides a thin portable wrapper around platform native mutex primitives:
// - pthread_mutex_t on POSIX systems
// - CRITICAL_SECTION on Windows
//
// The functions mirror the declarations in Mutex.h and return 0 on success
// and non-zero on failure. POSIX functions return the underlying pthread
// return codes where applicable.

#include "Mutex.h"
#include <stdlib.h>

#ifdef DSCONTAINERS_PLATFORM_WINDOWS
#include <windows.h>

/**
 * Initialize a DSCMutex (CRITICAL_SECTION) instance.
 *
 * @param mtx Pointer to an uninitialized DSCMutex.
 * @return 0 on success, -1 on invalid argument.
 */
int dsc_mutex_init(DSCMutex* mtx)
{
    if (!mtx)
    {
        return -1;
    }
    InitializeCriticalSection(mtx);
    return 0;
}

/**
 * Lock the mutex (blocking).
 *
 * @param mtx Pointer to an initialized DSCMutex.
 * @return 0 on success, -1 on invalid argument.
 */
int dsc_mutex_lock(DSCMutex* mtx)
{
    if (!mtx)
    {
        return -1;
    }
    EnterCriticalSection(mtx);
    return 0;
}

/**
 * Try to lock the mutex without blocking.
 *
 * @param mtx Pointer to an initialized DSCMutex.
 * @return 0 if lock acquired, 1 if not acquired, -1 on invalid argument.
 */
int dsc_mutex_trylock(DSCMutex* mtx)
{
    if (!mtx)
    {
        return -1;
    }
    return TryEnterCriticalSection(mtx) ? 0 : 1;
}

/**
 * Unlock the mutex.
 *
 * @param mtx Pointer to an initialized DSCMutex.
 * @return 0 on success, -1 on invalid argument.
 */
int dsc_mutex_unlock(DSCMutex* mtx)
{
    if (!mtx)
    {
        return -1;
    }
    LeaveCriticalSection(mtx);
    return 0;
}

/**
 * Destroy the mutex and free underlying resources.
 *
 * @param mtx Pointer to an initialized DSCMutex.
 * @return 0 on success, -1 on invalid argument.
 */
int dsc_mutex_destroy(DSCMutex* mtx)
{
    if (!mtx)
    {
        return -1;
    }
    DeleteCriticalSection(mtx);
    return 0;
}

#else
#include <pthread.h>

/**
 * Initialize a DSCMutex (pthread_mutex_t) instance.
 *
 * @param mtx Pointer to an uninitialized DSCMutex.
 * @return 0 on success, non-zero pthread error code on failure, -1 on invalid argument.
 */
int dsc_mutex_init(DSCMutex* mtx)
{
    if (!mtx)
    {
        return -1;
    }
    return pthread_mutex_init(mtx, NULL);
}

/**
 * Lock the mutex (blocking).
 *
 * @param mtx Pointer to an initialized DSCMutex.
 * @return 0 on success, non-zero pthread error code on failure, -1 on invalid argument.
 */
int dsc_mutex_lock(DSCMutex* mtx)
{
    if (!mtx)
    {
        return -1;
    }
    return pthread_mutex_lock(mtx);
}

/**
 * Try to lock the mutex without blocking.
 *
 * @param mtx Pointer to an initialized DSCMutex.
 * @return 0 if lock acquired, EBUSY if already locked, other pthread error codes on failure, -1 on invalid argument.
 */
int dsc_mutex_trylock(DSCMutex* mtx)
{
    if (!mtx)
    {
        return -1;
    }
    const int rc = pthread_mutex_trylock(mtx);
    return rc == 0 ? 0 : rc; // return 0 on success, error code otherwise
}

/**
 * Unlock the mutex.
 *
 * @param mtx Pointer to an initialized DSCMutex.
 * @return 0 on success, non-zero pthread error code on failure, -1 on invalid argument.
 */
int dsc_mutex_unlock(DSCMutex* mtx)
{
    if (!mtx)
    {
        return -1;
    }
    return pthread_mutex_unlock(mtx);
}

/**
 * Destroy the mutex and free underlying resources.
 *
 * @param mtx Pointer to an initialized DSCMutex.
 * @return 0 on success, non-zero pthread error code on failure, -1 on invalid argument.
 */
int dsc_mutex_destroy(DSCMutex* mtx)
{
    if (!mtx)
    {
        return -1;
    }
    return pthread_mutex_destroy(mtx);
}

#endif