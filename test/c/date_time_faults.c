// Test the production sections with synthetic clocks and CUDA APIs. These are
// deliberately not substitutes for an SDK build or execution on real hardware.
// Expose POSIX time types when this fixture is compiled as strict C99.
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <errno.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define X_INL static inline
#define X_WINDOWS TEST_WINDOWS
#define X_ENABLE_CUDA_DRIVER 1
#define X_ENABLE_CUDA_RUNTIME 1
#define CHECK(condition) do { \
  if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    exit(1); \
  } \
} while (0)

typedef struct {
  const char* domain;
  int32_t value;
} x_error;

X_INL x_error x_ok(void)
{
  x_error err = {"posix", 0};
  return err;
}

X_INL bool x_fail(x_error error)
{
  return error.value != 0;
}

X_INL bool x_succ(x_error error)
{
  return !x_fail(error);
}

X_INL x_error test_error(const char* domain, int32_t value)
{
  x_error err = {domain, value};
  return err;
}

#if X_WINDOWS
static unsigned long win32_error;

X_INL unsigned long GetLastError(void)
{
  return win32_error;
}
#endif

X_INL x_error test_error_capture(const char* domain)
{
#if X_WINDOWS
  if (strcmp(domain, "win32") == 0) {
    return test_error(domain, (int32_t)GetLastError());
  }
#endif

  return test_error(domain, errno);
}

#define TEST_ERROR_CAPTURE(domain) test_error_capture(domain)
#define TEST_ERROR_VALUE(domain, value) test_error(domain, value)
#define TEST_ERROR_SELECT(_1, _2, NAME, ...) NAME
#define x_error_set(...) \
  TEST_ERROR_SELECT(__VA_ARGS__, TEST_ERROR_VALUE, TEST_ERROR_CAPTURE)(__VA_ARGS__)

static int fail_clock;
static int fail_time;
static int fail_conversion;
static int clock_calls;
static int conversion_calls;
static int sleep_calls;
enum { TEST_FREQUENCY_ERROR = 50, TEST_COUNTER_ERROR = 87 };

#if X_WINDOWS
static int fail_frequency;
static int64_t counter_value;
static int64_t counter_frequency = 10000000;
typedef struct {
  int64_t QuadPart;
} LARGE_INTEGER;

X_INL int QueryPerformanceFrequency(LARGE_INTEGER* output)
{
  if (fail_frequency) {
    win32_error = TEST_FREQUENCY_ERROR;
    return 0;
  }

  output->QuadPart = counter_frequency;
  return 1;
}
X_INL int QueryPerformanceCounter(LARGE_INTEGER* output)
{
  ++clock_calls;
  if (fail_clock) {
    win32_error = TEST_COUNTER_ERROR;
    return 0;
  }

  output->QuadPart = counter_value;
  return 1;
}
X_INL void Sleep(unsigned long interval)
{
  CHECK(interval == 1234);
  ++sleep_calls;
}
#else
static struct timespec posix_value;

// Use a recognizable test-only clock identifier to catch accidental realtime.
#ifdef CLOCK_MONOTONIC
#undef CLOCK_MONOTONIC
#endif
#define CLOCK_MONOTONIC 123
X_INL int test_clock_gettime(int clock_id, struct timespec* output)
{
  CHECK(clock_id == CLOCK_MONOTONIC);
  ++clock_calls;
  if (fail_clock) {
    errno = EIO;
    return -1;
  }
  *output = posix_value;
  return 0;
}
X_INL int test_nanosleep(const struct timespec* request, struct timespec* remaining)
{
  ++sleep_calls;
  if (sleep_calls == 1) {
    CHECK(request->tv_sec == 1 && request->tv_nsec == 234000000);
    remaining->tv_sec = 0;
    remaining->tv_nsec = 500000000;
    errno = EINTR;
    return -1;
  }
  CHECK(request->tv_sec == 0 && request->tv_nsec == 500000000);
  return 0;
}
#define clock_gettime test_clock_gettime
#define nanosleep test_nanosleep
#endif

