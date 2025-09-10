#ifndef DSCONTAINERS_PLATFORM_DEFS_H
#define DSCONTAINERS_PLATFORM_DEFS_H

#ifdef __cplusplus
extern "C" {
    #endif

    /* Platform detection - use CMake definitions if available, otherwise auto-detect */
    #if !defined(DSCONTAINERS_PLATFORM_WINDOWS) && !defined(DSCONTAINERS_PLATFORM_MACOS) && !defined(DSCONTAINERS_PLATFORM_LINUX)
        #if defined(_WIN32) || defined(_WIN64)
            #define DSCONTAINERS_PLATFORM_WINDOWS 1
        #elif defined(__APPLE__) || defined(__MACH__)
            #define DSCONTAINERS_PLATFORM_MACOS 1
        #elif defined(__linux__)
            #define DSCONTAINERS_PLATFORM_LINUX 1
        #else
            #define DSCONTAINERS_PLATFORM_UNKNOWN 1
        #endif
    #endif

    /* API export/import macros */
    #ifdef DSCONTAINERS_PLATFORM_WINDOWS
        #ifdef DSCONTAINERS_BUILDING_DLL
            #define DSC_API __declspec(dllexport)
        #else
            #define DSC_API __declspec(dllimport)
        #endif
        #elif defined(__GNUC__) || defined(__clang__)
            #define DSC_API __attribute__((visibility("default")))
        #else
            #define DSC_API
    #endif

    #ifdef __cplusplus
}
#endif

#endif /* DSCONTAINERS_PLATFORM_DEFS_H */