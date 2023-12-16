#include "x.h"


int main()
{
  char msg[32] = {0};
  size_t msz = x_count(msg);

  x_err err = x_ok();

  err = x_err_set(x_err_custom, 99);
  x_log('d', NULL, "%s", x_err_msg(msg, msz, err));

  err = x_err_set(x_err_custom, 99);
  x_log('d', NULL, "%s", "some error 99");
  if (x_fail(err.val)) {
    x_log('d', NULL, "fail");
  } else {
    x_log('d', NULL, "succ");
  }

  err = x_err_set(x_err_posix);
  x_log('i', NULL, "%s", x_err_msg(msg, msz, err));

  err = x_err_set(x_err_posix, EINVAL);
  x_log('e', NULL, "%s", x_err_msg(msg, msz, err));

  return 0;
}
