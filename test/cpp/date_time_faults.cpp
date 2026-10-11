// Real date/time sections against deterministic APIs, without an SDK or GPU.
#include <cerrno>
#include <cfloat>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <limits>
#include <stdexcept>
#include <string>
#include "sdk_stubs/cuda.h"
#include "sdk_stubs/cuda_runtime.h"
#include "support.h"

#define X_INL inline
#define X_WINDOWS TEST_WINDOWS
#define X_ENABLE_CUDA_DRIVER 1
#define X_ENABLE_CUDA_RUNTIME 1

static int32_t win32_error{87};
class x_error
{
public:
  x_error() = default;
  explicit x_error(const char* domain)
      : m_domain{domain},
        m_value{strcmp(domain, "win32") == 0 ? win32_error : errno} {}
  x_error(const char* domain, int32_t value)
      : m_domain{domain}, m_value{value} {}
  x_error& set(const char* domain, int32_t value)
  { return *this = x_error(domain, value); }
  const char* domain() const { return this->m_domain; }
  int32_t value() const { return this->m_value; }
  const char* message() const { return "injected error"; }
private:
  const char* m_domain{"posix"};
  int32_t m_value{0};
};
static x_error x_ok() { return {}; }
static bool x_fail(const x_error& error) { return error.value() != 0; }
static bool x_succ(const x_error& error) { return !x_fail(error); }

static bool fail_clock, fail_time, fail_conversion;
static int clock_calls, sleep_calls, time_calls, conversion_calls;
#if X_WINDOWS
struct LARGE_INTEGER { int64_t QuadPart; };
static int64_t frequency{10000000}, counter;
static bool fail_frequency;
static bool QueryPerformanceFrequency(LARGE_INTEGER* value)
{ value->QuadPart = frequency; return !fail_frequency; }
static bool QueryPerformanceCounter(LARGE_INTEGER* value)
{
  ++clock_calls;
  value->QuadPart = counter;
  return !fail_clock;
}
static void Sleep(unsigned long millisecond)
{
  CHECK(millisecond == 1234 || millisecond == ULONG_MAX);
  ++sleep_calls;
}
#else
static timespec clock_value{};
static bool fail_sleep;
#undef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 123
static int test_clock_gettime(int clock, timespec* value)
{
  CHECK(clock == CLOCK_MONOTONIC);
  ++clock_calls;
  *value = clock_value;
  if (fail_clock) { errno = EIO; return -1; }
  return 0;
}
static int test_nanosleep(const timespec* request, timespec* remaining)
{
  ++sleep_calls;
  if (fail_sleep) {
    CHECK(request->tv_sec == 1 && request->tv_nsec == 234000000);
    errno = EINVAL;
    return -1;
  }
  if (sleep_calls == 1) {
    CHECK(request->tv_sec == 1 && request->tv_nsec == 234000000);
    *remaining = {0, 500000000};
    errno = EINTR;
    return -1;
  }
  CHECK(request->tv_sec == 0 && request->tv_nsec == 500000000);
  return 0;
}
#define clock_gettime test_clock_gettime
#define nanosleep test_nanosleep
#endif

static time_t test_time(time_t* ignored)
{
  CHECK(ignored == nullptr);
  ++time_calls;
  return fail_time ? static_cast<time_t>(-1) : static_cast<time_t>(123);
}
#if X_WINDOWS
static int test_ctime_s(char* buffer, size_t size, const time_t* now)
{
  CHECK(size >= 26 && *now == 123);
  ++conversion_calls;
  memcpy(buffer, "Sat Oct  3 12:00:00 2026\n", 26);
  return fail_conversion ? EINVAL : 0;
}
#define ctime_s test_ctime_s
#else
static char* test_ctime_r(const time_t* now, char* buffer)
{
  CHECK(*now == 123);
  ++conversion_calls;
  memcpy(buffer, "Sat Oct  3 12:00:00 2026\n", 26);
  return fail_conversion ? nullptr : buffer;
}
#define ctime_r test_ctime_r
#endif
static FILE* test_stdout = stdout;
#undef stdout
#define stdout test_stdout

