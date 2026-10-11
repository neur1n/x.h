#ifndef X_TEST_HARDWARE_FAULT_CASES_H
#define X_TEST_HARDWARE_FAULT_CASES_H

// Exercise the actual C/C++ implementation with deterministic CPU queries.
// Native-header tests separately check compilation against the real SDK.

#if TEST_WINDOWS_UNSPECIFIED
// Keep this branch deterministic even if the toolchain supplies a default.
#undef _WIN32_WINNT
#endif

#define X_WINDOWS TEST_WINDOWS
#define CHECK(condition) do { \
  if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    exit(EXIT_FAILURE); \
  } \
} while (0)

static long query_result;
static int query_calls;

#if TEST_WINDOWS
typedef uint32_t DWORD;
#if defined(_WIN32_WINNT) && _WIN32_WINNT >= 0x0601
#define ALL_PROCESSOR_GROUPS 0xFFFF
static DWORD GetActiveProcessorCount(unsigned short group)
{
  CHECK(group == ALL_PROCESSOR_GROUPS);
  ++query_calls;
  return (DWORD)query_result;
}
#else
typedef struct {
  DWORD dwNumberOfProcessors;
} SYSTEM_INFO;
static void GetSystemInfo(SYSTEM_INFO* info)
{
  CHECK(info->dwNumberOfProcessors == 0);
  ++query_calls;
  info->dwNumberOfProcessors = (DWORD)query_result;
}
#endif
#else
#define _SC_NPROCESSORS_ONLN 123
static long sysconf(int name)
{
  CHECK(name == _SC_NPROCESSORS_ONLN);
  ++query_calls;
  return query_result;
}
#endif

#include "hardware_under_test.h"

static void check_hardware(void)
{
  query_result = 128;
  CHECK(x_ncpu() == 128);
  CHECK(query_calls == 1);
  query_result = 1;
  CHECK(x_ncpu() == 1);
  CHECK(query_calls == 2);
  query_result = 0;
  CHECK(x_ncpu() == 0);
  CHECK(query_calls == 3);
#if !TEST_WINDOWS
  query_result = -1;
  CHECK(x_ncpu() == 0);
  CHECK(query_calls == 4);
#endif
}

#endif