X_INL time_t test_time(time_t* ignored)
{
  (void)ignored;
  return fail_time ? (time_t)-1 : (time_t)123;
}
X_INL int test_time_conversion(char* output, size_t size, const time_t* now)
{
  ++conversion_calls;
  CHECK(size >= 26 && *now == 123);
  if (fail_conversion) {
    return EINVAL;
  }
  memcpy(output, "Sat Oct  3 12:00:00 2026\n", 26);
  return 0;
}
#if X_WINDOWS
X_INL int test_ctime_s(char* output, size_t size, const time_t* now)
{
  return test_time_conversion(output, size, now);
}
#define ctime_s test_ctime_s
#else
X_INL char* test_ctime_r(const time_t* now, char* output)
{
  return test_time_conversion(output, 26, now) == 0 ? output : NULL;
}
#define ctime_r test_ctime_r
#endif
#define time test_time
#define timespec_get forbidden_wall_clock

// Synthetic status values used by the injected failures and missing-record
// condition; they do not attempt to reproduce the SDK's numeric error values.
enum {
  FAIL_CREATE = 17, FAIL_RECORD, FAIL_SYNC, FAIL_ELAPSED, FAIL_DESTROY,
  FAIL_UNRECORDED
};
typedef int CUresult;
typedef int cudaError_t;
typedef struct test_event {
  bool live;
  size_t record_count;
} *CUevent, *cudaEvent_t;
typedef void* CUstream;
typedef void* cudaStream_t;
#define CUDA_SUCCESS 0
#define cudaSuccess 0
#define CUDA_ERROR_INVALID_VALUE 1
#define cudaErrorInvalidValue 1
#define CU_EVENT_DISABLE_TIMING 2
#define cudaEventDisableTiming 2

typedef struct {
  int create_calls, destroy_calls, record_calls, sync_calls, elapsed_calls, live;
  int fail_create_on, fail_destroy_on, record_error, sync_error, elapsed_error;
  bool fail_all_destroys;
  unsigned int last_flags;
  void* last_stream;
  CUevent last_record_event, last_sync_event;
  CUevent last_elapsed_start, last_elapsed_stop;
  float milliseconds;
  struct test_event events[8];
} backend;
static backend driver, runtime;

X_INL int create_event(backend* api, CUevent* event, unsigned int flags)
{
  ++api->create_calls;
  api->last_flags = flags;
  if (api->create_calls == api->fail_create_on) {
    return FAIL_CREATE;
  }
  CHECK(api->create_calls <= 8);
  *event = &api->events[api->create_calls - 1];
  (*event)->live = true;
  (*event)->record_count = 0;
  ++api->live;
  return 0;
}
X_INL int destroy_event(backend* api, CUevent event)
{
  CHECK(event != NULL && event->live);
  ++api->destroy_calls;
  if (api->fail_all_destroys || api->destroy_calls == api->fail_destroy_on) {
    return FAIL_DESTROY;
  }
  event->live = false;
  --api->live;
  return 0;
}
X_INL int record_event(backend* api, CUevent event, void* stream, unsigned int flags)
{
  CHECK(event != NULL && event->live);
  ++api->record_calls;
  api->last_record_event = event;
  api->last_stream = stream;
  api->last_flags = flags;
  if (api->record_error) {
    // Inject a rejection before any new work is captured.
    return api->record_error;
  }

  ++event->record_count;
  return 0;
}
X_INL int sync_event(backend* api, CUevent event)
{
  CHECK(event != NULL && event->live);
  ++api->sync_calls;
  api->last_sync_event = event;
  // A never-recorded event represents an empty set of work. Synchronization
  // can succeed, although that event cannot participate in elapsed timing.
  return api->sync_error;
}
X_INL int elapsed_time(backend* api, float* ms, CUevent start, CUevent stop)
{
  CHECK(start != NULL && start->live && stop != NULL && stop->live);
  ++api->elapsed_calls;
  api->last_elapsed_start = start;
  api->last_elapsed_stop = stop;
  if (api->elapsed_error) {
    return api->elapsed_error;
  }

  if (start->record_count == 0 || stop->record_count == 0) {
    return FAIL_UNRECORDED;
  }

  *ms = api->milliseconds;
  return 0;
}
X_INL CUresult cuEventCreate(CUevent* e, unsigned int f)
{
  return create_event(&driver, e, f);
}

