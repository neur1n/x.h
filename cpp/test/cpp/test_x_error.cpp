#include "x.h"


// NOTE: This can also be a lambda function.
bool custom_fail(const int32_t val)
{
  return val != 99;
}


int main(int argc, char** argv)
{
  x_error err;

  // NOTE: This will cause an exception.
  // err.set("custom", 98);

  err.set("custom", 99, custom_fail);
  x_log('d', nullptr, "[1] %s", err.msg());
  if (err) {
    x_log('e', nullptr, "[1] %s", err.msg());
  }

  err.set("custom", 100, "some error 100.", custom_fail);
  x_log('w', nullptr, "[2] %s", err.msg());
  if (err) {
    x_log('e', nullptr, "[2] fail: %s", err.msg());
  }

  err.set("posix");
  x_log('i', nullptr, "[4] %s", err.msg());

  errno = EINVAL;
  err.set("posix");
  x_log('f', nullptr, "[5] %s", err.msg());

  return 0;
}
