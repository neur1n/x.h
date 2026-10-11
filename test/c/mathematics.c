#define X_ERROR_DOMAIN_IMPLEMENTATION
#include "error_test_support.h"
#include "../mathematics_cases.h"

// Verify that the macros still work in integer constant expressions in C99.
typedef char bit_31_constant[(x_bit(31) == UINT64_C(2147483648)) ? 1 : -1];
typedef char bit_63_constant[(x_bit(63) == UINT64_C(9223372036854775808)) ? 1 : -1];
typedef char kib_constant[(x_KiB(uint64_t, 1 + 1) == 2048) ? 1 : -1];
typedef char pib_constant[(x_PiB(uint64_t, 1) == UINT64_C(1125899906842624)) ? 1 : -1];

int main(void)
{
  CHECK(x_bit(0) == 1);
  CHECK(x_bit(31) == UINT64_C(2147483648));
  CHECK(x_bit(63) == UINT64_C(9223372036854775808));
  unsigned int bit = 31;
  CHECK(x_bit(bit++) == UINT64_C(2147483648));
  CHECK(bit == 32);
  CHECK(x_KiB(uint64_t, 1 + 1) == UINT64_C(2048));
  CHECK(x_MiB(uint64_t, 1 + 1) == UINT64_C(2097152));
  CHECK(x_GiB(uint64_t, 1 + 1) == UINT64_C(2147483648));
  CHECK(x_TiB(uint64_t, 1 + 1) == UINT64_C(2199023255552));
  CHECK(x_PiB(uint64_t, 1 + 1) == UINT64_C(2251799813685248));
  CHECK(x_KiB(double, 3.0 / 2.0) == 1536.0);
  unsigned int scale = 1;
  CHECK(x_KiB(uint64_t, scale++) == UINT64_C(1024));
  CHECK(scale == 2);
  check_cases();
  check_small_domains();
  // Exercise the native platform query; a zero result is permitted.
  (void)x_ncpu();
  return 0;
}