X_INL CUresult cuEventDestroy_v2(CUevent e)
{
  return destroy_event(&driver, e);
}

X_INL CUresult cuEventRecordWithFlags(CUevent e, CUstream s, unsigned int f)
{
  return record_event(&driver, e, s, f);
}

X_INL CUresult cuEventSynchronize(CUevent e)
{
  return sync_event(&driver, e);
}

X_INL CUresult cuEventElapsedTime(float* ms, CUevent a, CUevent b)
{
  return elapsed_time(&driver, ms, a, b);
}

X_INL cudaError_t cudaEventCreateWithFlags(cudaEvent_t* e, unsigned int f)
{
  return create_event(&runtime, e, f);
}

X_INL cudaError_t cudaEventDestroy(cudaEvent_t e)
{
  return destroy_event(&runtime, e);
}

X_INL cudaError_t cudaEventRecordWithFlags(cudaEvent_t e, cudaStream_t s, unsigned int f)
{
  return record_event(&runtime, e, s, f);
}

X_INL cudaError_t cudaEventSynchronize(cudaEvent_t e)
{
  return sync_event(&runtime, e);
}

X_INL cudaError_t cudaEventElapsedTime(float* ms, cudaEvent_t a, cudaEvent_t b)
{
  return elapsed_time(&runtime, ms, a, b);
}

#include "date_time_under_test.h"

static void check_error(x_error error, const char* domain, int value)
{
  CHECK(error.value == value && strcmp(error.domain, domain) == 0);
}
static void reset_backend(backend* api)
{
  CHECK(api->live == 0);
  memset(api, 0, sizeof(*api));
  api->milliseconds = 2.5f;
}
static void check_same_aggregation(const x_event_agg* a, const x_event_agg* b)
{
  CHECK(a->ready == b->ready && a->cyc == b->cyc);
  CHECK(a->sum == b->sum && a->avg == b->avg);
  CHECK(a->min.idx == b->min.idx && a->min.val == b->min.val);
  CHECK(a->max.idx == b->max.idx && a->max.val == b->max.val);
  CHECK(a->echo == b->echo && a->reset == b->reset);
}

static void check_request_errors(x_event* event)
{
  x_error err = event->status;
  const bool started = event->started;
  const bool stopped = event->stopped;
  x_event_agg aggregation;
  x_event_agg_init(&aggregation);
  x_event_agg snapshot = aggregation;

  check_error(event->agg(event, NULL, 1, NULL, 0), "posix", EINVAL);
  check_error(event->tic(NULL, NULL, 0), "posix", EINVAL);
  check_error(event->toc(NULL, NULL, 0), "posix", EINVAL);
  check_error(event->agg(NULL, &aggregation, 1, NULL, 0), "posix", EINVAL);
  CHECK(event->dur(event, "bogus") == -1.0);
  CHECK(event->dur(NULL, "ns") == -1.0);
  check_error(event->status, err.domain, err.value);
  CHECK(event->started == started && event->stopped == stopped);

  int32_t type = event->type;
  event->type = 99;
  check_error(event->tic(event, NULL, 0), "posix", EINVAL);
  check_error(event->toc(event, NULL, 0), "posix", EINVAL);
  check_error(event->agg(event, &aggregation, 1, NULL, 0), "posix", EINVAL);
  CHECK(event->dur(event, "ns") == -1.0);
  check_error(event->status, err.domain, err.value);
  CHECK(event->started == started && event->stopped == stopped);
  check_same_aggregation(&aggregation, &snapshot);
  event->type = type;
}

