// Compile real implementation sections against deterministic platform APIs.
#include <cerrno>
#include <cfloat>
#include <climits>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include "support.h"

#define X_INL inline
#define X_LINUX (TEST_PLATFORM == 1)
#define X_MACOS (TEST_PLATFORM == 2)
#define X_WINDOWS (TEST_PLATFORM == 3)
#define X_ENABLE_CUDA_DRIVER 0
#define X_ENABLE_CUDA_RUNTIME 0
#if TEST_NARROW
#undef SIZE_MAX
#define SIZE_MAX UINT32_MAX
#endif

static int query_error, time_error;
static uint64_t free_bytes{4096}, total_bytes{65536};
static int32_t win32_error{13};
class x_error
{
public:
  x_error() = default;
  explicit x_error(const char* name) : m_name{name}, m_value{strcmp(name, "win32") == 0 ? win32_error : errno} {}
  x_error(const char* name, int32_t value) : m_name{name}, m_value{value} {}
  x_error& set(const char* name) { return *this = x_error(name); }
  x_error& set(const char* name, int32_t value) { return *this = x_error(name, value); }
  const char* domain() const { return this->m_name; }
  int32_t value() const { return this->m_value; }
  const char* message() const { return "injected error"; }
  operator bool() const { return m_value != 0; }
private:
  const char* m_name{"posix"};
  int32_t m_value{0};
};
static x_error x_ok() { return {}; }
static bool x_fail(const x_error& error) { return bool(error); }
static bool x_succ(const x_error& error) { return !bool(error); }
static const char* _x_timestamp(char* buffer, size_t size)
{ snprintf(buffer, size, "timestamp"); return buffer; }

#if X_WINDOWS
struct LARGE_INTEGER { int64_t QuadPart; };
struct MEMORYSTATUSEX { unsigned long dwLength; uint64_t ullAvailPhys, ullTotalPhys; };
struct SYSTEM_INFO { unsigned long dwNumberOfProcessors; };
static int64_t frequency{1000000000}, counter_value;
static bool frequency_error;
static size_t cpu_count{7};
static bool QueryPerformanceFrequency(LARGE_INTEGER* value)
{ value->QuadPart = frequency; return !frequency_error; }
static bool QueryPerformanceCounter(LARGE_INTEGER* value)
{ value->QuadPart = counter_value; return !time_error; }
static bool GlobalMemoryStatusEx(MEMORYSTATUSEX* value)
{
  CHECK(value->dwLength == sizeof(*value));
  if (query_error) { return false; }
  value->ullAvailPhys = free_bytes;
  value->ullTotalPhys = total_bytes;
  return true;
}
#define ALL_PROCESSOR_GROUPS 0xFFFF
#if _WIN32_WINNT >= 0x0601
static unsigned long GetActiveProcessorCount(unsigned short groups)
{ CHECK(groups == ALL_PROCESSOR_GROUPS); return static_cast<unsigned long>(cpu_count); }
#else
static void GetSystemInfo(SYSTEM_INFO* value)
{ value->dwNumberOfProcessors = static_cast<unsigned long>(cpu_count); }
#endif
static void Sleep(unsigned long) {}
#else
static long clock_nanoseconds;
static time_t clock_seconds;
static long cpu_count{7};
#undef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#undef _SC_NPROCESSORS_ONLN
#define _SC_NPROCESSORS_ONLN 2
#define clock_gettime test_clock_gettime
#define nanosleep test_nanosleep
#define sysconf test_sysconf
static int clock_gettime(int clock, struct timespec* value)
{
  CHECK(clock == CLOCK_MONOTONIC);
  if (time_error) { errno = time_error; return -1; }
  value->tv_sec = clock_seconds;
  value->tv_nsec = clock_nanoseconds;
  return 0;
}
static long sysconf(int name) { CHECK(name == _SC_NPROCESSORS_ONLN); return cpu_count; }
static int nanosleep(const struct timespec*, struct timespec*) { return 0; }
#endif
#if X_LINUX
struct sysinfo { unsigned long freeram, totalram; unsigned int mem_unit; };
static unsigned int memory_unit{1024};
static int sysinfo(struct sysinfo* info)
{
  if (query_error) { errno = query_error; return -1; }
  info->freeram = memory_unit == 0 ? 0 : static_cast<unsigned long>(free_bytes / memory_unit);
  info->totalram = memory_unit == 0 ? 0 : static_cast<unsigned long>(total_bytes / memory_unit);
  info->mem_unit = memory_unit;
  return 0;
}
#elif X_MACOS
using host_t = unsigned int;
using vm_size_t = uint64_t;
using kern_return_t = int;
using mach_msg_type_number_t = unsigned int;
using host_info64_t = int64_t*;
struct vm_statistics64_data_t { uint32_t free_count, speculative_count; };
#define KERN_SUCCESS 0
#define HOST_VM_INFO64 4
#define HOST_VM_INFO64_COUNT 2
static int page_error, stats_error, release_error, acquired, released;
static int total_queries, stats_queries;
static bool short_sysctl;
static vm_size_t page_bytes{4096};
static int sysctlbyname(const char* name, void* output, size_t* size, void* input, size_t input_size)
{
  ++total_queries;
  CHECK(strcmp(name, "hw.memsize") == 0 && input == nullptr && input_size == 0);
  if (query_error) { errno = query_error; return -1; }
  *static_cast<uint64_t*>(output) = total_bytes;
  if (short_sysctl) { --*size; }
  return 0;
}
static host_t mach_host_self() { ++acquired; return 11; }
static host_t mach_task_self() { return 12; }
static kern_return_t host_page_size(host_t host, vm_size_t* size)
{ CHECK(host == 11); *size = page_bytes; return page_error; }
static kern_return_t host_statistics64(host_t host, int flavor,
    host_info64_t output, mach_msg_type_number_t* count)
{
  ++stats_queries;
  CHECK(host == 11 && flavor == HOST_VM_INFO64 && *count == HOST_VM_INFO64_COUNT);
  auto* info = reinterpret_cast<vm_statistics64_data_t*>(output);
  info->free_count = page_bytes == 0 ? 1 : static_cast<uint32_t>(free_bytes / page_bytes);
  info->speculative_count = 1;
  return stats_error;
}
static kern_return_t mach_port_deallocate(host_t task, host_t host)
{ CHECK(task == 12 && host == 11); ++released; errno = EBUSY; return release_error; }
#endif

