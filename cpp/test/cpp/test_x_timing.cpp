#include "x.h"


int main(int argc, char** argv)
{
  x_timing ttl;
  x_timing avg;

  ttl.tic<true>();

  for (size_t i = 0; i < 5; ++i) {
    avg.tic();
    x_sleep(1000);
    avg.toc<true>("ms", 5, "Average");
  }

  ttl.toc<true>("ms");
  x_log('p', nullptr, "Total: %fms", ttl.elapsed());

  return 0;
}
