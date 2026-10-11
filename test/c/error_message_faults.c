#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expression) do { \
  if (!(expression)) { \
    fprintf(stderr, "Check failed: %s (%s:%d)\n", \
        #expression, __FILE__, __LINE__); \
    exit(EXIT_FAILURE); \
  } \
} while (0)

#define X_INL static inline
#define X_WINDOWS 0
#undef __GLIBC__
#if TEST_INTERFACE == 1
#define __GLIBC__ 2
#endif
#define X_CYGWIN (TEST_INTERFACE == 2)

static int mode;
static int calls;
static const char text[] = "error %s %n %%";

#if TEST_INTERFACE != 0
static char* test_strerror_r(int value, char* buffer, size_t size)
#else
static int test_strerror_r(int value, char* buffer, size_t size)
#endif
{
  CHECK(value == 123 && size > 0);
  ++calls;
  if (mode == 0 || mode == 1) {
    snprintf(buffer, size, "%s", text);
  } else if (mode == 3) {
    // An XSI ERANGE result need not leave a terminated buffer.
    memset(buffer, 'T', size);
#if TEST_INTERFACE != 0
    buffer[size - 1] = '\0';
#endif
  } else {
    buffer[0] = '\0';
  }
#if TEST_INTERFACE != 0
  if (mode == 0) return (char*)text;  // GNU immutable-text result.
  if (mode == 2) return NULL;
  return buffer;
#else
  if (mode == 2) return EINVAL;
  return mode == 3 ? ERANGE : 0;
#endif
}

#define strerror_r test_strerror_r
#include "error_message_under_test.h"
#undef strerror_r

int main(void)
{
  CHECK(_x_error_message_posix(NULL, 8, 123) == NULL && calls == 0);
  const size_t capacities[] = {0, 1, 4, 32, 300};
  for (mode = 0; mode < 5; ++mode) {
    for (size_t i = 0; i < sizeof(capacities) / sizeof(capacities[0]); ++i) {
      char storage[302];
      memset(storage, 'Z', sizeof(storage));
      char* buffer = storage + 1;
      const size_t size = capacities[i];
      int before = calls;
      CHECK(_x_error_message_posix(buffer, size, 123) == buffer);
      CHECK(calls == before + (size != 0));
      CHECK(storage[0] == 'Z');
      for (size_t j = size + 1; j < sizeof(storage); ++j) {
        CHECK(storage[j] == 'Z');
      }
      if (size == 0) {
        CHECK(buffer[0] == 'Z');
        continue;
      }
      CHECK(memchr(buffer, '\0', size) != NULL);
      const char* expected = mode < 2 ? text : "Unknown error 123";
      if (mode == 3) {
        size_t len = size - 1;
        CHECK(strlen(buffer) == len);
        for (size_t j = 0; j < len; ++j) CHECK(buffer[j] == 'T');
      } else {
        size_t len = strlen(expected);
        if (len >= size) len = size - 1;
        CHECK(strlen(buffer) == len && memcmp(buffer, expected, len) == 0);
      }
    }
  }
  return EXIT_SUCCESS;
}