static void check_missing_samples(x_event* event)
{
  CHECK(x_succ(event->status));
  check_error(event->agg(event, NULL, 1, NULL, 0), "posix", EINVAL);
  CHECK(x_succ(event->status));
  CHECK(event->dur(event, "ns") == -1.0);
  check_error(event->status, "posix", ENODATA);
  CHECK(event->dur(event, "bogus") == -1.0);
  check_error(event->status, "posix", ENODATA);
  check_error(event->toc(event, NULL, 0), "posix", ENODATA);
  check_error(event->status, "posix", ENODATA);

  const char* units[] = {"ns", "us", "ms", "s", "m", "h", NULL, ""};
  for (size_t i = 0; i < sizeof(units) / sizeof(units[0]); ++i) {
    CHECK(event->dur(event, units[i]) == -1.0);
    check_error(event->status, "posix", ENODATA);
  }
  check_error(event->toc(event, NULL, 0), "posix", ENODATA);

  x_event_agg aggregation;
  x_event_agg_init(&aggregation);
  x_event_agg snapshot = aggregation;
  check_error(event->agg(event, &aggregation, 1, NULL, 0), "posix", ENODATA);
  check_same_aggregation(&aggregation, &snapshot);

  // An invalid event is not merely an event missing measurement data.
  check_request_errors(event);
  check_same_aggregation(&aggregation, &snapshot);
}

