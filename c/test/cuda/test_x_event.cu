#include "x.h"


int main(int argc, char** argv)
{
  x_event_stats stats;
  x_event_stats_init(&stats);
  const char* unit = "ms";

  // CUDA driver API event
  cuInit(0);

  CUdevice device = 0;
  cuDeviceGet(&device, 0);

  CUcontext context{nullptr};
  cuCtxCreate(&context, 0, device);

  x_event ttl_cu;
  x_event avg_cu;
  x_event_init(&ttl_cu, "cu", 0);
  x_event_init(&avg_cu, "cu", 0);

  ttl_cu.tic(&ttl_cu, NULL, 0);
  x_log('i', nullptr, "[cu] x_event starts.");

  for (size_t i = 0; i < 5; ++i) {
    avg_cu.tic(&avg_cu, NULL, 0);
    x_sleep(1000);
    avg_cu.toc_stats(&avg_cu, &stats, 3, NULL, 0);
  }
  if (stats.ready) {
    stats.echo(&stats, unit, NULL, NULL);
  }

  stats.reset(&stats);

  for (size_t i = 0; i < 5; ++i) {
    avg_cu.tic(&avg_cu, NULL, 0);
    x_sleep(1000);
    avg_cu.toc_stats(&avg_cu, &stats, 5, NULL, 0);
  }
  if (stats.ready) {
    stats.echo(&stats, unit, NULL, NULL);
  }

  ttl_cu.toc(&ttl_cu, NULL, 0);
  x_log('i', nullptr, "[cu] x_event stops.", ttl_cu.duration(&ttl_cu, unit));
  x_log('p', nullptr, "[cu] Total: %fms", ttl_cu.duration(&ttl_cu, unit));

  cuCtxDestroy(context);

  stats.reset(&stats);
  printf("\n");

  // CUDA runtime API event
  x_event ttl_cuda;
  x_event avg_cuda;
  x_event_init(&ttl_cuda, "cuda", 0);
  x_event_init(&avg_cuda, "cuda", 0);

  ttl_cuda.tic(&ttl_cuda, NULL, 0);
  x_log('i', nullptr, "[cuda] x_event starts.");

  for (size_t i = 0; i < 5; ++i) {
    avg_cuda.tic(&avg_cuda, NULL, 0);
    x_sleep(1000);
    avg_cuda.toc_stats(&avg_cuda, &stats, 3, NULL, 0);
  }
  if (stats.ready) {
    stats.echo(&stats, unit, NULL, NULL);
  }

  stats.reset(&stats);

  for (size_t i = 0; i < 5; ++i) {
    avg_cuda.tic(&avg_cuda, NULL, 0);
    x_sleep(1000);
    avg_cuda.toc_stats(&avg_cuda, &stats, 5, NULL, 0);
  }
  if (stats.ready) {
    stats.echo(&stats, unit, NULL, NULL);
  }

  ttl_cuda.toc(&ttl_cuda, NULL, 0);
  x_log('i', nullptr, "[cuda] x_event stops.", ttl_cuda.duration(&ttl_cuda, unit));
  x_log('p', nullptr, "[cuda] Total: %fms", ttl_cuda.duration(&ttl_cuda, unit));

  return 0;
}
