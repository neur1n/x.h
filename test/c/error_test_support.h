#ifndef ERROR_TEST_SUPPORT_H
#define ERROR_TEST_SUPPORT_H

// The FILE declaration below precedes x.h. Keep its feature-test macros early
// enough for POSIX declarations when compiling with -std=c99.
#if defined(__linux__) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif
#if (defined(__linux__) || defined(__CYGWIN__)) && !defined(_FILE_OFFSET_BITS)
#define _FILE_OFFSET_BITS 64
#endif

#include <stdio.h>
#if defined(_WIN32)
// The current Windows filesystem helpers need this header.
#include <sys/stat.h>
#endif

#ifdef TEST_CPP_DISTRIBUTION
#include "../../cpp/x.h"
#else
#include "../../c/x.h"
#endif

// Unlike assert(), these checks remain active when NDEBUG is defined. A test
// failure exits normally, so it cannot masquerade as an expected abort().
#define CHECK(expression) do { \
  if (!(expression)) { \
    fprintf(stderr, "Check failed: %s (%s:%d)\n", \
        #expression, __FILE__, __LINE__); \
    exit(EXIT_FAILURE); \
  } \
} while (false)

int32_t error_peer_register(void);
x_error error_peer_set(const char* name, const int32_t value);
x_error error_peer_capture(const char* name);
void error_peer_verify(const x_error_domain* expected);

#endif