#include "cpp_platform_under_test.h"

static void test_memory()
{
  size_t available{7}, total{9};
  CHECK(x_meminfo("cpu", nullptr, nullptr).value() == EINVAL);
  CHECK(x_meminfo(nullptr, &available, &total).value() == EINVAL);
  CHECK(x_meminfo("bad", &available, &total).value() == ENOTSUP);
#if TEST_PLATFORM == 0
  CHECK(x_meminfo("cpu", &available, &total).value() == ENOTSUP);
  CHECK(available == 7 && total == 9);
  (void)query_error; (void)free_bytes; (void)total_bytes;
#else
  CHECK(!x_meminfo("cpu", &available, &total));
  CHECK(available == 4096 && total == 65536);
  available = 7; total = 9;
#if X_MACOS
  const int before_total{total_queries};
#endif
  CHECK(!x_meminfo("cpu", &available, nullptr));
  CHECK(available == 4096 && total == 9);
#if X_MACOS
  CHECK(total_queries == before_total);
  const int before_host{acquired};
#endif
  available = 7;
  CHECK(!x_meminfo("cpu", nullptr, &total));
  CHECK(available == 7 && total == 65536);
#if X_MACOS
  CHECK(acquired == before_host);
#endif
  available = 7; total = 9;
  query_error = EACCES;
  const x_error error = x_meminfo("cpu", &available, &total);
#if X_WINDOWS
  CHECK(error.value() == win32_error && strcmp(error.domain(), "win32") == 0);
#else
  CHECK(error.value() == EACCES && strcmp(error.domain(), "posix") == 0);
#endif
  CHECK(available == 7 && total == 9);
  query_error = 0;
#if X_LINUX
  memory_unit = 0;
  CHECK(x_meminfo("cpu", &available, &total).value() == ERANGE);
  CHECK(available == 7 && total == 9);
  memory_unit = 1024;
#endif
#if TEST_NARROW
  total_bytes = UINT64_C(1) << 32;
  CHECK(x_meminfo("cpu", &available, &total).value() == ERANGE);
  CHECK(available == 7 && total == 9);
  CHECK(!x_meminfo("cpu", &available, nullptr) && available == 4096);
  total_bytes = 65536;
  free_bytes = UINT64_C(1) << 32;
  available = 7; total = 9;
  CHECK(x_meminfo("cpu", &available, &total).value() == ERANGE);
  CHECK(available == 7 && total == 9);
  CHECK(!x_meminfo("cpu", nullptr, &total) && total == 65536);
  CHECK(available == 7);
  free_bytes = 4096;
#endif
#if X_MACOS
  CHECK(acquired == released);
  short_sysctl = true;
  CHECK(x_meminfo("cpu", nullptr, &total).value() == EIO);
  short_sysctl = false;
  for (int* failure : {&page_error, &stats_error, &release_error}) {
    available = 7; total = 9;
    *failure = 17;
    const x_error result = x_meminfo("cpu", &available, &total);
    CHECK(result.value() == 17 && strcmp(result.domain(), "generic") == 0);
    CHECK(available == 7 && total == 9 && acquired == released);
    *failure = 0;
  }
  for (int* failure : {&page_error, &stats_error}) {
    available = 7; total = 9;
    *failure = 17;
    release_error = 18;
    const int before_stats{stats_queries};
    const x_error result{x_meminfo("cpu", &available, &total)};
    CHECK(result.value() == 17 && strcmp(result.domain(), "generic") == 0);
    CHECK(available == 7 && total == 9 && acquired == released);
    CHECK(stats_queries == before_stats + (failure == &stats_error ? 1 : 0));
    *failure = 0;
    release_error = 0;
  }
  page_bytes = 0;
  CHECK(x_meminfo("cpu", &available, nullptr).value() == ERANGE);
  CHECK(available == 7 && acquired == released);
  page_bytes = 4096;
#endif
#endif
}

