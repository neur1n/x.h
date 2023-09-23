#include "x.h"


int main()
{
  char msg[32] = {0};
  size_t msz = x_count(msg);

  x_err err = x_ok();

  err = x_err_set(x_err_cuda, 700);
  x_log('e', NULL, "%s", x_err_msg(msg, msz, err));

  return 0;
}
