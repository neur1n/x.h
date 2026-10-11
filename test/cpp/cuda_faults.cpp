#if TEST_FULL_HEADER
#include "../../cpp/x.h"
#else
#include "generated_under_test.h"
#endif
#include "support.h"

#if TEST_EVENTS
#include "date_time_support.h"

static void test_events(const char* type, test_backend& backend)
{
  const int32_t create_error = strcmp(type, "cu") == 0 ? 7 : 8;
  backend = {};
  expect_throw<std::invalid_argument>([&] { x_event event{type, 2}; });
  CHECK(backend.creates == 0);
  for (int failure : {1, 2}) {
    backend = {};
    backend.fail_create_at = failure;
    expect_event_error([&] { x_event event{type}; }, type, create_error);
    CHECK(backend.creates == failure && backend.live == 0);
    CHECK(backend.destroys == failure - 1);
  }
  backend = {};
  backend.fail_create_at = 2;
  backend.fail_destroy_at = 1;
  expect_event_error([&] { x_event event{type}; }, type, create_error);
  CHECK(backend.creates == 2 && backend.destroys == 1);
  // Synthetic failed releases retain a fake handle, not allocated host memory.
  backend = {};
  {
    x_event event{type};
    CHECK(backend.live == 2);
    x_event_agg stats;
    check_event_status(event.agg(stats, 1), "posix", ENODATA);
    CHECK(x_succ(event.status()));
    CHECK(stats.cyc == 0 && !stats.ready && backend.records == 0);
    CHECK(x_succ(event.tic()));
    const void* start = backend.last_record_event;
    backend.record_error = 7;
    check_event_status(event.agg(stats, 1), type, 7);
    const void* stop = backend.last_record_event;
    CHECK(start != nullptr && stop != nullptr && start != stop);
    CHECK(stats.cyc == 0 && backend.synchronizes == 0);
    check_duration_error(event, type, 7);
    backend.record_error = 0;
    CHECK(x_succ(event.agg(stats, 1)));
    CHECK(backend.last_record_event == stop && backend.last_sync_event == stop);
    CHECK(backend.last_elapsed_start == start);
    CHECK(backend.last_elapsed_stop == stop);
    CHECK(stats.ready && stats.cyc == 1 && stats.sum == 2e6);
    CHECK(event.dur("us") == 2000.0 && event.dur("ms") == 2.0);
    const int records = backend.records;
    CHECK(x_succ(event.agg(stats, 1)));
    CHECK(backend.records == records);
    CHECK(x_succ(event.agg(stats, 0)));
    CHECK(stats.cyc == 0 && !stats.ready);
    backend.sync_error = 7;
    check_event_status(event.agg(stats, 1), type, 7);
    CHECK(stats.cyc == 0);
    backend.sync_error = 0;
    backend.elapsed_error = 7;
    check_event_status(event.agg(stats, 1), type, 7);
    CHECK(stats.cyc == 0);
    backend.elapsed_error = 0;
    for (float invalid : {-1.0f, (std::numeric_limits<float>::infinity)(),
                          (std::numeric_limits<float>::quiet_NaN)()}) {
      backend.milliseconds = invalid;
      check_event_status(event.agg(stats, 1), "posix",
          std::isnan(invalid) ? EDOM : ERANGE);
      CHECK(stats.cyc == 0 && !stats.ready);
    }
    backend.milliseconds = 0.0f;
    CHECK(x_succ(event.agg(stats, 2)));
    CHECK(stats.cyc == 1 && stats.sum == 0.0 && !stats.ready);
    CHECK(x_succ(event.tic()));
    backend.milliseconds = 1.0f;
    CHECK(x_succ(event.agg(stats, 2)));
    CHECK(stats.ready && stats.sum == 1e6);
    CHECK(stats.min.idx == 0 && stats.max.idx == 1);
    backend.record_error = 7;
    check_event_status(event.tic(), type, 7);
    check_duration_error(event, type, 7);
    backend.record_error = 0;
    CHECK(x_succ(event.tic()));
    CHECK(x_succ(event.toc()));
    CHECK(event.dur() == 1e6 && x_succ(event.status()));
    void* stream = reinterpret_cast<void*>(std::uintptr_t{16});
    CHECK(x_succ(event.tic(stream, 1)));
    CHECK(backend.last_stream == stream && backend.last_flags == 1);
  }
  CHECK(backend.live == 0 && backend.destroys == 2);
  backend = {};
  {
    x_event event{type};
    backend.fail_destroy_at = 1;
  }
  // Failure on the first destruction must not prevent the other attempt.
  CHECK(backend.destroys == 2 && backend.live == 1);
  backend = {};
}
#endif
#if TEST_TOOLS
static void test_function() {}