static void test_time()
{
  x_event event{"cpu"};
  x_event_agg stats;
  CHECK(event.agg(stats, 1).value() == ENODATA && x_succ(event.status()));
  CHECK(stats.cyc == 0);
#if X_WINDOWS
  counter_value = INT64_MAX - 5;
#else
  clock_seconds = 5; clock_nanoseconds = 999999999;
#endif
  CHECK(x_succ(event.tic()));
#if X_WINDOWS
  counter_value = INT64_MAX - 4;
#else
  clock_seconds = 6; clock_nanoseconds = 0;
#endif
  CHECK(x_succ(event.agg(stats, 1)));
  CHECK(stats.sum == 1.0 && stats.min.val == 1.0 && stats.max.val == 1.0);
  CHECK(x_succ(event.agg(stats, 0)));
  time_error = EIO;
  const x_error failed = event.tic();
#if X_WINDOWS
  CHECK(failed.value() == win32_error);
#else
  CHECK(failed.value() == EIO);
#endif
  CHECK(event.agg(stats, 1).value() == failed.value());
  CHECK(event.dur() == -1.0 && event.status().value() == failed.value());
  CHECK(stats.cyc == 0);
  time_error = 0;
  CHECK(x_succ(event.tic()));
#if X_WINDOWS
  counter_value = INT64_MAX - 5;
#else
  clock_seconds = 5;
#endif
  CHECK(event.agg(stats, 1).value() == ERANGE);
  CHECK(event.dur() == -1.0 && event.status().value() == ERANGE);
  CHECK(stats.cyc == 0);
#if X_WINDOWS
  frequency_error = true;
  expect_throw<std::runtime_error>([] { x_event invalid{"cpu"}; });
  frequency_error = false;
  frequency = 0;
  expect_throw<std::invalid_argument>([] { x_event invalid{"cpu"}; });
#endif
  CHECK(x_ncpu() == 7);
  cpu_count = 0;
  CHECK(x_ncpu() == 0);
#if !X_WINDOWS
  cpu_count = -1;
  CHECK(x_ncpu() == 0);
#endif
}
int main() { test_memory(); test_time(); return 0; }
