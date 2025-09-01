#ifndef DSC_PLATFORM_DEFS_H
#define DSC_PLATFORM_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Platform detection - use CMake definitions if available, otherwise auto-detect */
#if !defined(DSC_PLATFORM_WINDOWS) && !defined(DSC_PLATFORM_MACOS) && !defined(DSC_PLATFORM_LINUX)
    #if defined(_WIN32) || defined(_WIN64)
        #define DSC_PLATFORM_WINDOWS 1
    #elif defined(__APPLE__) || defined(__MACH__)
        #define DSC_PLATFORM_MACOS 1
    #elif defined(__linux__)
        #define DSC_PLATFORM_LINUX 1
    #else
        #define DSC_PLATFORM_UNKNOWN 1
    #endif
#endif
    
/* API export/import macros */
#ifdef DSC_PLATFORM_WINDOWS
    #ifdef DSC_BUILDING_DLL
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

#endif /* DSC_PLATFORM_DEFS_H */
