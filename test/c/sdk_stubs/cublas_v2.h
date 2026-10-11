/* Minimal substitute for the cuBLAS error API. */
#ifndef TEST_CUBLAS_H
#define TEST_CUBLAS_H
typedef enum { CUBLAS_STATUS_SUCCESS = 0, CUBLAS_STATUS_TEST = 1 } cublasStatus_t;
#ifdef __cplusplus
inline
#else
static inline
#endif
const char* cublasGetStatusString(cublasStatus_t value)
{
  return value == CUBLAS_STATUS_SUCCESS ? "cublas:success" : "cublas:test";
}
#endif
