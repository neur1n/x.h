#include "x.h"


// NOTE: This can also be a lambda function.
bool custom_fail(const int32_t val)
{
  return val != 99;
}


int main(int argc, char** argv)
{
  char msg[64] = {0};
  size_t msz = x_count(msg);

  x_error err;

  // NOTE: This will cause an exception.
  // err = x_error_set("custom", 98);

  err = x_error_set("custom", 99, custom_fail);
  x_error_msg(msg, msz, err);
  x_log('d', NULL, "[1] %s", msg);
  if (x_fail(err)) {
    x_log('e', NULL, "[1] %s", msg);
  }

  err = x_error_set("custom", 100);
  x_strcpy(msg, msz, "some error 100.");
  err.fail = custom_fail;
  x_log('w', NULL, "[2] %s", msg);
  if (x_fail(err)) {
    x_log('e', NULL, "[2] fail: %s", msg);
  }

  err = x_error_set("posix");
  x_error_msg(msg, msz, err);
  x_log('i', NULL, "[4] %s", msg);

  errno = EINVAL;
  err = x_error_set("posix");
  x_error_msg(msg, msz, err);
  x_log('f', NULL, "[5] %s", msg);

  return 0;
}
