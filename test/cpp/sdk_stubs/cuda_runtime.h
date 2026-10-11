#ifndef X_TEST_RUNTIME_H
#define X_TEST_RUNTIME_H
#include "backend.h"
enum cudaError_t { cudaSuccess = 0, cudaErrorInvalidValue = 1, cudaErrorTest = 8 };
using cudaEvent_t = struct test_cuda_event*;
using cudaStream_t = struct test_cuda_stream*;
// A fixed underlying type permits well-defined future/unknown-value injection.
enum cudaMemoryType : unsigned int {
  cudaMemoryTypeUnregistered, cudaMemoryTypeHost, cudaMemoryTypeDevice, cudaMemoryTypeManaged
};
struct cudaPointerAttributes { cudaMemoryType type; };
inline constexpr unsigned int cudaEventDisableTiming = 2;
inline const char* cudaGetErrorString(cudaError_t error)
{
  ++runtime.message_calls;
  runtime.last_error = static_cast<int>(error);
  return runtime.null_message ? nullptr
      : runtime.message == nullptr ? "runtime:test" : runtime.message;
}
inline cudaError_t cudaEventCreateWithFlags(cudaEvent_t* handle, unsigned int flags)
{
  runtime.last_flags = flags;
  ++runtime.creates;
  if (runtime.creates == runtime.fail_create_at) {
    *handle = reinterpret_cast<cudaEvent_t>(UINTPTR_MAX);
    return cudaErrorTest;
  }
  ++runtime.live;
  *handle = reinterpret_cast<cudaEvent_t>(static_cast<std::uintptr_t>(runtime.creates));
  return cudaSuccess;
}
inline cudaError_t cudaEventDestroy(cudaEvent_t handle)
{
  if (reinterpret_cast<std::uintptr_t>(handle) == UINTPTR_MAX) { std::abort(); }
  ++runtime.destroys;
  if (runtime.destroys == runtime.fail_destroy_at) { return cudaErrorTest; }
  --runtime.live;
  return cudaSuccess;
}
inline cudaError_t cudaEventRecordWithFlags(
    cudaEvent_t event, cudaStream_t stream, unsigned int flags)
{
  ++runtime.records;
  runtime.last_record_event = event;
  runtime.last_stream = stream;
  runtime.last_flags = flags;
  return static_cast<cudaError_t>(runtime.record_error);
}
inline cudaError_t cudaEventSynchronize(cudaEvent_t event)
{
  ++runtime.synchronizes;
  runtime.last_sync_event = event;
  return static_cast<cudaError_t>(runtime.sync_error);
}
inline cudaError_t cudaEventElapsedTime(
    float* duration, cudaEvent_t start, cudaEvent_t stop)
{
  ++runtime.elapsed_calls;
  runtime.last_elapsed_start = start;
  runtime.last_elapsed_stop = stop;
  *duration = runtime.milliseconds;
  return static_cast<cudaError_t>(runtime.elapsed_error);
}
inline cudaError_t cudaMemGetInfo(std::size_t* available, std::size_t* total)
{
  ++runtime.memory_queries;
  if (available == nullptr || total == nullptr || available == total) {
    std::abort();
  }
  *available = runtime.query_error ? 999 : 128;
  *total = runtime.query_error ? 777 : 256;
  return static_cast<cudaError_t>(runtime.query_error);
}
inline cudaError_t cudaPointerGetAttributes(cudaPointerAttributes* output, const void* pointer)
{
  ++runtime.pointer_queries;
  runtime.last_pointer = pointer;
  output->type = static_cast<cudaMemoryType>(runtime.memory_type);
  return static_cast<cudaError_t>(runtime.query_error);
}
inline cudaError_t cudaGetDeviceCount(int* count)
{
  ++runtime.device_queries;
  *count = runtime.devices;
  return static_cast<cudaError_t>(runtime.query_error);
}
#endif
