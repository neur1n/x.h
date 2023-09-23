#include "x.h"


static const char* msg_fn(char* msg, const size_t msz, int32_t val)
{
  if (val == 0) {
    strcpy(msg, "msg_fn msg 0");
  } else {
    strcpy(msg, "msg_fn msg 1");
  }

  return msg;
}

static const char* msg_literal()
{
  return "literal msg 1";
}

int main()
{
  char msg[32] = {0};
  size_t msz = x_count(msg);

  x_err err = x_ok();

  err = x_err_set(x_err_posix, EINVAL);
  x_log('e', NULL, "%s", x_err_msg(msg, msz, err));

  err = x_err_set(x_err_custom, 99);
  x_log('e', NULL, "%s", x_err_msg(msg, msz, err, x_err_msg_literal, "literal msg 0"));

  err = x_err_set(x_err_custom, 999);
  x_log('e', NULL, "%s", x_err_msg(msg, msz, err, x_err_msg_literal, msg_literal()));

  err = x_err_set(x_err_custom, 0);
  x_log('e', NULL, "%s", x_err_msg(msg, msz, err, x_err_msg_lookup, msg_fn));

  err = x_err_set(x_err_custom, 1);
  x_log('e', NULL, "%s", x_err_msg(msg, msz, err, x_err_msg_lookup, msg_fn));

  return 0;
}
