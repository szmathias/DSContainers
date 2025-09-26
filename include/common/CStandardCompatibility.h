//
// Created by zack on 8/27/25.
//

#ifndef ANVIL_CSTANDARDCOMPATIBILITY_H
#define ANVIL_CSTANDARDCOMPATIBILITY_H

#include "Platform.h"

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
#define ANVIL_NODISCARD [[nodiscard]]
#define ANVIL_DEPRECATED [[deprecated]]
#define ANVIL_NORETURN [[noreturn]]
#elif defined(__GNUC__) || defined(__clang__)
#define ANVIL_NODISCARD __attribute__((warn_unused_result))
#define ANVIL_DEPRECATED __attribute__((deprecated))
#define ANVIL_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
#define ANVIL_NODISCARD _Check_return_
#define ANVIL_DEPRECATED __declspec(deprecated)
#define ANVIL_NORETURN __declspec(noreturn)
#else
#define ANVIL_NODISCARD
#define ANVIL_DEPRECATED
#define ANVIL_NORETURN
#endif

#ifdef __cplusplus
}
#endif

#endif /* ANVIL_CSTANDARDCOMPATIBILITY_H */