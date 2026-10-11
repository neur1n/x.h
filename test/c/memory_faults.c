// Extracted production code with injected system/CUDA API results. This checks
// all branches on one host; it does not replace a macOS SDK or GPU runtime test.
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define X_INL static inline
#define X_LINUX (TEST_PLATFORM == 1)
#define X_MACOS (TEST_PLATFORM == 2)
#define X_WINDOWS (TEST_PLATFORM == 3)
#define X_ENABLE_CUDA_DRIVER 1
#define X_ENABLE_CUDA_RUNTIME 1
#define X_ENABLE_CUBLAS 1
#if TEST_NARROW
// Exercise representability guards with a 32-bit limit, even on a 64-bit host.
#undef SIZE_MAX
#define SIZE_MAX UINT32_MAX
#endif
#define CHECK(condition) do { \
  if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    exit(EXIT_FAILURE); \
  } \
} while (0)

typedef struct { const char* domain; int32_t value; } x_error;
static unsigned long win32_error;
static x_error test_error(const char* domain, int32_t value)
{
  x_error error = {domain, value};
  return error;
}
static x_error test_capture(const char* domain)
{
  return test_error(domain,
      strcmp(domain, "win32") == 0 ? (int32_t)win32_error : errno);
}
static x_error x_ok(void) { return test_error("posix", 0); }
static bool x_fail(const x_error error) { return error.value != 0; }
#define TEST_SELECT(_1, _2, NAME, ...) NAME
#define x_error_set(...) \
  TEST_SELECT(__VA_ARGS__, test_error, test_capture)(__VA_ARGS__)

#if X_LINUX || X_MACOS || X_WINDOWS
static uint64_t free_bytes = 4096, total_bytes = 65536;
static int query_error;
#endif
#if X_LINUX
struct sysinfo { unsigned long freeram, totalram; unsigned int mem_unit; };
static unsigned int memory_unit = 1024;
static int sysinfo(struct sysinfo* info)
{
  if (query_error) { errno = query_error; return -1; }
  info->freeram = (unsigned long)(free_bytes / memory_unit);
  info->totalram = (unsigned long)(total_bytes / memory_unit);
  info->mem_unit = memory_unit;
  return 0;
}
#elif X_MACOS
typedef unsigned int host_t;
typedef int kern_return_t;
typedef uint64_t vm_size_t;
typedef unsigned int mach_msg_type_number_t;
typedef int64_t* host_info64_t;
typedef struct { uint32_t free_count, speculative_count; } vm_statistics64_data_t;
#define HOST_VM_INFO64 4
#define HOST_VM_INFO64_COUNT 2
#define KERN_SUCCESS 0
static vm_size_t page_bytes = 4096;
static int page_error, stats_error, release_error;
static int total_calls, page_calls, stats_calls, acquired, released;
static bool short_sysctl;
static int sysctlbyname(
    const char* name, void* output, size_t* size, void* input, size_t input_size)
{
  CHECK(strcmp(name, "hw.memsize") == 0 && input == NULL && input_size == 0);
  CHECK(*size == sizeof(uint64_t));
  ++total_calls;
  if (query_error) { errno = query_error; return -1; }
  *(uint64_t*)output = total_bytes;
  if (short_sysctl) --*size;
  return 0;
}
static host_t mach_host_self(void) { ++acquired; return 11; }
static host_t mach_task_self(void) { return 12; }
static kern_return_t host_page_size(host_t host, vm_size_t* size)
{
  CHECK(host == 11);
  ++page_calls;
  *size = page_bytes;
  return page_error;
}
static kern_return_t host_statistics64(
    host_t host, int flavor, host_info64_t output, mach_msg_type_number_t* count)
{
  CHECK(host == 11 && flavor == HOST_VM_INFO64 && *count == HOST_VM_INFO64_COUNT);
  ++stats_calls;
  vm_statistics64_data_t* info = (vm_statistics64_data_t*)output;
  info->free_count = (uint32_t)(free_bytes / page_bytes);
  info->speculative_count = 1; // Already included in free_count.
  return stats_error;
}
static kern_return_t mach_port_deallocate(host_t task, host_t host)
{
  CHECK(task == 12 && host == 11);
  ++released;
  errno = EBUSY; // Mach status must survive unrelated errno changes in cleanup.
  return release_error;
}
#elif X_WINDOWS
typedef struct {
  unsigned long dwLength;
  uint64_t ullAvailPhys, ullTotalPhys;
} MEMORYSTATUSEX;
static int GlobalMemoryStatusEx(MEMORYSTATUSEX* status)
{
  CHECK(status->dwLength == sizeof(*status));
  if (query_error) { win32_error = (unsigned long)query_error; return 0; }
  status->ullAvailPhys = free_bytes;
  status->ullTotalPhys = total_bytes;
  return 1;
}
#endif