#include "cpp_date_time_under_test.h"
#include "date_time_support.h"

static void set_clock(bool stop)
{
#if X_WINDOWS
  counter = INT64_C(1000000000000000000) + (stop ? 1 : 0);
#else
  clock_value = stop ? timespec{43, 500} : timespec{42, 999999500};
#endif
}

static void test_cpu()
{
#if X_WINDOWS
  fail_frequency = true;
  expect_event_error([] { x_event event{"cpu"}; }, "win32", win32_error);
  fail_frequency = false;
  frequency = 0;
  expect_throw<std::invalid_argument>([] { x_event event{"cpu"}; });
  frequency = 10000000;
  constexpr double expected{100.0};
  const char* domain{"win32"};
  const int32_t value{win32_error};
#else
  constexpr double expected{1000.0};
  const char* domain{"posix"};
  constexpr int32_t value{EIO};
#endif
  x_event event{"cpu"};
  x_event_agg stats;
  set_clock(false);
  CHECK(x_succ(event.tic()));
  set_clock(true);
  CHECK(x_succ(event.agg(stats, 2)));
  CHECK(stats.sum == expected && stats.cyc == 1 && !stats.ready);
  CHECK(stats.min.val == expected && stats.max.val == expected);
  CHECK(event.dur() == expected && x_succ(event.status()));
  CHECK(event.dur("bad") == -1.0 && x_succ(event.status()));
  CHECK(event.dur() == expected);

  fail_clock = true;
  check_event_status(event.tic(), domain, value);
  const x_event_agg snapshot = stats;
  const int calls = clock_calls;
  CHECK(event.dur("bad") == -1.0);
  check_event_status(event.status(), domain, value);
  check_event_status(event.agg(stats, 2), domain, value);
  check_duration_error(event, domain, value);
  CHECK(clock_calls == calls);
  check_stats(stats, snapshot);

  stats.ready = true;
  const x_event_agg ready = stats;
  CHECK(x_succ(event.agg(stats, 2)));
  check_stats(stats, ready);
  check_event_status(event.status(), domain, value);
  CHECK(x_succ(event.agg(stats, 0)));
  check_reset(stats);
  check_event_status(event.status(), domain, value);
  stats.cyc = SIZE_MAX;
  const x_event_agg overflow = stats;
  check_event_status(event.agg(stats, 1), "posix", EOVERFLOW);
  check_stats(stats, overflow);
  check_event_status(event.status(), domain, value);
  check_event_status(event.toc(), domain, value);
  CHECK(clock_calls == calls);

  fail_clock = false;
  stats.reset();
  set_clock(false);
  CHECK(x_succ(event.tic()) && x_succ(event.status()));
  fail_clock = true;
  const x_event_agg empty = stats;
  check_event_status(event.agg(stats, 1), domain, value);
  check_stats(stats, empty);
  check_duration_error(event, domain, value);
  fail_clock = false;
  set_clock(true);
  CHECK(x_succ(event.agg(stats, 1)));
  CHECK(stats.sum == expected && stats.ready && x_succ(event.status()));

  CHECK(x_succ(event.agg(stats, 0)));
  set_clock(true);
  CHECK(x_succ(event.tic()));
  set_clock(false);
  check_event_status(event.agg(stats, 1), "posix", ERANGE);
  check_stats(stats, empty);
  check_duration_error(event, "posix", ERANGE);
  set_clock(true);
  CHECK(x_succ(event.agg(stats, 1)));
  CHECK(stats.sum == 0.0 && stats.ready && stats.min.val == 0.0);
  CHECK(x_succ(event.status()));

#if X_WINDOWS
  // Exercise a difference that would overflow signed integer subtraction.
  counter = INT64_MIN;
  CHECK(x_succ(event.tic()));
  counter = INT64_MAX;
  CHECK(x_succ(event.toc()));
  CHECK(std::isfinite(event.dur()) && event.dur() > 0.0);
#else
  // POSIX mocks still use the host CRT's difftime; keep seconds in its range.
  clock_value = {2000000000, 999999999};
  CHECK(x_succ(event.tic()));
  clock_value = {2000000001, 0};
  CHECK(x_succ(event.toc()));
  CHECK(event.dur() == 1.0);
#endif
}

