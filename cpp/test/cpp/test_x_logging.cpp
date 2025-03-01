#include "x.h"


int main()
{
  float a{x_Pi<float>};
  int b{x_Pi<int>};

  char buf[128]{0};

  x_fmt(buf, 128, "Hello {}, world {}, {} bar", a, b, "foo");
  x_log('i', nullptr, "%s", buf);

  memset(buf, 0, 128);

  x_fmt(buf, 128, "Hello {}, world {}, foo {}", a, b, std::string("bar"));
  x_log('i', nullptr, buf);

  memset(buf, 0, 128);

  x_log('w', nullptr, "Hello world! %p", &a);

  // NOTE: This cannot work with `std::vformat`.
  x_log('w', nullptr, x_fmt(buf, 128, "Hello world! {}", &a));

  x_log('i', nullptr, "float: %.5f, int: %d", a, b);

  // NOTE (2025-03-01): With C++20, the followiing will be formatted properly.
  x_log('i', nullptr, x_fmt(buf, 128, "float: {:.5}, int: {}", a, b));

  x_log('d', nullptr, "Hello world!");
  x_log('d', nullptr, "Hello world!", a, b);

  return 0;
}
