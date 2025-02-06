#include "x.h"


int main(int argc, char** argv)
{
  x_error err;

  double* ptr{nullptr};

  err = x_malloc(&ptr, sizeof(double));
  if (err) {
    x_log('e', nullptr, "x_malloc: %s", err.msg());
    return EXIT_FAILURE;
  }

  *ptr = x_Pi<double>;
  x_log('i', nullptr, "%f", *ptr);

  x_free(ptr);

  err = x_malloc(&ptr, sizeof(double));
  if (err) {
    x_log('e', nullptr, "x_malloc: %s", err.msg());
    return EXIT_FAILURE;
  }

  *ptr = x_KiB<double>(2);
  x_log('i', nullptr, "%f", *ptr);

  x_free(ptr);

  return 0;
}
