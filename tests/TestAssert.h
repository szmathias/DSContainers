//
// Created by szmat on 8/20/2025.
//

#ifndef DS_TESTASSERT_H
#define DS_TESTASSERT_H

#include <string.h>
#include <math.h>
#include <stdio.h>

#define TEST_SUCCESS 1
#define TEST_FAILURE 0
#define TEST_SKIPPED -1

#define ASSERT(expr) \
	do \
	{ \
		if (!(expr)) \
		{ \
			fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", #expr, __FILE__, __LINE__); \
			return TEST_FAILURE; \
		} \
	} while (0)

#define ASSERT_EQ(a, b) \
	do \
	{ \
		if ((a) != (b)) \
		{ \
			fprintf(stderr, "Assertion failed: %s == %s, file %s, line %d\n", #a, #b, __FILE__, __LINE__); \
			fprintf(stderr, "  Actual: %lld != %lld\n", (long long)(a), (long long)(b)); \
			return TEST_FAILURE; \
		} \
	} while (0)

#define ASSERT_EQ_STR(a, b) \
	do \
	{ \
		size_t len_a = strlen(a); \
		size_t len_b = strlen(b); \
		if (len_a != len_b || strncmp((a), (b), len_a) != 0) \
		{ \
			fprintf(stderr, "Assertion failed: %s == %s, file %s, line %d\n", #a, #b, __FILE__, __LINE__); \
			fprintf(stderr, "  Actual: '%s' (len=%zu) != '%s' (len=%zu)\n", (a), len_a, (b), len_b); \
			return TEST_FAILURE; \
		} \
	} while (0)

#define ASSERT_EQ_FLOAT(a, b, eps) \
	do \
	{ \
		if (fabs((a) - (b)) > (eps)) \
		{ \
			fprintf(stderr, "Assertion failed: fabs(%s - %s) <= %s, file %s, line %d\n", #a, #b, #eps, __FILE__, __LINE__); \
			fprintf(stderr, "  Actual: %g != %g (eps=%g)\n", (double)(a), (double)(b), (double)(eps)); \
			return TEST_FAILURE; \
		} \
	} while (0)

#define ASSERT_EQ_PTR(a, b) \
	do \
	{ \
		if ((void*)(a) != (void*)(b)) \
		{ \
			fprintf(stderr, "Assertion failed: %s == %s, file %s, line %d\n", #a, #b, __FILE__, __LINE__); \
			fprintf(stderr, "  Actual: %p != %p\n", (void*)(a), (void*)(b)); \
			return TEST_FAILURE; \
		} \
	} while (0)

#define ASSERT_NEQ(a, b) \
	do \
	{ \
		if ((a) == (b)) \
		{ \
			fprintf(stderr, "Assertion failed: %s != %s, file %s, line %d\n", #a, #b, __FILE__, __LINE__); \
			fprintf(stderr, "  Actual: %lld == %lld\n", (long long)(a), (long long)(b)); \
			return TEST_FAILURE; \
		} \
	} while (0)

#define ASSERT_NEQ_STR(a, b) \
	do \
	{ \
		size_t _len_a = strlen(a); \
		size_t _len_b = strlen(b); \
		if (_len_a == _len_b && strncmp((a), (b), _len_a) == 0) \
		{ \
			fprintf(stderr, "Assertion failed: %s != %s, file %s, line %d\n", #a, #b, __FILE__, __LINE__); \
			fprintf(stderr, "  Actual: '%s' == '%s'\n", (a), (b)); \
			return TEST_FAILURE; \
		} \
	} while (0)

#define ASSERT_NEQ_PTR(a, b) \
	do \
	{ \
		if ((void*)(a) == (void*)(b)) \
		{ \
			fprintf(stderr, "Assertion failed: %s != %s, file %s, line %d\n", #a, #b, __FILE__, __LINE__); \
			fprintf(stderr, "  Actual: %p == %p\n", (void*)(a), (void*)(b)); \
			return TEST_FAILURE; \
		} \
	} while (0)

#define ASSERT_NULL(ptr) \
	do \
	{ \
		if ((ptr) != NULL) \
		{ \
			fprintf(stderr, "Assertion failed: %s == NULL, file %s, line %d\n", #ptr, __FILE__, __LINE__); \
			fprintf(stderr, "  Actual: %p != NULL\n", (void*)(ptr)); \
			return TEST_FAILURE; \
		} \
	} while (0)

#define ASSERT_NOT_NULL(ptr) \
	do \
	{ \
		if ((ptr) == NULL) \
		{ \
			fprintf(stderr, "Assertion failed: %s != NULL, file %s, line %d\n", #ptr, __FILE__, __LINE__); \
			return TEST_FAILURE; \
		} \
	} while (0)

#define ASSERT_GT(a, b) \
	do \
	{ \
		if (!((a) > (b))) \
		{ \
			fprintf(stderr, "Assertion failed: %s > %s, file %s, line %d\n", #a, #b, __FILE__, __LINE__); \
			fprintf(stderr, "  Actual: %lld <= %lld\n", (long long)(a), (long long)(b)); \
			return TEST_FAILURE; \
		} \
	} while (0)

#define ASSERT_LT(a, b) \
	do \
	{ \
		if (!((a) < (b))) \
		{ \
			fprintf(stderr, "Assertion failed: %s < %s, file %s, line %d\n", #a, #b, __FILE__, __LINE__); \
			fprintf(stderr, "  Actual: %lld >= %lld\n", (long long)(a), (long long)(b)); \
			return TEST_FAILURE; \
		} \
	} while (0)

#define ASSERT_GTE(a, b) \
	do \
	{ \
		if (!((a) >= (b))) \
		{ \
			fprintf(stderr, "Assertion failed: %s >= %s, file %s, line %d\n", #a, #b, __FILE__, __LINE__); \
			fprintf(stderr, "  Actual: %lld < %lld\n", (long long)(a), (long long)(b)); \
			return TEST_FAILURE; \
		} \
	} while (0)

#define ASSERT_LTE(a, b) \
	do \
	{ \
		if (!((a) <= (b))) \
		{ \
			fprintf(stderr, "Assertion failed: %s <= %s, file %s, line %d\n", #a, #b, __FILE__, __LINE__); \
			fprintf(stderr, "  Actual: %lld > %lld\n", (long long)(a), (long long)(b)); \
			return TEST_FAILURE; \
		} \
	} while (0)

#define ASSERT_TRUE(expr) \
	do \
	{ \
		if (!(expr)) \
		{ \
			fprintf(stderr, "Assertion failed: %s is true, file %s, line %d\n", #expr, __FILE__, __LINE__); \
			return TEST_FAILURE; \
		} \
	} while (0)

#define ASSERT_FALSE(expr) \
	do \
	{ \
		if (expr) \
		{ \
			fprintf(stderr, "Assertion failed: %s is false, file %s, line %d\n", #expr, __FILE__, __LINE__); \
			return TEST_FAILURE; \
		} \
	} while (0)

#endif //DS_TESTASSERT_H