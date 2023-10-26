#include "x.h"


int main()
{
  x_err err;

  x_log('i', nullptr, "%s", err.msg());

  err.set(x_err_cuda, 700);
  x_log('e', nullptr, "%s", err.msg());

  return 0;
}
