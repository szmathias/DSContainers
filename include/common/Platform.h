#ifndef ANVIL_PLATFORM_H
#define ANVIL_PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Platform detection - use CMake definitions if available, otherwise auto-detect */
#if !defined(ANVIL_PLATFORM_WINDOWS) && !defined(ANVIL_PLATFORM_MACOS) && !defined(ANVIL_PLATFORM_LINUX)
    #if defined(_WIN32) || defined(_WIN64)
        #define ANVIL_PLATFORM_WINDOWS 1
    #elif defined(__APPLE__) || defined(__MACH__)
        #define ANVIL_PLATFORM_MACOS 1
    #elif defined(__linux__)
        #define ANVIL_PLATFORM_LINUX 1
    #else
        #define ANVIL_PLATFORM_UNKNOWN 1
    #endif
#endif

/* API export/import macros */
#ifdef ANVIL_PLATFORM_WINDOWS
    #ifdef ANVIL_BUILDING_DLL
        #define ANV_API __declspec(dllexport)
    #else
        #define ANV_API __declspec(dllimport)
    #endif
    #elif defined(__GNUC__) || defined(__clang__)
        #define ANV_API __attribute__((visibility("default")))
    #else
        #define ANV_API
#endif

#ifdef __cplusplus
}
#endif

#endif /* ANVIL_PLATFORM_H */