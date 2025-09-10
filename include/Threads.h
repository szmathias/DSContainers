//
// Created by zack on 8/31/25.
// Threads.h
// Simple cross-platform thread abstraction
//
// This header defines a minimal, portable wrapper around native thread
// primitives (pthread_t on POSIX and HANDLE on Windows). The API is
// intentionally small: create, join and detach. Thread functions follow a
// single conventional signature: void* func(void*). All functions return 0
// on success and a non-zero value on failure. Platform specific notes are
// documented for each function.
//

#ifndef DSCONTAINERS_THREADS_H
#define DSCONTAINERS_THREADS_H

#include "PlatformDefs.h"

#ifdef DSCONTAINERS_PLATFORM_WINDOWS
#include <windows.h>
typedef HANDLE DSCThread;
#else
#include <pthread.h>
typedef pthread_t DSCThread;
#endif

/**
 * Thread function signature.
 *
 * This matches pthreads and the Win32 thread start signature (adapted).
 * The function should return a pointer-sized result (can be NULL).
 */
typedef void* (*dscthread_func)(void*);

/**
 * Create a new thread.
 *
 * @param thread Pointer to a DSCThread variable that will receive the
 *               thread handle/identifier on success.
 * @param func   The function to run on the new thread.
 * @param arg    Argument passed to the thread function.
 * @return 0 on success, non-zero on failure.
 *
 * Notes:
 * - On POSIX this returns the pthread_create return code (0 on success).
 * - On Windows this returns 0 on success and -1 on failure.
 */
DSC_API int dsc_thread_create(DSCThread* thread, dscthread_func func, void* arg);

/**
 * Join a thread, waiting for it to finish and retrieving its return value.
 *
 * @param thread The thread handle/identifier to join.
 * @param retval If non-NULL, receives the thread function's return value.
 * @return 0 on success, non-zero on failure.
 *
 * Notes:
 * - On POSIX this returns the pthread_join return code.
 * - On Windows this waits for the thread to exit, retrieves a stored result
 *   (if available) and returns 0 on success; otherwise a non-zero value.
 */
DSC_API int dsc_thread_join(DSCThread thread, void** retval);

/**
 * Detach a thread, allowing resources to be released when it exits.
 *
 * @param thread The thread handle/identifier to detach.
 * @return 0 on success, non-zero on failure.
 *
 * Notes:
 * - On POSIX this returns the pthread_detach return code.
 * - On Windows this marks the thread as detached and closes the handle. The
 *   library attempts to clean up any stored thread wrapper resources once the
 *   thread exits.
 */
DSC_API int dsc_thread_detach(DSCThread thread);

#endif // DSCONTAINERS_THREADS_H