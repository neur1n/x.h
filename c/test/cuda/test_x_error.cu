#include "x.h"


int main(int argc, char** argv)
{
  // NOTE: Then length of the buffer may not be enough and it is intended to
  // be truncated for testing purposes.
  char msg[32] = {0};
  size_t msz = x_count(msg);

  x_error err = x_ok();

  double* ptr{NULL};

  x_error_msg(msg, msz, err);
  x_log('i', NULL, "%s", msg);

  // CUDA driver API error
  cuInit(0);

  CUdevice device{0};
  cuDeviceGet(&device, 0);

  CUcontext context{NULL};
  cuCtxCreate(&context, 0, device);

  err = x_error_set("cu", CUDA_ERROR_OUT_OF_MEMORY);
  x_error_msg(msg, msz, err);
  x_log('e', NULL, "%s", msg);

  cuCtxDestroy(context);

  // CUDA runtime API error
  err = x_error_set("cuda", cudaErrorMemoryAllocation);
  x_error_msg(msg, msz, err);
  x_log('e', NULL, "%s", msg);

  x_check("cuda", err, cudaMalloc<double>, &ptr, sizeof(double));
  x_check("cuda", err, cudaFree, ptr);
  ptr = NULL;

  auto faulty = [](double** ptr, size_t size) -> int {
    return cudaErrorMemoryAllocation;
  };
  x_check("cuda", err, faulty, &ptr, sizeof(double));

  return 0;
}
