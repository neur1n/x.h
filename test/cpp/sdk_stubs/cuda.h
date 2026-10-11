#ifndef X_TEST_DRIVER_H
#define X_TEST_DRIVER_H
#include "backend.h"
enum CUresult { CUDA_SUCCESS = 0, CUDA_ERROR_INVALID_VALUE = 1, CUDA_ERROR_TEST = 7 };
using CUdeviceptr = unsigned long long;
using CUevent = struct test_cu_event*;
using CUstream = struct test_cu_stream*;
enum CUmemorytype {
  CU_MEMORYTYPE_HOST = 1, CU_MEMORYTYPE_DEVICE, CU_MEMORYTYPE_ARRAY, CU_MEMORYTYPE_UNIFIED
};
inline constexpr unsigned int CU_EVENT_DISABLE_TIMING = 2;
inline constexpr int CU_POINTER_ATTRIBUTE_MEMORY_TYPE = 2;
inline CUresult cuGetErrorString(CUresult error, const char** message)
{
  ++driver.message_calls;
  driver.last_error = static_cast<int>(error);
  *message = driver.null_message ? nullptr
      : driver.message == nullptr ? "driver:test" : driver.message;
  return static_cast<CUresult>(driver.message_error);
}
inline CUresult cuEventCreate(CUevent* handle, unsigned int flags)
{
  driver.last_flags = flags;
  ++driver.creates;
  if (driver.creates == driver.fail_create_at) {
    // Failure output is deliberately unusable; it must never become owned.
    *handle = reinterpret_cast<CUevent>(UINTPTR_MAX);
    return CUDA_ERROR_TEST;
  }
  ++driver.live;
  *handle = reinterpret_cast<CUevent>(static_cast<std::uintptr_t>(driver.creates));
  return CUDA_SUCCESS;
}
inline CUresult cuEventDestroy_v2(CUevent handle)
{
  if (reinterpret_cast<std::uintptr_t>(handle) == UINTPTR_MAX) { std::abort(); }
  ++driver.destroys;
  if (driver.destroys == driver.fail_destroy_at) { return CUDA_ERROR_TEST; }
  --driver.live;
  return CUDA_SUCCESS;
}
inline CUresult cuEventRecordWithFlags(
    CUevent event, CUstream stream, unsigned int flags)
{
  ++driver.records;
  driver.last_record_event = event;
  driver.last_stream = stream;
  driver.last_flags = flags;
  return static_cast<CUresult>(driver.record_error);
}
inline CUresult cuEventSynchronize(CUevent event)
{
  ++driver.synchronizes;
  driver.last_sync_event = event;
  return static_cast<CUresult>(driver.sync_error);
}
inline CUresult cuEventElapsedTime(float* duration, CUevent start, CUevent stop)
{
  ++driver.elapsed_calls;
  driver.last_elapsed_start = start;
  driver.last_elapsed_stop = stop;
  *duration = driver.milliseconds;
  return static_cast<CUresult>(driver.elapsed_error);
}
inline CUresult cuMemGetInfo_v2(std::size_t* available, std::size_t* total)
{
  ++driver.memory_queries;
  // Require complete SDK output storage; poison it even on failure.
  if (available == nullptr || total == nullptr || available == total) {
    std::abort();
  }
  *available = driver.query_error ? 999 : 128;
  *total = driver.query_error ? 777 : 256;
  return static_cast<CUresult>(driver.query_error);
}
inline CUresult cuPointerGetAttribute(void* output, int, CUdeviceptr pointer)
{
  ++driver.pointer_queries;
  driver.last_driver_pointer = static_cast<std::uintptr_t>(pointer);
  *static_cast<unsigned int*>(output) = static_cast<unsigned int>(driver.memory_type);
  return static_cast<CUresult>(driver.query_error);
}
inline CUresult cuDeviceGetCount(int* count)
{
  ++driver.device_queries;
  *count = driver.devices;
  return static_cast<CUresult>(driver.query_error);
}
#endif
