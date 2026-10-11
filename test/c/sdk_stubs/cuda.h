/* Minimal substitute for the driver error API, not CUDA utilities. */
#ifndef TEST_CUDA_H
#define TEST_CUDA_H
typedef enum { CUDA_SUCCESS = 0, CUDA_ERROR_TEST = 1 } CUresult;
#ifdef __cplusplus
inline
#else
static inline
#endif
CUresult cuGetErrorString(CUresult value, const char** message)
{
  *message = value == CUDA_SUCCESS ? "driver:success" : "driver:test";
  return CUDA_SUCCESS;
}
#endif
