//
// Created by zack on 8/31/25.
//

#ifndef DSC_THREADS_H
#define DSC_THREADS_H

#ifdef DSC_PLATFORM_WINDOWS
#include <windows.h>
typedef HANDLE DSCThread;
#else
#include <pthread.h>
typedef pthread_t DSCThread;
#endif

// Thread function signature
typedef void* (*dscthread_func)(void*);

int dsc_thread_create(DSCThread* thread, dscthread_func func, void* arg);
int dsc_thread_join(DSCThread thread, void** retval);
int dsc_thread_detach(DSCThread thread);

#endif //DSC_THREADS_H