// Minimal SDK-shaped declarations compile the cleaned CUDA code too. Synthetic
// enum values/statuses deliberately do not model actual CUDA execution.
typedef uint64_t CUdeviceptr, CUdeviceptr_v2;
typedef enum { CUDA_SUCCESS = 0, TEST_CU_FAILURE = 17 } CUresult;
typedef enum {
  CU_MEMORYTYPE_HOST = 1, CU_MEMORYTYPE_DEVICE, CU_MEMORYTYPE_ARRAY,
  CU_MEMORYTYPE_UNIFIED
} CUmemorytype;
#define CU_POINTER_ATTRIBUTE_MEMORY_TYPE 2
typedef enum { cudaSuccess = 0, TEST_CUDA_FAILURE = 18 } cudaError_t;
enum cudaMemoryType {
  cudaMemoryTypeUnregistered, cudaMemoryTypeHost, cudaMemoryTypeDevice,
  cudaMemoryTypeManaged
};
// The CUDA SDK declares a struct tag, not a C typedef of the same name.
struct cudaPointerAttributes { enum cudaMemoryType type; };
typedef enum { CUBLAS_STATUS_SUCCESS = 0, TEST_CUBLAS_FAILURE = 7 } cublasStatus_t;
static CUresult driver_status;
static cudaError_t runtime_status;
static unsigned int driver_memory_type = CU_MEMORYTYPE_HOST;
static enum cudaMemoryType runtime_memory_type = cudaMemoryTypeHost;
static CUdeviceptr last_driver_pointer;
static const void* last_runtime_pointer;
static int device_count = 1;
static CUresult message_status;
static bool null_message;
static const char* driver_message = "driver failure";
static const char* runtime_message = "runtime failure";
static const char* cublas_message = "cublas status";
static int driver_memory_queries, runtime_memory_queries;
static int driver_message_calls, runtime_message_calls, cublas_message_calls;
static int32_t last_driver_error, last_runtime_error, last_cublas_error;
static CUresult cuMemGetInfo_v2(size_t* available, size_t* total)
{
  ++driver_memory_queries;
  CHECK(available != NULL && total != NULL && available != total);
  *available = driver_status == CUDA_SUCCESS ? 128 : 999;
  *total = driver_status == CUDA_SUCCESS ? 256 : 777;
  return driver_status;
}
static cudaError_t cudaMemGetInfo(size_t* available, size_t* total)
{
  ++runtime_memory_queries;
  CHECK(available != NULL && total != NULL && available != total);
  *available = runtime_status == cudaSuccess ? 128 : 999;
  *total = runtime_status == cudaSuccess ? 256 : 777;
  return runtime_status;
}
static CUresult cuPointerGetAttribute(void* output, int attribute, CUdeviceptr p)
{
  last_driver_pointer = p;
  CHECK(attribute == CU_POINTER_ATTRIBUTE_MEMORY_TYPE);
  if (driver_status != CUDA_SUCCESS) return driver_status;
  *(unsigned int*)output = driver_memory_type;
  return driver_status;
}
static cudaError_t cudaPointerGetAttributes(
    struct cudaPointerAttributes* info, const void* p)
{
  last_runtime_pointer = p;
  if (runtime_status != cudaSuccess) return runtime_status;
  info->type = runtime_memory_type;
  return runtime_status;
}
static CUresult cuDeviceGetCount(int* count)
{
  *count = device_count;
  return driver_status;
}
static cudaError_t cudaGetDeviceCount(int* count)
{
  *count = device_count;
  return runtime_status;
}
static CUresult cuGetErrorString(CUresult value, const char** message)
{
  ++driver_message_calls;
  last_driver_error = (int32_t)value;
  *message = null_message ? NULL : driver_message;
  return message_status;
}
static const char* cudaGetErrorString(cudaError_t value)
{
  ++runtime_message_calls;
  last_runtime_error = (int32_t)value;
  return runtime_message;
}
static const char* cublasGetStatusString(cublasStatus_t value)
{
  ++cublas_message_calls;
  last_cublas_error = (int32_t)value;
  return cublas_message;
}

