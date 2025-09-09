//
// Created by zack on 9/3/25.
//

#include "DynamicString.h"
#include "TestAssert.h"
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#ifdef __linux__
#include <sys/random.h>
#endif
#else
#include <windows.h>
#include <bcrypt.h>
#ifdef _MSC_VER
#pragma comment(lib, "bcrypt")
#endif
#endif

#define NUM_FUZZ_OPERATIONS 100000

// Small SplitMix64 implementation for deterministic but higher-quality PRNG
static uint64_t fuzz_rng_state = 0;

static void fuzz_seed(void)
{
    uint64_t seed = 0;

    #ifdef _WIN32
    // Use Windows CNG to get random bytes
    if (BCryptGenRandom(NULL, (PUCHAR) & seed, (ULONG)sizeof(seed), BCRYPT_USE_SYSTEM_PREFERRED_RNG) == 0)
    {
        fuzz_rng_state = seed;
        return;
    }
    #else
    #ifdef SYS_getrandom
    // Try getrandom first (Linux)
    if (getrandom(&seed, sizeof(seed), 0) == (ssize_t)sizeof(seed))
    {
        fuzz_rng_state = seed;
        return;
    }
    #endif
    // Try /dev/urandom
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd >= 0)
    {
        ssize_t r = read(fd, &seed, sizeof(seed));
        close(fd);
        if (r == (ssize_t)sizeof(seed))
        {
            fuzz_rng_state = seed;
            return;
        }
    }
    #endif
    // Fallback: mix time, pid and address
    seed = (uint64_t)time(NULL);
    #ifdef _WIN32
    seed ^= (uint64_t)GetCurrentProcessId() << 21;
    #else
    seed ^= (uint64_t)getpid() << 21;
    #endif
    seed ^= (uint64_t)(uintptr_t) & seed << 7;
    fuzz_rng_state = seed ? seed : 0xF00BAAULL; // ensure non-zero
}

static uint64_t fuzz_next_u64(void)
{
    // splitmix64
    uint64_t z = (fuzz_rng_state += 0x9E3779B97F4A7C15ULL);
    z          = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z          = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

static uint32_t fuzz_next_u32(void)
{
    return (uint32_t)(fuzz_next_u64() & 0xFFFFFFFFu);
}

static void ensure_seeded(void)
{
    if (fuzz_rng_state == 0)
    {
        fuzz_seed();
    }
}

static unsigned fuzz_rand_range(const unsigned bound)
{
    ensure_seeded();
    if (bound == 0)
        return 0u;
    return (unsigned)(fuzz_next_u32() % bound);
}

static size_t fuzz_rand_size(size_t bound)
{
    ensure_seeded();
    if (bound == 0)
        return 0;
    return (size_t)(fuzz_next_u32() % (uint32_t)bound);
}

static void perform_random_operation(DSCString* str)
{
    const int op = fuzz_rand_range(8);

    switch (op)
    {
        case 0:                                                       // push_back
            dsc_str_push_back(str, (char)(fuzz_rand_range(95) + 32)); // Printable ASCII
            break;
        case 1: // pop_back
            if (!dsc_str_empty(str))
            {
                dsc_str_pop_back(str);
            }
            break;
        case 2: // insert
            if (dsc_str_size(str) > 0)
            {
                dsc_str_insert_char(str, fuzz_rand_size(dsc_str_size(str)), 'X');
            }
            break;
        case 3: // erase
            if (dsc_str_size(str) > 0)
            {
                dsc_str_erase(str, fuzz_rand_size(dsc_str_size(str)));
            }
            break;
        case 4: // assign
            dsc_str_assign_cstring(str, "fuzz");
            break;
        case 5: // clear
            dsc_str_clear(str);
            break;
        case 6: // trim
            dsc_str_trim_front(str);
            dsc_str_trim_back(str);
            break;
        case 7: // reserve
            dsc_str_reserve(str, (size_t)fuzz_rand_range(256));
            break;

        default:
            break;
    }
}

int test_string_fuzz(void)
{
    ensure_seeded();
    DSCString str = dsc_str_create_empty(0);

    for (int i = 0; i < NUM_FUZZ_OPERATIONS; ++i)
    {
        perform_random_operation(&str);
    }

    dsc_str_destroy(&str);
    printf("DString fuzz test completed %d operations without crashing.\n", NUM_FUZZ_OPERATIONS);
    return TEST_SUCCESS;
}

int main(void)
{
    return test_string_fuzz() ? 0 : 1;
}