static void test_cpu(void)
{
  x_event event;
#if X_WINDOWS
  fail_frequency = 1;
  check_error(x_event_init(&event, "cpu", 0), "win32", TEST_FREQUENCY_ERROR);
  check_error(event.status, "win32", TEST_FREQUENCY_ERROR);
  CHECK(event.type == -1 && x_succ(x_event_term(&event)));
  fail_frequency = 0;
  counter_frequency = 0;
  check_error(x_event_init(&event, "cpu", 0), "posix", EINVAL);
  CHECK(event.type == -1);
  check_error(event.status, "posix", EINVAL);
  counter_frequency = 10000000;
#endif
  CHECK(x_succ(x_event_init(&event, "cpu", 0)));
  check_missing_samples(&event);
#if X_WINDOWS
  // Large absolute counts ensure conversion happens after subtraction.
  counter_value = INT64_C(1000000000000000000);
#else
  posix_value.tv_sec = 42;
  posix_value.tv_nsec = 999999500;
#endif
  CHECK(x_succ(event.tic(&event, NULL, 0)));
  CHECK(event.dur(&event, "ns") == -1.0);
  check_error(event.status, "posix", ENODATA);
#if X_WINDOWS
  counter_value += 1;
#else
  posix_value.tv_sec = 43;
  posix_value.tv_nsec = 500;
#endif
  CHECK(x_succ(event.toc(&event, NULL, 0)));
  CHECK(event.dur(&event, "ns") == (X_WINDOWS ? 100.0 : 1000.0));
  double output = -123.0;
  const x_event* samples = &event;
  CHECK(x_succ(_x_event_dur_cpu(samples, &output)));
  CHECK(output == (X_WINDOWS ? 100.0 : 1000.0));
  check_request_errors(&event);

#if X_WINDOWS
  int64_t frequency = event.frequency;
  event.frequency = 0;
  output = -123.0;
  check_error(_x_event_dur_cpu(samples, &output), "posix", EINVAL);
  CHECK(output == -123.0);
  CHECK(event.dur(&event, "ns") == -1.0);
  check_error(event.status, "posix", EINVAL);
  event.frequency = frequency;
#endif

  fail_clock = 1;
  CHECK(x_fail(event.tic(&event, NULL, 0)));
  CHECK(!event.started && !event.stopped);
  check_error(event.status,
      X_WINDOWS ? "win32" : "posix", X_WINDOWS ? TEST_COUNTER_ERROR : EIO);
  check_request_errors(&event);
  check_error(event.agg(&event, NULL, 1, NULL, 0), "posix", EINVAL);
  CHECK(event.dur(&event, "bogus") == -1.0);
  check_error(event.status,
      X_WINDOWS ? "win32" : "posix", X_WINDOWS ? TEST_COUNTER_ERROR : EIO);
  x_event_agg unrelated;
  CHECK(x_succ(event.agg(&event, &unrelated, 0, NULL, 0)));
  check_error(event.status,
      X_WINDOWS ? "win32" : "posix", X_WINDOWS ? TEST_COUNTER_ERROR : EIO);
  CHECK(event.dur(&event, "h") == -1.0);
  check_error(event.status,
      X_WINDOWS ? "win32" : "posix", X_WINDOWS ? TEST_COUNTER_ERROR : EIO);
  check_error(event.toc(&event, NULL, 0),
      X_WINDOWS ? "win32" : "posix", X_WINDOWS ? TEST_COUNTER_ERROR : EIO);
  fail_clock = 0;
  CHECK(x_succ(event.tic(&event, NULL, 0)));
  CHECK(x_succ(event.status));
  x_event_agg aggregation;
  x_event_agg_init(&aggregation);
  x_event_agg snapshot = aggregation;
  fail_clock = 1;
  CHECK(x_fail(event.agg(&event, &aggregation, 1, NULL, 0)));
  check_error(event.status,
      X_WINDOWS ? "win32" : "posix", X_WINDOWS ? TEST_COUNTER_ERROR : EIO);
  check_same_aggregation(&aggregation, &snapshot);
  CHECK(event.started && !event.stopped);
  check_request_errors(&event);
  check_error(event.agg(&event, NULL, 1, NULL, 0), "posix", EINVAL);
  CHECK(event.dur(&event, "ns") == -1.0);
  check_error(event.status,
      X_WINDOWS ? "win32" : "posix", X_WINDOWS ? TEST_COUNTER_ERROR : EIO);
  fail_clock = 0;
  CHECK(x_succ(event.agg(&event, &aggregation, 1, NULL, 0)));
  CHECK(aggregation.ready && aggregation.cyc == 1);

  aggregation.reset(&aggregation);
  CHECK(x_succ(event.tic(&event, NULL, 0)));
#if X_WINDOWS
  --counter_value;
#else
  --posix_value.tv_nsec;
#endif
  snapshot = aggregation;
  check_error(event.agg(&event, &aggregation, 1, NULL, 0), "posix", ERANGE);
  check_same_aggregation(&aggregation, &snapshot);
  output = -123.0;
  check_error(_x_event_dur_cpu(samples, &output), "posix", ERANGE);
  CHECK(output == -123.0);
  aggregation.cyc = SIZE_MAX;
  snapshot = aggregation;
  int calls = clock_calls;
  check_error(event.agg(&event, &aggregation, 1, NULL, 0), "posix", EOVERFLOW);
  check_error(event.status, "posix", ERANGE);
  CHECK(clock_calls == calls);
  check_same_aggregation(&aggregation, &snapshot);
  CHECK(x_succ(x_event_term(&event)));
  CHECK(event.dur(&event, "ns") == -1.0);
  CHECK(x_succ(event.status));
  check_error(event.toc(&event, NULL, 0), "posix", EINVAL);
  CHECK(x_succ(event.status));
}