static void test_device_counts(const char* api, test_backend& backend)
{
  for (int count : {3, 0, -1}) {
    backend.devices = count;
    CHECK(x_ngpu(api) == (count < 0 ? 0 : static_cast<size_t>(count)));
  }
  CHECK(backend.device_queries == 3 && backend.message_calls == 0);
}
#endif

#if TEST_QUERIES
static void test_memory_queries(
    const char* type, test_backend& backend, int32_t failure)
{
  size_t available{7}, total{9};
  const int before{backend.memory_queries};
  const x_error invalid{x_meminfo(type, nullptr, nullptr)};
  CHECK(invalid.value() == EINVAL && strcmp(invalid.domain(), "posix") == 0);
  CHECK(backend.memory_queries == before);
  const x_error success{x_meminfo(type, &available, &total)};
  CHECK(x_succ(success) && strcmp(success.domain(), type) == 0);
  CHECK(available == 128 && total == 256);
  available = 7;
  total = 9;
  CHECK(x_succ(x_meminfo(type, &available, nullptr)) && available == 128);
  CHECK(total == 9);
  CHECK(x_succ(x_meminfo(type, nullptr, &total)) && total == 256);
  size_t aliased{0};
  CHECK(x_succ(x_meminfo(type, &aliased, &aliased)) && aliased == 256);
  CHECK(backend.memory_queries == before + 4);

  backend.query_error = failure;
  available = 7;
  total = 9;
  const x_error error{x_meminfo(type, &available, &total)};
  CHECK(x_fail(error) && error.value() == failure);
  CHECK(strcmp(error.domain(), type) == 0);
  CHECK(available == 7 && total == 9);
  CHECK(x_meminfo(type, &available, nullptr).value() == failure);
  CHECK(x_meminfo(type, nullptr, &total).value() == failure);
  CHECK(available == 7 && total == 9);
  backend.query_error = 0;
}
#endif

