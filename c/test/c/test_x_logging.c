#include "x.h"


int main()
{
  float a = x_Pi(float);
  float b = x_Pi(int);

  x_log('w', NULL, "Hellow world!");
  x_log('i', NULL, "float: %.5f, %d", a, b);

  return 0;
}
