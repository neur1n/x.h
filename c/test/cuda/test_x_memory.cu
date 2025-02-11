#include "x.h"


int main(int argc, char** argv)
{
  char msg[32] = {0};
  size_t msz = x_count(msg);

  x_error err = x_ok();

  // CUDA driver API error
  CUdeviceptr ptr{0};
  cuInit(0);

  CUdevice device{0};
  cuDeviceGet(&device, 0);

  CUcontext context{nullptr};
  cuCtxCreate(&context, 0, device);

  err = x_error_set("cuda", cuMemAlloc(&ptr, sizeof(double)));
  if (x_fail(err)) {
    x_error_msg(msg, msz, err);
    x_log('e', nullptr, "cuMemAllocHost: %s", msg);
  }

  x_log('i', nullptr, "ptr type: %s", x_memtype("cu", ptr));

  if (ptr != 0) {
    err = x_error_set("cuda", cuMemFree(ptr));
    if (x_fail(err)) {
      x_error_msg(msg, msz, err);
      x_log('e', nullptr, "cuMemFree: %s", msg);
    }
    ptr = 0;
  }

  cuCtxDestroy(context);

  // CUDA runtime API error
  double* mem{nullptr};

  err = x_error_set("cuda", cudaMalloc(&mem, sizeof(double)));
  if (x_fail(err)) {
    x_error_msg(msg, msz, err);
    x_log('e', nullptr, "cudaMalloc: %s", msg);
  }

  x_log('i', nullptr, "mem type: %s", x_memtype("cuda", mem));

  if (mem != nullptr) {
    err = x_error_set("cuda", cudaFree(mem));
    if (x_fail(err)) {
      x_error_msg(msg, msz, err);
      x_log('e', nullptr, "cudaFree: %s", msg);
    }
    mem = nullptr;
  }

  return 0;
}
