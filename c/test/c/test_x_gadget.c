#include "x.h"


int main(int argc, char** argv)
{
  const uint64_t m = 1536;
  const uint64_t n = 1024;

  uint64_t gcd = x_gcd(m, n);
  uint64_t lcm = x_lcm(m, n);

  x_log('d', NULL, "gcd: %llu, lcm: %llu", x_gcd(m, n), x_lcm(m, n));
  x_log('i', NULL, "gcd: %llu, lcm: %llu", gcd, lcm);

  char msg[64] = {0};
  snprintf(msg, 64, "gcd = %llu", gcd);
  x_assert(gcd == 64, msg);
  // x_assert(gcd == 64, "");

  return 0;
}
