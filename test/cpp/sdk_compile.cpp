// Compile against real CUDA Toolkit declarations, without running a GPU test.
#include "../../cpp/x.h"

void x_cpp_cuda_sdk_compile_check(CUdeviceptr driver_pointer, const float* pointer)
{
  x_event driver_event{"cudadrv"};
  x_event runtime_event{"cudart"};
  (void)driver_event.tic();
  (void)driver_event.toc();
  (void)runtime_event.tic();
  (void)runtime_event.toc();
  (void)driver_event.dur();
  (void)runtime_event.dur();
  (void)driver_event.status();
  (void)runtime_event.status();
  (void)x_memtype("cudadrv", driver_pointer);
  (void)x_memtype("cudart", pointer);
  (void)x_memtype("cudart", nullptr);
  size_t available{0}, total{0};
  (void)x_meminfo("cudadrv", &available, &total);
  (void)x_meminfo("cudart", &available, &total);
  x_error error;
  x_check("cudadrv", error, cuInit(0));
  (void)x_ngpu("cudadrv");
  (void)x_ngpu("cudart");
}