static char cuda_diagnostic[256];
static int diagnostic_calls;
static int test_cuda_fprintf(FILE* stream, const char* format, ...)
{
  CHECK(stream == stderr);
  ++diagnostic_calls;
  va_list args;
  va_start(args, format);
  int result = vsnprintf(cuda_diagnostic, sizeof(cuda_diagnostic), format, args);
  va_end(args);
  return result;
}

#define fprintf test_cuda_fprintf
#include "memory_under_test.h"
#undef fprintf

static void test_cuda(void)
{
  const CUdeviceptr device_pointer = (CUdeviceptr)UINT64_C(0x123456789);
  const float object = 1.0f;
  CHECK(strcmp(x_memtype(NULL, (void*)NULL), "N/A") == 0);
  CHECK(strcmp(x_memtype("", (void*)NULL), "N/A") == 0);
  CHECK(strcmp(x_memtype("unknown", (void*)NULL), "N/A") == 0);

  static const char* driver_names[] = {"Unknown", "Host", "Device", "Array", "Unified"};
  for (unsigned int i = 0; i < 5; ++i) {
    driver_memory_type = i;
    CHECK(strcmp(x_memtype("cu", device_pointer), driver_names[i]) == 0);
    CHECK(last_driver_pointer == device_pointer);
  }
  driver_memory_type = UINT_MAX;
  CHECK(strcmp(x_memtype("cu", device_pointer), "Unknown") == 0);
  driver_status = TEST_CU_FAILURE;
  CHECK(strcmp(x_memtype("cu", device_pointer), "Unknown") == 0);
  driver_status = CUDA_SUCCESS;
  driver_memory_type = CU_MEMORYTYPE_HOST;
  CHECK(strcmp(x_memtype("cu", (CUdeviceptr)0), "Host") == 0);
  CHECK(last_driver_pointer == 0);

  static const char* runtime_names[] = {"Unregistered", "Host", "Device", "Managed"};
  for (int i = 0; i < 4; ++i) {
    runtime_memory_type = (enum cudaMemoryType)i;
    CHECK(strcmp(x_memtype("cuda", (void*)&object), runtime_names[i]) == 0);
    CHECK(last_runtime_pointer == &object && object == 1.0f);
  }
  runtime_memory_type = (enum cudaMemoryType)99;
  CHECK(strcmp(x_memtype("cuda", (void*)&object), "Unknown") == 0);
  runtime_status = TEST_CUDA_FAILURE;
  CHECK(strcmp(x_memtype("cuda", (void*)&object), "Unknown") == 0);
  runtime_status = cudaSuccess;
  runtime_memory_type = cudaMemoryTypeHost;
  CHECK(strcmp(x_memtype("cuda", (void*)NULL), "Host") == 0);
  CHECK(last_runtime_pointer == NULL);

  CHECK(x_ngpu(NULL) == 0 && x_ngpu("") == 0 && x_ngpu("unknown") == 0);
  device_count = 3;
  CHECK(x_ngpu("cu") == 3 && x_ngpu("cuda") == 3);
  device_count = 0;
  CHECK(x_ngpu("cu") == 0 && x_ngpu("cuda") == 0);
  CHECK(diagnostic_calls == 0);
  device_count = 1;

  driver_status = TEST_CU_FAILURE;
  CHECK(x_ngpu("cu") == 0 && diagnostic_calls == 1);
  CHECK(strcmp(cuda_diagnostic, "cuDeviceGetCount: driver failure\n") == 0);
  message_status = (CUresult)101;
  CHECK(x_ngpu("cu") == 0 && diagnostic_calls == 2);
  CHECK(strcmp(cuda_diagnostic, "cuDeviceGetCount: CUDA driver error 17\n") == 0);
  message_status = CUDA_SUCCESS;
  null_message = true;
  CHECK(x_ngpu("cu") == 0 && diagnostic_calls == 3);
  CHECK(strcmp(cuda_diagnostic, "cuDeviceGetCount: CUDA driver error 17\n") == 0);
  null_message = false;
  driver_status = CUDA_SUCCESS;
  runtime_status = TEST_CUDA_FAILURE;
  CHECK(x_ngpu("cuda") == 0 && diagnostic_calls == 4);
  CHECK(strcmp(cuda_diagnostic, "cudaGetDeviceCount: runtime failure\n") == 0);
  runtime_status = cudaSuccess;
}

typedef char* (*test_message_function)(char*, size_t, int32_t);

