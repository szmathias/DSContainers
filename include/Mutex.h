// Mutex.h
// Cross-platform mutex abstraction
//
// This header provides a small, portable wrapper around native mutex
// primitives (pthread_mutex_t on POSIX and CRITICAL_SECTION on Windows).
// The API is intentionally minimal: init, lock, trylock, unlock, and destroy.
// All functions return 0 on success and a non-zero value on failure. On
// POSIX platforms the underlying pthread return codes are returned where
// applicable.

#ifndef DSCONTAINERS_MUTEX_H
#define DSCONTAINERS_MUTEX_H

#include "PlatformDefs.h"

#ifdef DSCONTAINERS_PLATFORM_WINDOWS
#include <windows.h>
// Structure to wrap CRITICAL_SECTION with ownership tracking
typedef struct
{
    CRITICAL_SECTION cs;
    volatile DWORD owner_thread_id;
    volatile LONG lock_count;
} DSCMutex;
#else
#include <pthread.h>
typedef pthread_mutex_t DSCMutex;
#endif

/**
 * Initialize a mutex object.
 *
 * @param mtx Pointer to an uninitialized DSCMutex.
 * @return 0 on success, non-zero on failure.
 *
 * Notes:
 * - On success the mutex is ready to use with dsc_mutex_lock/trylock/unlock.
 * - On failure the value of *m is undefined and dsc_mutex_destroy must not be
 *   called on it.
 */
DSC_API int dsc_mutex_init(DSCMutex* mtx);

/**
 * Acquire (lock) the mutex. This call blocks until the mutex is available.
 *
 * @param mtx Pointer to an initialized DSCMutex.
 * @return 0 on success, non-zero on failure.
 */
DSC_API int dsc_mutex_lock(DSCMutex* mtx);

/**
 * Attempt to acquire (trylock) the mutex without blocking.
 *
 * @param mtx Pointer to an initialized DSCMutex.
 * @return 0 if the lock was acquired, non-zero if not acquired or on error.
 *
 * Notes:
 * - On POSIX this returns the pthread error code (0 on success, EBUSY if the
 *   mutex is already locked, or another error code on failure).
 * - On Windows this returns 0 on success and 1 if the lock was not acquired.
 */
DSC_API int dsc_mutex_trylock(DSCMutex* mtx);

/**
 * Release (unlock) the mutex.
 *
 * @param mtx Pointer to an initialized DSCMutex.
 * @return 0 on success, non-zero on failure.
 */
DSC_API int dsc_mutex_unlock(DSCMutex* mtx);

/**
 * Destroy a mutex and free any underlying resources.
 *
 * @param mtx Pointer to an initialized DSCMutex.
 * @return 0 on success, non-zero on failure.
 *
 * Notes:
 * - The mutex must be unlocked and not in use by any thread when calling
 *   dsc_mutex_destroy.
 */
DSC_API int dsc_mutex_destroy(DSCMutex* mtx);

#endif // DSCONTAINERS_MUTEX_H