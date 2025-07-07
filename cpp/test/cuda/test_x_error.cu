#include "x.h"


int main(int argc, char** argv)
{
  x_error err;

  double* ptr{nullptr};

  x_log('i', nullptr, "%s", err.msg());

  // CUDA driver API error
  cuInit(0);

  CUdevice device{0};
  cuDeviceGet(&device, 0);

  CUcontext context{nullptr};
  cuCtxCreate(&context, 0, device);

  err.set("cu", CUDA_ERROR_OUT_OF_MEMORY);
  x_log('e', nullptr, "[CU] %s", err.msg());

  cuCtxDestroy(context);

  // CUDA runtime API error
  err.set("cuda", cudaErrorMemoryAllocation);
  x_log('e', nullptr, "[CUDA] %s", err.msg());

  x_check("cuda", err, cudaMalloc<double>, &ptr, sizeof(double));
  if (err) {
    x_log('e', nullptr, "[CUDA] cudaMalloc error.");
  } else {
    x_log('i', nullptr, "[CUDA] cudaMalloc success.");
  }

  x_check("cuda", err, cudaFree, ptr);
  if (err) {
    x_log('e', nullptr, "[CUDA] cudaFree error.");
  } else {
    x_log('i', nullptr, "[CUDA] cudaFree success.");
  }

  ptr = nullptr;

  auto faulty = [](double** ptr, size_t size) -> x_error {
    return x_error("cuda", cudaErrorMemoryAllocation);
  };
  x_check("cuda", err, faulty, &ptr, sizeof(double));
  if (err) {
    x_log('e', nullptr, "[CUDA] Faulty lambda error.");
  } else {
    x_log('i', nullptr, "[CUDA] Faulty lambda success.");
  }

  // cuBLAS API error
  err.set("cublas", CUBLAS_STATUS_NOT_INITIALIZED);
  x_log('e', nullptr, "[cuBLAS] %s", err.msg());

  return 0;
}