static void check_message_buffers(
    test_message_function format, int* calls, const int32_t* last_error,
    int32_t value, const char* expected)
{
  char storage[80];
  memset(storage, 'Z', sizeof(storage));
  int before = *calls;
  CHECK(format(NULL, 0, value) == NULL);
  CHECK(format(NULL, 64, value) == NULL);
  CHECK(format(storage + 1, 0, value) == storage + 1);
  CHECK(*calls == before);
  for (size_t i = 0; i < sizeof(storage); ++i) CHECK(storage[i] == 'Z');

  const size_t length = strlen(expected);
  const size_t capacities[] = {1, 4, length + 1, length + 2, 64};
  for (size_t i = 0; i < sizeof(capacities) / sizeof(capacities[0]); ++i) {
    const size_t size = capacities[i];
    CHECK(size < sizeof(storage) - 1);
    memset(storage, 'Z', sizeof(storage));
    CHECK(format(storage + 1, size, value) == storage + 1);
    CHECK(*calls == before + (int)i + 1 && *last_error == value);
    const size_t copied = length < size - 1 ? length : size - 1;
    CHECK(memcmp(storage + 1, expected, copied) == 0);
    CHECK(storage[copied + 1] == '\0');
    CHECK(storage[0] == 'Z');
    for (size_t j = size + 1; j < sizeof(storage); ++j) {
      CHECK(storage[j] == 'Z');
    }
  }
}

static void test_cuda_errors(void)
{
  CHECK(!_x_error_failed_cublas(CUBLAS_STATUS_SUCCESS));
  CHECK(_x_error_failed_cublas(TEST_CUBLAS_FAILURE));
  CHECK(!_x_error_failed_cuda_driver(CUDA_SUCCESS));
  CHECK(_x_error_failed_cuda_driver(TEST_CU_FAILURE));
  CHECK(!_x_error_failed_cuda_runtime(cudaSuccess));
  CHECK(_x_error_failed_cuda_runtime(TEST_CUDA_FAILURE));

  check_message_buffers(_x_error_message_cublas,
      &cublas_message_calls, &last_cublas_error, TEST_CUBLAS_FAILURE,
      "cublas status");
  check_message_buffers(_x_error_message_cuda_driver,
      &driver_message_calls, &last_driver_error, TEST_CU_FAILURE,
      "driver failure");
  check_message_buffers(_x_error_message_cuda_runtime,
      &runtime_message_calls, &last_runtime_error, TEST_CUDA_FAILURE,
      "runtime failure");

  // SDK text must be copied literally, never interpreted as a format string.
  driver_message = runtime_message = cublas_message = "error %s %n %%";
  check_message_buffers(_x_error_message_cublas,
      &cublas_message_calls, &last_cublas_error, TEST_CUBLAS_FAILURE,
      "error %s %n %%");
  check_message_buffers(_x_error_message_cuda_driver,
      &driver_message_calls, &last_driver_error, TEST_CU_FAILURE,
      "error %s %n %%");
  check_message_buffers(_x_error_message_cuda_runtime,
      &runtime_message_calls, &last_runtime_error, TEST_CUDA_FAILURE,
      "error %s %n %%");
  driver_message = "driver failure";
  runtime_message = "runtime failure";
  cublas_message = "cublas status";

  // Preserve the original error, not the failed string query's status (101).
  message_status = (CUresult)101;
  check_message_buffers(_x_error_message_cuda_driver,
      &driver_message_calls, &last_driver_error, TEST_CU_FAILURE,
      "CUDA driver error 17");
  message_status = CUDA_SUCCESS;
  null_message = true;
  check_message_buffers(_x_error_message_cuda_driver,
      &driver_message_calls, &last_driver_error, TEST_CU_FAILURE,
      "CUDA driver error 17");
  null_message = false;

  runtime_message = NULL;
  check_message_buffers(_x_error_message_cuda_runtime,
      &runtime_message_calls, &last_runtime_error, TEST_CUDA_FAILURE,
      "CUDA runtime error");
  runtime_message = "runtime failure";
  cublas_message = NULL;
  check_message_buffers(_x_error_message_cublas,
      &cublas_message_calls, &last_cublas_error, TEST_CUBLAS_FAILURE,
      "cuBLAS error");
  cublas_message = "cublas status";

  // Exercise the SDK's unrecognized-code result, distinct from NULL fallback.
  runtime_message = "unrecognized error code";
  check_message_buffers(_x_error_message_cuda_runtime,
      &runtime_message_calls, &last_runtime_error, 1234,
      "unrecognized error code");
  runtime_message = "runtime failure";
}

