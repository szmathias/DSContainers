// Mutex.h
// Cross-platform mutex abstraction
//
// This header provides a small, portable wrapper around native mutex
// primitives (pthread_mutex_t on POSIX and CRITICAL_SECTION on Windows).
// The API is intentionally minimal: init, lock, trylock, unlock, and destroy.
// All functions return 0 on success and a non-zero value on failure. On
// POSIX platforms the underlying pthread return codes are returned where
// applicable.

#ifndef ANVIL_MUTEX_H
#define ANVIL_MUTEX_H

#include "common/CStandardCompatibility.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ANVIL_PLATFORM_WINDOWS
#include <windows.h>
// Structure to wrap CRITICAL_SECTION with ownership tracking
typedef struct
{
    CRITICAL_SECTION cs;
    volatile DWORD owner_thread_id;
    volatile LONG lock_count;
} ANVMutex;
#else
#include <pthread.h>
typedef pthread_mutex_t ANVMutex;
#endif

/**
 * Initialize a mutex object.
 *
 * @param mtx Pointer to an uninitialized ANVMutex.
 * @return 0 on success, non-zero on failure.
 *
 * Notes:
 * - On success the mutex is ready to use with anv_mutex_lock/trylock/unlock.
 * - On failure the value of *m is undefined and anv_mutex_destroy must not be
 *   called on it.
 */
ANV_API int anv_mutex_init(ANVMutex* mtx);

/**
 * Acquire (lock) the mutex. This call blocks until the mutex is available.
 *
 * @param mtx Pointer to an initialized ANVMutex.
 * @return 0 on success, non-zero on failure.
 */
ANV_API int anv_mutex_lock(ANVMutex* mtx);

/**
 * Attempt to acquire (trylock) the mutex without blocking.
 *
 * @param mtx Pointer to an initialized ANVMutex.
 * @return 0 if the lock was acquired, non-zero if not acquired or on error.
 *
 * Notes:
 * - On POSIX this returns the pthread error code (0 on success, EBUSY if the
 *   mutex is already locked, or another error code on failure).
 * - On Windows this returns 0 on success and 1 if the lock was not acquired.
 */
ANV_API int anv_mutex_trylock(ANVMutex* mtx);

/**
 * Release (unlock) the mutex.
 *
 * @param mtx Pointer to an initialized ANVMutex.
 * @return 0 on success, non-zero on failure.
 */
ANV_API int anv_mutex_unlock(ANVMutex* mtx);

/**
 * Destroy a mutex and free any underlying resources.
 *
 * @param mtx Pointer to an initialized ANVMutex.
 * @return 0 on success, non-zero on failure.
 *
 * Notes:
 * - The mutex must be unlocked and not in use by any thread when calling
 *   anv_mutex_destroy.
 */
ANV_API int anv_mutex_destroy(ANVMutex* mtx);

#ifdef __cplusplus
}
#endif

#endif // ANVIL_MUTEX_H