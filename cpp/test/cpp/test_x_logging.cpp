#include "x.h"


int main()
{
  float a{x_Pi<float>};
  int b{x_Pi<int>};

  char buf[128]{0};
  _x_log_parse(buf, 128, "Hello {}, world {}, {} bar", a, b, "foo");
  x_log('i', nullptr, "%s", buf);

  memset(buf, 0, 128);

  _x_log_parse(buf, 128, "Hello {}, world {}, foo {}", a, b, std::string("bar"));
  x_log('i', nullptr, "%s", buf);

  x_log('w', nullptr, "Hello world! %p", &a);
  x_log('w', nullptr, "Hello world! {}", &a);
  x_log('i', nullptr, "float: %.5f, int: %d", a, b);
  x_log('i', nullptr, "float: {:.5}, int: {}", a, b);
  x_log('i', nullptr, "float: {}, int: {}", a, b);
  x_log('i', nullptr, "float: {, int: {}", a, b);

  return 0;
}