static void expect_failure(const char* domain, int32_t value)
{
  size_t available = 123, total = 456;
  x_error error = x_meminfo("cpu", &available, &total);
  CHECK(strcmp(error.domain, domain) == 0 && error.value == value);
  CHECK(available == 123 && total == 456);
}

static void test_gpu_memory(const char* type, int* calls, const int32_t failure)
{
  size_t available = 7, total = 9;
  const int before = *calls;
  const x_error invalid = x_meminfo(type, NULL, NULL);
  CHECK(invalid.value == EINVAL && strcmp(invalid.domain, "posix") == 0);
  CHECK(*calls == before);
  x_error error = x_meminfo(type, &available, &total);
  CHECK(error.value == 0 && strcmp(error.domain, type) == 0);
  CHECK(available == 128 && total == 256);
  available = 7;
  total = 9;
  CHECK(x_meminfo(type, &available, NULL).value == 0 && available == 128);
  CHECK(total == 9);
  CHECK(x_meminfo(type, NULL, &total).value == 0 && total == 256);
  size_t aliased = 0;
  CHECK(x_meminfo(type, &aliased, &aliased).value == 0 && aliased == 256);
  CHECK(*calls == before + 4);

  if (strcmp(type, "cu") == 0) {
    driver_status = (CUresult)failure;
  } else {
    runtime_status = (cudaError_t)failure;
  }
  available = 7;
  total = 9;
  error = x_meminfo(type, &available, &total);
  CHECK(strcmp(error.domain, type) == 0 && error.value == failure);
  CHECK(available == 7 && total == 9);
  CHECK(x_meminfo(type, &available, NULL).value == failure);
  CHECK(x_meminfo(type, NULL, &total).value == failure);
  CHECK(available == 7 && total == 9);
  driver_status = CUDA_SUCCESS;
  runtime_status = cudaSuccess;
}

int main(void)
{
  test_cuda();
  test_cuda_errors();
  size_t available = 123, total = 456;
  CHECK(x_meminfo("cpu", NULL, NULL).value == EINVAL);
  CHECK(x_meminfo(NULL, &available, &total).value == EINVAL);
  CHECK(x_meminfo("unknown", &available, &total).value == ENOTSUP);
  CHECK(available == 123 && total == 456);
#if X_LINUX || X_MACOS || X_WINDOWS
  CHECK(x_meminfo("cpu", &available, &total).value == 0);
  CHECK(available == free_bytes && total == total_bytes);
  CHECK(x_meminfo("cpu", &available, NULL).value == 0 && available == free_bytes);
  CHECK(x_meminfo("cpu", NULL, &total).value == 0 && total == total_bytes);
  query_error = EIO;
#if X_WINDOWS
  expect_failure("win32", EIO);
#else
  expect_failure("posix", EIO);
#endif
  query_error = 0;
#if TEST_NARROW
  // Fail atomically if either requested value is too large. An unrequested
  // total must not prevent an available-only query from succeeding.
  total_bytes = UINT64_C(1) << 33;
  expect_failure("posix", ERANGE);
  CHECK(x_meminfo("cpu", &available, NULL).value == 0);
  total_bytes = 65536;
  free_bytes = UINT64_C(1) << 33;
  expect_failure("posix", ERANGE);
  CHECK(x_meminfo("cpu", NULL, &total).value == 0);
  free_bytes = 4096;
#endif
#if X_MACOS
  CHECK(acquired == released);
  int before = acquired;
  CHECK(x_meminfo("cpu", NULL, &total).value == 0 && acquired == before);
  before = total_calls;
  CHECK(x_meminfo("cpu", &available, NULL).value == 0 && total_calls == before);
  short_sysctl = true;
  expect_failure("posix", EIO);
  short_sysctl = false;
  page_error = 71;
  before = stats_calls;
  expect_failure("generic", 71);
  CHECK(stats_calls == before && acquired == released);
  page_error = 0;
  stats_error = 72;
  release_error = 73;
  expect_failure("generic", 72); // Preserve primary failure over cleanup error.
  stats_error = 0;
  expect_failure("generic", 73);
  release_error = 0;
  CHECK(acquired == released && page_calls > 0);
#endif
#else
  expect_failure("posix", ENOTSUP);
  CHECK(x_meminfo("cpu", &available, NULL).value == ENOTSUP);
  CHECK(x_meminfo("cpu", NULL, &total).value == ENOTSUP);
#endif
  test_gpu_memory("cu", &driver_memory_queries, TEST_CU_FAILURE);
  test_gpu_memory("cuda", &runtime_memory_queries, TEST_CUDA_FAILURE);
  return 0;
}
