#include "x.h"


int main(int argc, char** argv)
{
  x_error err;
  char msg[64] = {0};

  double* ptr = NULL;

  err = x_malloc((void**)&ptr, sizeof(double));
  if (x_fail(err)) {
    x_error_msg(msg, 64, err);
    x_log('e', NULL, "x_malloc: %s", msg);
    return EXIT_FAILURE;
  }

  *ptr = x_Pi(double);
  x_log('i', NULL, "%f", *ptr);

  x_free(ptr);

  err = x_malloc((void**)&ptr, sizeof(double));
  if (x_fail(err)) {
    x_error_msg(msg, 64, err);
    x_log('e', NULL, "x_malloc: %s", msg);
    return EXIT_FAILURE;
  }

  *ptr = x_KiB(double, 2);
  x_log('i', NULL, "%f", *ptr);

  x_free(ptr);

  return 0;
}
