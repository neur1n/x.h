#include "x.h"


int main(int argc, char** argv)
{
  x_event_stats stats;
  const char* unit{"ms"};

  x_event ttl("cpu");
  x_event avg("cpu");

  ttl.tic();
  x_log('i', nullptr, "[cpu] x_event starts.");

  for (size_t i = 0; i < 5; ++i) {
    avg.tic();
    x_sleep(1000);
    avg.toc(stats, 3);
  }
  if (stats.ready) {
    stats.echo(unit);
  }

  stats.reset();

  for (size_t i = 0; i < 5; ++i) {
    avg.tic();
    x_sleep(1000);
    avg.toc(stats, 5);
  }
  if (stats.ready) {
    stats.echo(unit);
  }

  ttl.toc();
  x_log('i', nullptr, "[cpu] x_event stops.", ttl.duration(unit));
  x_log('p', nullptr, "[cpu] Total: %fms", ttl.duration(unit));

  return 0;
}
