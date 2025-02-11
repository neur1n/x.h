#include "x.h"


int main(int argc, char** argv)
{
  x_event_stats stats;
  x_event_stats_init(&stats);
  const char* unit = "ms";

  x_event ttl;
  x_event avg;
  x_event_init(&ttl, "cpu", 0);
  x_event_init(&avg, "cpu", 0);

  ttl.tic(&ttl, NULL, 0);
  x_log('i', NULL, "[cpu] x_event starts.");

  for (size_t i = 0; i < 5; ++i) {
    avg.tic(&avg, NULL, 0);
    x_sleep(1000);
    avg.toc_stats(&avg, &stats, 3, NULL, 0);
  }
  if (stats.ready) {
    stats.echo(&stats, unit, NULL, NULL);
  }

  stats.reset(&stats);

  for (size_t i = 0; i < 5; ++i) {
    avg.tic(&avg, NULL, 0);
    x_sleep(1000);
    avg.toc_stats(&avg, &stats, 5, NULL, 0);
  }
  if (stats.ready) {
    stats.echo(&stats, unit, NULL, NULL);
  }

  ttl.toc(&ttl, NULL, 0);
  x_log('i', NULL, "[cpu] x_event stops.", ttl.duration(&ttl, unit));
  x_log('p', NULL, "[cpu] Total: %fms", ttl.duration(&ttl, unit));

  return 0;
}