static void test_cuda(const char* type, test_backend& backend)
{
  backend = {};
  {
    x_event event{type};
    x_event_agg stats;
    const x_event_agg empty = stats;
    check_event_status(event.agg(stats, 2), "posix", ENODATA);
    CHECK(x_succ(event.status()));
    check_duration_error(event, "posix", ENODATA);
    CHECK(backend.records == 0 && backend.synchronizes == 0);
    void* stream = reinterpret_cast<void*>(std::uintptr_t{16});
    check_event_status(event.tic(stream, 1), "posix", 0);
    check_event_status(event.status(), "posix", 0);
    const void* start = backend.last_record_event;
    CHECK(backend.last_stream == stream && backend.last_flags == 1);
    backend.record_error = 3;
    check_event_status(event.agg(stats, 2, stream), type, 3);
    const void* stop = backend.last_record_event;
    CHECK(start != nullptr && stop != nullptr && start != stop);
    CHECK(backend.last_stream == stream && backend.last_flags == 0);
    check_stats(stats, empty);
    check_duration_error(event, type, 3);
    CHECK(event.dur("bad") == -1.0);
    check_event_status(event.status(), type, 3);
    CHECK(backend.synchronizes == 0 && backend.elapsed_calls == 0);

    backend.record_error = 0;
    check_event_status(event.agg(stats, 2, stream, 1), "posix", 0);
    CHECK(backend.last_record_event == stop);
    CHECK(backend.last_sync_event == stop);
    CHECK(backend.last_elapsed_start == start);
    CHECK(backend.last_elapsed_stop == stop);
    CHECK(stats.cyc == 1 && stats.sum == 2e6 && x_succ(event.status()));
    const x_event_agg snapshot = stats;
    const int elapsed = backend.elapsed_calls;
    backend.sync_error = 4;
    check_event_status(event.agg(stats, 2), type, 4);
    CHECK(backend.elapsed_calls == elapsed);
    check_stats(stats, snapshot);
    CHECK(event.dur("bad") == -1.0);
    check_event_status(event.status(), type, 4);
    check_duration_error(event, type, 4);
    CHECK(backend.elapsed_calls == elapsed);
    backend.sync_error = 0;
    backend.elapsed_error = 5;
    check_event_status(event.agg(stats, 2), type, 5);
    check_stats(stats, snapshot);
    check_duration_error(event, type, 5);
    backend.elapsed_error = 0;

    const float invalid[]{
      (std::numeric_limits<float>::quiet_NaN)(), -1.0f,
      (std::numeric_limits<float>::infinity)(),
      -(std::numeric_limits<float>::infinity)()
    };
    for (size_t idx{0}; idx < sizeof(invalid) / sizeof(invalid[0]); ++idx) {
      backend.milliseconds = invalid[idx];
      const int32_t value = idx == 0 ? EDOM : ERANGE;
      check_event_status(event.agg(stats, 2), "posix", value);
      check_stats(stats, snapshot);
      check_duration_error(event, "posix", value);
    }
    backend.milliseconds = 1.0f;
    CHECK(x_succ(event.agg(stats, 2)));
    CHECK(stats.ready && stats.cyc == 2 && stats.sum == 3e6);
    CHECK(x_succ(event.status()));
    CHECK(stats.min.idx == 1 && stats.max.idx == 0);
    const x_event_agg ready = stats;
    backend.record_error = 6;
    check_event_status(event.tic(), type, 6);
    const int records = backend.records;
    const int synchronizes = backend.synchronizes;
    CHECK(x_succ(event.agg(stats, 2)));
    check_stats(stats, ready);
    check_event_status(event.status(), type, 6);
    CHECK(x_succ(event.agg(stats, 0)));
    check_reset(stats);
    check_event_status(event.status(), type, 6);
    stats.cyc = SIZE_MAX;
    const x_event_agg overflow = stats;
    check_event_status(event.agg(stats, 1), "posix", EOVERFLOW);
    check_stats(stats, overflow);
    check_event_status(event.status(), type, 6);
    check_duration_error(event, type, 6);
    check_event_status(event.toc(), type, 6);
    CHECK(backend.records == records && backend.synchronizes == synchronizes);
    backend.record_error = 0;
    CHECK(x_succ(event.tic()) && x_succ(event.status()));
    check_event_status(event.toc(), "posix", 0);
    CHECK(event.dur("ms") == 1.0 && x_succ(event.status()));
  }
  CHECK(backend.live == 0 && backend.destroys == 2);
}

