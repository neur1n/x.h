#include "x.h"


int main()
{
  float a{x_Pi<float>};
  int b{x_Pi<int>};

  x_log('w', nullptr, "Hellow world!");
  x_log('i', nullptr, "float: %.5f, %d", a, b);
  x_log('i', nullptr, "float: {:.5}, {}", a, b);

  return 0;
}
