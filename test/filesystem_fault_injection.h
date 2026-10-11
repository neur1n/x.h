// Include the runtime declarations before redirecting the path resolver.
#if defined(__linux__) || defined(__CYGWIN__)
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#endif
#if defined(__linux__)
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#endif

#ifdef __cplusplus
#include <cerrno>
#include <cstdlib>
#else
#include <errno.h>
#include <stdlib.h>
#endif

static int test_resolve_error;
static size_t test_resolve_calls;

#if defined(_WIN32) && !defined(__CYGWIN__)
static char* test_resolve(char* destination, const char* source, size_t size)
{
  ++test_resolve_calls;
  if (test_resolve_error != 0) {
    errno = test_resolve_error;
    return NULL;
  }
  return _fullpath(destination, source, size);
}
#define _fullpath test_resolve
#else
static char* test_resolve(const char* source, char* destination)
{
  ++test_resolve_calls;
  if (test_resolve_error != 0) {
    errno = test_resolve_error;
    return NULL;
  }
  return realpath(source, destination);
}
#define realpath test_resolve
#endif
