#include "x.h"


int main(int argc, char** argv)
{
  x_event_stats stats;
  const char* unit{"ms"};

  // CUDA driver API event
  cuInit(0);

  CUdevice device{0};
  cuDeviceGet(&device, 0);

  CUcontext context{nullptr};
  cuCtxCreate(&context, 0, device);

  x_event ttl_cu("cu");
  x_event avg_cu("cu");

  ttl_cu.tic();
  x_log('i', nullptr, "[cu] x_event starts.");

  for (size_t i = 0; i < 5; ++i) {
    avg_cu.tic();
    x_sleep(1000);
    avg_cu.toc(stats, 3);
  }
  if (stats.ready) {
    stats.echo(unit);
  }

  stats.reset();

  for (size_t i = 0; i < 5; ++i) {
    avg_cu.tic();
    x_sleep(1000);
    avg_cu.toc(stats, 5);
  }
  if (stats.ready) {
    stats.echo(unit);
  }

  ttl_cu.toc();
  x_log('i', nullptr, "[cu] x_event stops.", ttl_cu.duration(unit));
  x_log('p', nullptr, "[cu] Total: %fms", ttl_cu.duration(unit));

  cuCtxDestroy(context);

  stats.reset();
  printf("\n");

  // CUDA runtime API event
  x_event ttl_cuda("cuda");
  x_event avg_cuda("cuda");

  ttl_cuda.tic();
  x_log('i', nullptr, "[cuda] x_event starts.");

  for (size_t i = 0; i < 5; ++i) {
    avg_cuda.tic();
    x_sleep(1000);
    avg_cuda.toc(stats, 3);
  }
  if (stats.ready) {
    stats.echo(unit);
  }

  stats.reset();

  for (size_t i = 0; i < 5; ++i) {
    avg_cuda.tic();
    x_sleep(1000);
    avg_cuda.toc(stats, 5);
  }
  if (stats.ready) {
    stats.echo(unit);
  }

  ttl_cuda.toc();
  x_log('i', nullptr, "[cuda] x_event stops.", ttl_cuda.duration(unit));
  x_log('p', nullptr, "[cuda] Total: %fms", ttl_cuda.duration(unit));

  return 0;
}