static void test_cuda_recording(backend* api, const char* domain)
{
  reset_backend(api);
  x_event event;
  CHECK(x_succ(x_event_init(&event, domain, 0)));

  CUevent start;
  CUevent stop;
  if (strcmp(domain, "cu") == 0) {
    start = event.start.cu;
    stop = event.stop.cu;
  } else {
    start = event.start.cuda;
    stop = event.stop.cuda;
  }
  CHECK(start != stop && start->record_count == 0 && stop->record_count == 0);

  x_error (*duration)(const x_event*, double*) =
      strcmp(domain, "cu") == 0 ? _x_event_dur_cuda_driver : _x_event_dur_cuda_runtime;
  double output = -123.0;
  CHECK(sync_event(api, stop) == 0);
  check_error(duration(&event, &output), domain, FAIL_UNRECORDED);
  CHECK(output == -123.0);
  CHECK(api->last_sync_event == stop);
  CHECK(api->last_elapsed_start == start && api->last_elapsed_stop == stop);

  int stream;
  CHECK(x_succ(event.tic(&event, &stream, 1)));
  CHECK(api->last_record_event == start);
  CHECK(api->last_stream == &stream && api->last_flags == 1);
  CHECK(start->record_count == 1 && stop->record_count == 0);
  check_error(duration(&event, &output), domain, FAIL_UNRECORDED);
  CHECK(output == -123.0);
  CHECK(api->last_sync_event == stop);
  CHECK(api->last_elapsed_start == start && api->last_elapsed_stop == stop);

  // Reversing the pair exercises the missing-start case in the SDK model.
  float milliseconds = -123.0f;
  CHECK(elapsed_time(api, &milliseconds, stop, start) == FAIL_UNRECORDED);
  CHECK(milliseconds == -123.0f);

  api->record_error = FAIL_RECORD;
  check_error(event.toc(&event, &stream, 0), domain, FAIL_RECORD);
  CHECK(api->last_record_event == stop);
  CHECK(api->last_stream == &stream && api->last_flags == 0);
  CHECK(start->record_count == 1 && stop->record_count == 0);
  CHECK(!event.stopped);
  int calls = api->elapsed_calls;
  CHECK(event.dur(&event, "ms") == -1.0);
  check_error(event.status, domain, FAIL_RECORD);
  CHECK(api->elapsed_calls == calls);
  check_error(duration(&event, &output), domain, FAIL_UNRECORDED);
  CHECK(output == -123.0);

  api->record_error = 0;
  CHECK(x_succ(event.toc(&event, &stream, 0)));
  CHECK(api->last_record_event == stop);
  CHECK(api->last_stream == &stream && api->last_flags == 0);
  CHECK(start->record_count == 1 && stop->record_count == 1);
  CHECK(event.dur(&event, "ms") == 2.5 && x_succ(event.status));
  CHECK(api->last_sync_event == stop);
  CHECK(api->last_elapsed_start == start && api->last_elapsed_stop == stop);

  // An injected rejection must not invent a new capture or erase the old one.
  api->record_error = FAIL_RECORD;
  check_error(event.tic(&event, &stream, 1), domain, FAIL_RECORD);
  CHECK(api->last_record_event == start);
  CHECK(api->last_stream == &stream && api->last_flags == 1);
  CHECK(start->record_count == 1 && stop->record_count == 1);
  calls = api->elapsed_calls;
  CHECK(event.dur(&event, "ms") == -1.0);
  check_error(event.status, domain, FAIL_RECORD);
  CHECK(api->elapsed_calls == calls);
  CHECK(x_succ(x_event_term(&event)) && api->live == 0);
}