static void test_timestamp_and_sleep()
{
  char buffer[26] = "old";
  CHECK(x_timestamp(buffer, 25) == buffer && buffer[0] == '\0');
  CHECK(strcmp(x_timestamp(nullptr, 26), "") == 0);
  buffer[0] = 'x';
  CHECK(strcmp(x_timestamp(buffer, 0), "") == 0 && buffer[0] == 'x');
  CHECK(time_calls == 0 && conversion_calls == 0);
  fail_time = true;
  CHECK(x_timestamp(buffer, 26) == buffer && buffer[0] == '\0');
  CHECK(time_calls == 1 && conversion_calls == 0);
  fail_time = false;
  fail_conversion = true;
  CHECK(x_timestamp(buffer, 26) == buffer && buffer[0] == '\0');
  fail_conversion = false;
  CHECK(x_timestamp(buffer, 26) == buffer);
  CHECK(strcmp(buffer, "Sat Oct  3 12:00:00 2026") == 0);
  CHECK(time_calls == 3 && conversion_calls == 2);
  x_sleep(1234);
#if X_WINDOWS
  CHECK(sleep_calls == 1);
  x_sleep(ULONG_MAX);
  CHECK(sleep_calls == 2);
#else
  CHECK(sleep_calls == 2);
  fail_sleep = true;
  sleep_calls = 0;
  x_sleep(1234);
  CHECK(sleep_calls == 1);
#endif
}

static void test_echo_defaults()
{
  test_stdout = tmpfile();
  CHECK(test_stdout != nullptr);
  x_event_agg stats;
  const x_event_agg empty = stats;
  stats.echo(nullptr);
  stats.echo("", "", nullptr);
  check_stats(stats, empty);
  CHECK(read_stream(test_stdout)
      == "[Sat Oct  3 12:00:00 2026] no samples (0 cycles)\n"
         "[Sat Oct  3 12:00:00 2026] no samples (0 cycles)\n");
  CHECK(fclose(test_stdout) == 0);

  test_stdout = tmpfile();
  CHECK(test_stdout != nullptr);
  stats.cyc = 1;
  stats.sum = stats.avg = 1.0;
  stats.min = {0, 1.0};
  stats.max = {0, 1.0};
  const x_event_agg snapshot = stats;
  stats.echo("unknown", "TITLE", nullptr);
  check_stats(stats, snapshot);
  CHECK(read_stream(test_stdout)
      == "[TITLE] 1.000000ns in 1 cycles - avg: 1.000000ns, "
         "min(0): 1.000000ns, max(0): 1.000000ns\n");
  CHECK(fclose(test_stdout) == 0);
}

int main()
{
  test_cpu();
  test_cuda("cu", driver);
  test_cuda("cuda", runtime);
  test_timestamp_and_sleep();
  test_echo_defaults();
  return 0;
}
