//
// Created by zack on 8/27/25.
//

#ifndef DS_CSTANDARDCOMPATIBILITY_H
#define DS_CSTANDARDCOMPATIBILITY_H

#include "PlatformDefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Require C11 or higher */
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
    #error "C11 or higher required"
#endif

/* Detect C23 for standard attributes */
#if __STDC_VERSION__ >= 202311L
    #define COMPAT_HAS_C23 1
#endif

#if !defined(COMPAT_HAS_C23) || !defined(__cplusplus)
    #define nullptr NULL
    #define constexpr const
    #include <stdbool.h>
    #define static_assert(condition, message) _Static_assert(condition, message)
#endif
    
/* Attribute compatibility macros */
#ifdef COMPAT_HAS_C23
    #define DS_NODISCARD [[nodiscard]]
    #define DS_DEPRECATED [[deprecated]]
    #define DS_NORETURN [[noreturn]]
#elif defined(__GNUC__) || defined(__clang__)
    #define DS_NODISCARD __attribute__((warn_unused_result))
    #define DS_DEPRECATED __attribute__((deprecated))
    #define DS_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
    #define DS_NODISCARD _Check_return_
    #define DS_DEPRECATED __declspec(deprecated)
    #define DS_NORETURN __declspec(noreturn)
#else
    #define DS_NODISCARD
    #define DS_DEPRECATED
    #define DS_NORETURN
#endif

#ifdef __cplusplus
}
#endif

#endif /* DS_CSTANDARDCOMPATIBILITY_H */