int main()
{
#if TEST_EVENTS
#if X_ENABLE_CUDA_DRIVER
  test_events("cu", driver);
#endif
#if X_ENABLE_CUDA_RUNTIME
  test_events("cuda", runtime);
#endif
#endif
#if TEST_QUERIES
#if X_ENABLE_CUDA_DRIVER
  test_memory_queries("cu", driver, 7);
#else
  CHECK(x_meminfo("cu", nullptr, nullptr).value() == ENOTSUP);
#endif
#if X_ENABLE_CUDA_RUNTIME
  test_memory_queries("cuda", runtime, 8);
#else
  CHECK(x_meminfo("cuda", nullptr, nullptr).value() == ENOTSUP);
#endif
#endif
#if TEST_TOOLS
  driver = {};
  runtime = {};
  CHECK(x_ngpu(nullptr) == 0 && x_ngpu("") == 0);
  CHECK(x_ngpu("missing") == 0);
  CHECK(strcmp(x_memtype(nullptr, nullptr), "N/A") == 0);
  CHECK(strcmp(x_memtype("", nullptr), "N/A") == 0);
  CHECK(strcmp(x_memtype("missing", nullptr), "N/A") == 0);
  CHECK(strcmp(x_memtype("cu", nullptr), "N/A") == 0);
  CHECK(strcmp(x_memtype("cu", 0), "N/A") == 0);
  CHECK(strcmp(x_memtype("cu", &test_function), "N/A") == 0);
  CHECK(strcmp(x_memtype("cuda", 0), "N/A") == 0);
  CHECK(strcmp(x_memtype("cuda", &test_function), "N/A") == 0);
#if !X_ENABLE_CUDA_DRIVER
  CHECK(x_ngpu("cu") == 0);
  CHECK(strcmp(x_memtype("cu", UINT64_C(0)), "N/A") == 0);
#endif
#if !X_ENABLE_CUDA_RUNTIME
  CHECK(x_ngpu("cuda") == 0);
  CHECK(strcmp(x_memtype("cuda", nullptr), "N/A") == 0);
#endif
  CHECK(driver.pointer_queries == 0 && runtime.pointer_queries == 0);
  CHECK(driver.device_queries == 0 && runtime.device_queries == 0);
  CHECK(driver.message_calls == 0 && runtime.message_calls == 0);
#if X_ENABLE_CUDA_DRIVER
  const CUdeviceptr pointer = UINT64_C(0x123456789);
  const char* names[]{"Unknown", "Host", "Device", "Array", "Unified"};
  for (int type = 0; type < 5; ++type) {
    driver.memory_type = type;
    CHECK(strcmp(x_memtype("cu", pointer), names[type]) == 0);
    CHECK(driver.last_driver_pointer == pointer);
  }
  driver.memory_type = 99;
  CHECK(strcmp(x_memtype("cu", pointer), "Unknown") == 0);
  driver.memory_type = CU_MEMORYTYPE_HOST;
  CHECK(strcmp(x_memtype("cu", static_cast<CUdeviceptr>(0)), "Host") == 0);
  CHECK(driver.last_driver_pointer == 0);
  const int driver_queries = driver.pointer_queries;
  CHECK(strcmp(x_memtype("cu", static_cast<void*>(nullptr)), "N/A") == 0);
  CHECK(strcmp(x_memtype("cuda", pointer), "N/A") == 0);
  CHECK(driver.pointer_queries == driver_queries && runtime.pointer_queries == 0);
  test_device_counts("cu", driver);
  driver.query_error = 7;
  CHECK(strcmp(x_memtype("cu", pointer), "Unknown") == 0 && x_ngpu("cu") == 0);
  CHECK(driver.message_calls == 1 && driver.last_error == 7);
  driver.message_error = 1;
  CHECK(x_ngpu("cu") == 0);
  CHECK(driver.message_calls == 2 && driver.last_error == 7);
  CHECK(std::string(x_error("cu", 7).message()) == "CUDA driver error 7");
  driver.message_error = 0;
  driver.null_message = true;
  const int driver_messages = driver.message_calls;
  CHECK(x_ngpu("cu") == 0);
  CHECK(driver.message_calls == driver_messages + 1 && driver.last_error == 7);
  driver.null_message = false;
  const std::string driver_text(300, 'D');
  driver.message = driver_text.c_str();
  CHECK(x_ngpu("cu") == 0);
  CHECK(driver.message_calls == driver_messages + 2 && driver.last_error == 7);
  driver = {};
#endif
#if X_ENABLE_CUDA_RUNTIME
  const float value{1.0f};
  const char* runtime_names[]{"Unregistered", "Host", "Device", "Managed"};
  for (int type = 0; type < 4; ++type) {
    runtime.memory_type = type;
    CHECK(strcmp(x_memtype("cuda", &value), runtime_names[type]) == 0);
    CHECK(runtime.last_pointer == &value);
  }
  CHECK(strcmp(x_memtype("cuda", nullptr), "Managed") == 0);
  CHECK(runtime.last_pointer == nullptr);
  void* untyped = const_cast<float*>(&value);
  CHECK(strcmp(x_memtype("cuda", untyped), "Managed") == 0);
  CHECK(runtime.last_pointer == &value);
  volatile float volatile_value{1.0f};
  CHECK(strcmp(x_memtype("cuda", &volatile_value), "Managed") == 0);
  CHECK(runtime.last_pointer == const_cast<const float*>(&volatile_value));
  const volatile float qualified_value{2.0f};
  CHECK(strcmp(x_memtype("cuda", &qualified_value), "Managed") == 0);
  CHECK(runtime.last_pointer == const_cast<const float*>(&qualified_value));
  runtime.memory_type = 99;
  CHECK(strcmp(x_memtype("cuda", &value), "Unknown") == 0);
  const int runtime_queries = runtime.pointer_queries;
  CHECK(strcmp(x_memtype("cuda", 0), "N/A") == 0);
  CHECK(strcmp(x_memtype("cuda", &test_function), "N/A") == 0);
  CHECK(runtime.pointer_queries == runtime_queries);
  CHECK(value == 1.0f && volatile_value == 1.0f && qualified_value == 2.0f);
  test_device_counts("cuda", runtime);
  runtime.query_error = 8;
  CHECK(strcmp(x_memtype("cuda", &value), "Unknown") == 0 && x_ngpu("cuda") == 0);
  CHECK(runtime.message_calls == 1 && runtime.last_error == 8);
  runtime.null_message = true;
  CHECK(x_ngpu("cuda") == 0);
  CHECK(runtime.message_calls == 2 && runtime.last_error == 8);
  CHECK(std::string(x_error("cuda", 8).message()) == "CUDA runtime error");
  runtime.null_message = false;
  const int runtime_messages = runtime.message_calls;
  const std::string runtime_text(300, 'R');
  runtime.message = runtime_text.c_str();
  CHECK(x_ngpu("cuda") == 0);
  CHECK(runtime.message_calls == runtime_messages + 1 && runtime.last_error == 8);
  runtime = {};
#endif
#endif
  return 0;
}
