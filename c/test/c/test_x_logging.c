#include "x.h"


int main()
{
  float a = x_Pi(float);
  int b = x_Pi(int);

  x_log('w', NULL, "Hello world!");
  x_log('i', NULL, "float: %.5f, int: %d", a, b);

  return 0;
}
