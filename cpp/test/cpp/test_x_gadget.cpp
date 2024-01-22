#include "x.h"


int main()
{
  const size_t m{1536};
  const size_t n{1024};

  constexpr size_t gcd = x_gcd(m, n);
  constexpr size_t lcm = x_lcm(m, n);

  x_log('d', nullptr, "gcd: %zu, lcm: %zu", x_gcd(m, n), x_lcm(m, n));
  x_log('i', nullptr, "gcd: %zu, lcm: %zu", gcd, lcm);

  return 0;
}
