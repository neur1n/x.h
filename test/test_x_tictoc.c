#include "x.h"


int main(int argc, char** argv)
{
  char report[128] = {0};
  size_t rsz = x_count(report) * sizeof(char);

  x_tictoc ttl = {0};
  x_tictoc avg = {0};

  x_tictoc_init(&ttl);
  x_tictoc_init(&avg);

  x_tic(&ttl, true);

  for (size_t i = 0; i < 5; ++i) {
    x_tic(&avg, false);
    x_sleep(1000);
    x_toc_ex(&avg, "ms", 5, "REPORT", report, &rsz);
  }

  x_toc(&ttl, "ms", false);

  x_log('p', NULL, "Total: %fms", ttl.elapsed);
  x_log('p', NULL, "%s", report);

  return 0;
}