static void test_cuda(backend* api, const char* domain)
{
  x_event event;
  reset_backend(api);
  check_error(x_event_init(&event, domain, 2), domain, 1);
  CHECK(api->create_calls == 0 && event.type == -1);
  CHECK(x_succ(x_event_term(&event)));

  api->fail_create_on = 1;
  check_error(x_event_init(&event, domain, 0), domain, FAIL_CREATE);
  CHECK(api->live == 0 && event.type == -1);
  CHECK(x_succ(x_event_term(&event)));

  reset_backend(api);
  api->fail_create_on = 2;
  check_error(x_event_init(&event, domain, 0), domain, FAIL_CREATE);
  CHECK(api->live == 0 && api->destroy_calls == 1 && event.type == -1);
  check_error(event.status, domain, FAIL_CREATE);

  reset_backend(api);
  api->fail_create_on = 2;
  api->fail_destroy_on = 1;
  check_error(x_event_init(&event, domain, 0), domain, FAIL_CREATE);
  CHECK(api->live == 1 && event.type != -1);
  check_error(event.status, domain, FAIL_CREATE);
  CHECK(event.type == (strcmp(domain, "cu") == 0 ? 1 : 2));
  CHECK(x_succ(x_event_term(&event)) && api->live == 0 && event.type == -1);

  reset_backend(api);
  CHECK(x_succ(x_event_init(&event, domain, 1)));
  CHECK(api->live == 2 && api->last_flags == 1);
  check_missing_samples(&event);
  CHECK(api->record_calls == 0 && api->sync_calls == 0);
  int stream;
  CHECK(x_succ(event.tic(&event, &stream, 1)));
  CHECK(api->last_stream == &stream && api->last_flags == 1);
  CHECK(x_succ(event.toc(&event, &stream, 0)));
  CHECK(event.dur(&event, "ms") == 2.5 && x_succ(event.status));
  check_request_errors(&event);
  x_error (*duration)(const x_event*, double*) =
      strcmp(domain, "cu") == 0 ? _x_event_dur_cuda_driver : _x_event_dur_cuda_runtime;
  const x_event* samples = &event;
  double output = -123.0;
  CHECK(x_succ(duration(samples, &output)) && output == 2500000.0);

  const char* units[] = {"ns", "us", "ms", "s", "m", "h", NULL, ""};
  api->sync_error = FAIL_SYNC;
  for (size_t i = 0; i < sizeof(units) / sizeof(units[0]); ++i) {
    CHECK(event.dur(&event, units[i]) == -1.0);
    check_error(event.status, domain, FAIL_SYNC);
  }
  check_request_errors(&event);
  output = -123.0;
  check_error(duration(samples, &output), domain, FAIL_SYNC);
  CHECK(output == -123.0);
  api->sync_error = 0;
  api->elapsed_error = FAIL_ELAPSED;
  CHECK(event.dur(&event, "ms") == -1.0);
  check_error(event.status, domain, FAIL_ELAPSED);
  check_error(duration(samples, &output), domain, FAIL_ELAPSED);
  CHECK(output == -123.0);

  x_event_agg aggregation;
  x_event_agg_init(&aggregation);
  CHECK(x_succ(event.tic(&event, NULL, 0)));
  x_event_agg snapshot = aggregation;
  check_error(event.agg(&event, &aggregation, 2, NULL, 0), domain, FAIL_ELAPSED);
  check_same_aggregation(&aggregation, &snapshot);
  api->elapsed_error = 0;
  CHECK(x_succ(event.agg(&event, &aggregation, 2, NULL, 0)));
  CHECK(aggregation.cyc == 1 && aggregation.sum == 2500000.0);

  CHECK(x_succ(event.tic(&event, NULL, 0)));
  api->record_error = FAIL_RECORD;
  snapshot = aggregation;
  check_error(event.agg(&event, &aggregation, 2, NULL, 0), domain, FAIL_RECORD);
  check_same_aggregation(&aggregation, &snapshot);
  check_error(event.status, domain, FAIL_RECORD);
  check_request_errors(&event);
  check_error(event.agg(&event, NULL, 2, NULL, 0), "posix", EINVAL);
  CHECK(!event.stopped && event.dur(&event, "s") == -1.0);
  check_error(event.status, domain, FAIL_RECORD);
  api->record_error = 0;
  CHECK(x_succ(event.agg(&event, &aggregation, 2, NULL, 0)));
  CHECK(aggregation.cyc == 2 && aggregation.ready && aggregation.avg == 2500000.0);
  CHECK(aggregation.min.idx == 0 && aggregation.max.idx == 0);

  api->record_error = FAIL_RECORD;
  check_error(event.tic(&event, NULL, 0), domain, FAIL_RECORD);
  CHECK(!event.started && !event.stopped && event.dur(&event, "ms") == -1.0);
  check_error(event.status, domain, FAIL_RECORD);
  snapshot = aggregation;
  int records = api->record_calls;
  CHECK(x_succ(event.agg(&event, &aggregation, 2, NULL, 0)));
  check_error(event.status, domain, FAIL_RECORD);
  CHECK(api->record_calls == records);
  check_same_aggregation(&aggregation, &snapshot);
  x_event_agg unrelated;
  CHECK(x_succ(event.agg(&event, &unrelated, 0, NULL, 0)));
  check_error(event.status, domain, FAIL_RECORD);
  check_error(event.agg(&event, NULL, 2, NULL, 0), "posix", EINVAL);
  CHECK(event.dur(&event, "bogus") == -1.0);
  check_error(event.status, domain, FAIL_RECORD);
  check_error(event.toc(&event, NULL, 0), domain, FAIL_RECORD);
  api->record_error = 0;
  CHECK(x_succ(event.tic(&event, NULL, 0)));
  CHECK(x_succ(event.toc(&event, NULL, 0)));
  CHECK(x_succ(event.status));
  const float invalid_values[] = {NAN, -1.0f, INFINITY, -INFINITY};
  const int expected_errors[] = {EDOM, ERANGE, ERANGE, ERANGE};
  aggregation.reset(&aggregation);
  snapshot = aggregation;
  for (size_t i = 0; i < sizeof(invalid_values) / sizeof(invalid_values[0]); ++i) {
    api->milliseconds = invalid_values[i];
    CHECK(event.dur(&event, "ns") == -1.0);
    check_error(event.status, "posix", expected_errors[i]);
    check_request_errors(&event);
    output = -123.0;
    check_error(duration(samples, &output), "posix", expected_errors[i]);
    CHECK(output == -123.0);
    CHECK(x_succ(event.tic(&event, NULL, 0)));
    check_error(event.agg(&event, &aggregation, 1, NULL, 0), "posix", expected_errors[i]);
    check_same_aggregation(&aggregation, &snapshot);
  }

  api->record_error = FAIL_RECORD;
  check_error(event.toc(&event, NULL, 0), domain, FAIL_RECORD);
  check_error(event.status, domain, FAIL_RECORD);
  api->fail_all_destroys = true;
  check_error(x_event_term(&event), domain, FAIL_DESTROY);
  CHECK(api->destroy_calls == 2 && api->live == 2 && event.type != -1);
  CHECK(!event.started && !event.stopped);
  check_error(event.status, domain, FAIL_DESTROY);

  api->fail_all_destroys = false;
  api->fail_destroy_on = api->destroy_calls + 1;
  check_error(x_event_term(&event), domain, FAIL_DESTROY);
  CHECK(api->destroy_calls == 4 && api->live == 1 && event.type != -1);
  CHECK(!event.started && !event.stopped);
  CHECK(x_succ(x_event_term(&event)) && api->live == 0 && event.type == -1);
  int destroyed = api->destroy_calls;
  CHECK(x_succ(x_event_term(&event)) && api->destroy_calls == destroyed);
  CHECK(event.dur(&event, "ms") == -1.0);
  CHECK(x_succ(event.status));
  check_error(event.toc(&event, NULL, 0), "posix", EINVAL);
  CHECK(x_succ(event.status));
}

static void test_timestamp_and_sleep(void)
{
  char buffer[26] = "old";
  CHECK(x_timestamp(buffer, 25) == buffer && buffer[0] == '\0');
  CHECK(conversion_calls == 0);
  fail_time = 1;
  CHECK(x_timestamp(buffer, 26) == buffer && buffer[0] == '\0');
  CHECK(conversion_calls == 0);
  fail_time = 0;
  fail_conversion = 1;
  CHECK(x_timestamp(buffer, 26) == buffer && buffer[0] == '\0');
  fail_conversion = 0;
  CHECK(x_timestamp(buffer, 26) == buffer);
  CHECK(strcmp(buffer, "Sat Oct  3 12:00:00 2026") == 0);
  x_sleep(1234);
  CHECK(sleep_calls == (X_WINDOWS ? 1 : 2));
}

int main(void)
{
  test_cpu();
  test_cuda_recording(&driver, "cu");
  test_cuda_recording(&runtime, "cuda");
  test_cuda(&driver, "cu");
  test_cuda(&runtime, "cuda");
  test_timestamp_and_sleep();
  return 0;
}
