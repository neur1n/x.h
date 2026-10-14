/* Minimal substitute for the runtime error API, not CUDA utilities. */
#ifndef TEST_CUDA_RUNTIME_H
#define TEST_CUDA_RUNTIME_H
typedef enum { cudaSuccess = 0, cudaErrorTest = 1 } cudaError_t;
#ifdef __cplusplus
inline cudaError_t test_cuda_last_error = cudaSuccess;
inline int test_cuda_capture_calls = 0;
#else
static cudaError_t test_cuda_last_error;
static int test_cuda_capture_calls;
#endif
#ifdef __cplusplus
inline
#else
static inline
#endif
cudaError_t cudaGetLastError(void)
{
  ++test_cuda_capture_calls;
  cudaError_t value = test_cuda_last_error;
  test_cuda_last_error = cudaSuccess;
  return value;
}
#ifdef __cplusplus
inline
#else
static inline
#endif
const char* cudaGetErrorString(cudaError_t value)
{
  return value == cudaSuccess ? "runtime:success" : "runtime:test";
}
#endif
