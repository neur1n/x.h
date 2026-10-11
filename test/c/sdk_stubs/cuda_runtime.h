/* Minimal substitute for the runtime error API, not CUDA utilities. */
#ifndef TEST_CUDA_RUNTIME_H
#define TEST_CUDA_RUNTIME_H
typedef enum { cudaSuccess = 0, cudaErrorTest = 1 } cudaError_t;
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
