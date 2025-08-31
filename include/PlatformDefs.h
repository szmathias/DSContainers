#ifndef DS_PLATFORM_DEFS_H
#define DS_PLATFORM_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Platform detection - use CMake definitions if available, otherwise auto-detect */
#if !defined(DS_PLATFORM_WINDOWS) && !defined(DS_PLATFORM_MACOS) && !defined(DS_PLATFORM_LINUX)
    #if defined(_WIN32) || defined(_WIN64)
        #define DS_PLATFORM_WINDOWS 1
    #elif defined(__APPLE__) || defined(__MACH__)
        #define DS_PLATFORM_MACOS 1
    #elif defined(__linux__)
        #define DS_PLATFORM_LINUX 1
    #else
        #define DS_PLATFORM_UNKNOWN 1
    #endif
#endif
    
/* API export/import macros */
#ifdef DS_PLATFORM_WINDOWS
    #ifdef DS_BUILDING_DLL
        #define DS_API __declspec(dllexport)
    #else
        #define DS_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #define DS_API __attribute__((visibility("default")))
#else
    #define DS_API
#endif
    
#ifdef __cplusplus
}
#endif

#endif /* DS_PLATFORM_DEFS_H */
