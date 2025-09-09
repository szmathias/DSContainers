//
// Created by zack on 8/27/25.
//

#ifndef DSCONTAINERS_CSTANDARDCOMPATIBILITY_H
#define DSCONTAINERS_CSTANDARDCOMPATIBILITY_H

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
    #define DSCONTAINERS_NODISCARD [[nodiscard]]
    #define DSCONTAINERS_DEPRECATED [[deprecated]]
    #define DSCONTAINERS_NORETURN [[noreturn]]
    #elif defined(__GNUC__) || defined(__clang__)
    #define DSCONTAINERS_NODISCARD __attribute__((warn_unused_result))
    #define DSCONTAINERS_DEPRECATED __attribute__((deprecated))
    #define DSCONTAINERS_NORETURN __attribute__((noreturn))
    #elif defined(_MSC_VER)
    #define DSCONTAINERS_NODISCARD _Check_return_
    #define DSCONTAINERS_DEPRECATED __declspec(deprecated)
    #define DSCONTAINERS_NORETURN __declspec(noreturn)
    #else
    #define DSCONTAINERS_NODISCARD
    #define DSCONTAINERS_DEPRECATED
    #define DSCONTAINERS_NORETURN
    #endif

    #ifdef __cplusplus
}
#endif

#endif /* DSCONTAINERS_CSTANDARDCOMPATIBILITY_H */