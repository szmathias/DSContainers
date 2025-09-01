// Mutex.h
// Cross-platform mutex abstraction

#ifndef DSC_MUTEX_H
#define DSC_MUTEX_H

#ifdef DSC_PLATFORM_WINDOWS
#include <windows.h>
typedef CRITICAL_SECTION DSCMutex;
#else
#include <pthread.h>
typedef pthread_mutex_t DSCMutex;
#endif

int dsc_mutex_init(DSCMutex* m);
int dsc_mutex_lock(DSCMutex* m);
int dsc_mutex_trylock(DSCMutex* m);
int dsc_mutex_unlock(DSCMutex* m);
int dsc_mutex_destroy(DSCMutex* m);

#endif // DSC_MUTEX_H
