#ifndef X_TEST_MATHEMATICS_CASES_H
#define X_TEST_MATHEMATICS_CASES_H

// Shared behavioral cases for the independently maintained C/C++ implementations.
// Include the distribution and define CHECK before including this file.

typedef uint64_t (*binary_function)(uint64_t, uint64_t);
typedef struct {
  binary_function function;
  uint64_t first;
  uint64_t second;
  uint64_t expected;
  int error;
} math_case;

static void check_cases(void)
{
  const uint64_t high_bit = UINT64_C(9223372036854775808);
  const uint64_t beyond_double = UINT64_C(9007199254740993);
  const uint64_t three_40 = UINT64_C(12157665459056928801);
  const math_case cases[] = {
    {x_gcd, 0, 0, 0, 0},
    {x_gcd, 0, UINT64_MAX, UINT64_MAX, 0},
    {x_gcd, UINT64_MAX, 0, UINT64_MAX, 0},
    {x_gcd, 1536, 1024, 512, 0},
    {x_gcd, UINT64_MAX, UINT64_MAX - 1, 1, 0},
    {x_lcm, 0, 0, 0, 0},
    {x_lcm, 0, UINT64_MAX, 0, 0},
    {x_lcm, UINT64_MAX, 0, 0, 0},
    {x_lcm, 1536, 1024, 3072, 0},
    {x_lcm, 1, UINT64_MAX, UINT64_MAX, 0},
    {x_lcm, UINT64_MAX, UINT64_MAX, UINT64_MAX, 0},
    {x_lcm, high_bit, 2, high_bit, 0},
    {x_lcm, 2, UINT64_MAX, 0, ERANGE},
    {x_lcm, UINT64_MAX, 2, 0, ERANGE},
    {x_lcm, high_bit, 3, 0, ERANGE},
    {x_next_exp, 0, 0, 0, EINVAL},
    {x_next_exp, 0, 7, 0, EINVAL},
    {x_next_exp, 1, 0, 0, EINVAL},
    {x_next_exp, 1, 1, 0, EINVAL},
    {x_next_exp, 1, 7, 0, EINVAL},
    {x_next_exp, 2, 0, 1, 0},
    {x_next_exp, 2, 1, 1, 0},
    {x_next_exp, 2, 8, 8, 0},
    {x_next_exp, 2, 9, 16, 0},
    {x_next_exp, 2, high_bit - 1, high_bit, 0},
    {x_next_exp, 2, high_bit, high_bit, 0},
    {x_next_exp, 2, high_bit + 1, 0, ERANGE},
    {x_next_exp, 2, UINT64_MAX, 0, ERANGE},
    {x_next_exp, 3, 8, 9, 0},
    {x_next_exp, 3, 9, 9, 0},
    {x_next_exp, 3, 10, 27, 0},
    {x_next_exp, 3, three_40 - 1, three_40, 0},
    {x_next_exp, 3, three_40, three_40, 0},
    {x_next_exp, 3, three_40 + 1, 0, ERANGE},
    {x_next_exp, 10, UINT64_C(10000000000000000000),
        UINT64_C(10000000000000000000), 0},
    {x_next_exp, 10, UINT64_MAX, 0, ERANGE},
    {x_next_exp, beyond_double - 1, beyond_double, 0, ERANGE},
    {x_next_exp, UINT64_MAX, 0, 1, 0},
    {x_next_exp, UINT64_MAX, 2, UINT64_MAX, 0},
    {x_next_exp, UINT64_MAX, UINT64_MAX, UINT64_MAX, 0},
    {x_prev_exp, 0, 0, 0, EINVAL},
    {x_prev_exp, 0, 7, 0, EINVAL},
    {x_prev_exp, 1, 0, 0, EINVAL},
    {x_prev_exp, 1, 1, 0, EINVAL},
    {x_prev_exp, 1, 7, 0, EINVAL},
    {x_prev_exp, 2, 0, 0, 0},
    {x_prev_exp, 2, 1, 1, 0},
    {x_prev_exp, 2, 8, 8, 0},
    {x_prev_exp, 2, 9, 8, 0},
    {x_prev_exp, 2, high_bit - 1, high_bit / 2, 0},
    {x_prev_exp, 2, high_bit, high_bit, 0},
    {x_prev_exp, 2, UINT64_MAX, high_bit, 0},
    {x_prev_exp, 3, 8, 3, 0},
    {x_prev_exp, 3, 9, 9, 0},
    {x_prev_exp, 3, 10, 9, 0},
    {x_prev_exp, 3, three_40 - 1, three_40 / 3, 0},
    {x_prev_exp, 3, three_40, three_40, 0},
    {x_prev_exp, 3, UINT64_MAX, three_40, 0},
    {x_prev_exp, beyond_double - 1, beyond_double, beyond_double - 1, 0},
    {x_prev_exp, UINT64_MAX, 0, 0, 0},
    {x_prev_exp, UINT64_MAX, UINT64_MAX - 1, 1, 0},
    {x_prev_exp, UINT64_MAX, UINT64_MAX, UINT64_MAX, 0},
    {x_next_mul, 0, 0, 0, EINVAL},
    {x_next_mul, 0, 7, 0, EINVAL},
    {x_next_mul, 1, 0, 1, 0},
    {x_next_mul, 1, UINT64_MAX - 1, UINT64_MAX, 0},
    {x_next_mul, 1, UINT64_MAX, 0, ERANGE},
    {x_next_mul, 2, 8, 10, 0},
    {x_next_mul, 2, 9, 10, 0},
    {x_next_mul, 2, UINT64_MAX - 2, UINT64_MAX - 1, 0},
    {x_next_mul, 2, UINT64_MAX - 1, 0, ERANGE},
    {x_next_mul, 2, UINT64_MAX, 0, ERANGE},
    {x_next_mul, UINT64_MAX, 0, UINT64_MAX, 0},
    {x_next_mul, UINT64_MAX, UINT64_MAX - 1, UINT64_MAX, 0},
    {x_next_mul, UINT64_MAX, UINT64_MAX, 0, ERANGE},
    {x_prev_mul, 0, 0, 0, EINVAL},
    {x_prev_mul, 0, 7, 0, EINVAL},
    {x_prev_mul, 1, 0, 0, 0},
    {x_prev_mul, 1, UINT64_MAX, UINT64_MAX, 0},
    {x_prev_mul, 2, 0, 0, 0},
    {x_prev_mul, 2, 1, 0, 0},
    {x_prev_mul, 2, 8, 8, 0},
    {x_prev_mul, 2, 9, 8, 0},
    {x_prev_mul, 2, UINT64_MAX, UINT64_MAX - 1, 0},
    {x_prev_mul, UINT64_MAX, UINT64_MAX - 1, 0, 0},
    {x_prev_mul, UINT64_MAX, UINT64_MAX, UINT64_MAX, 0},
  };

  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
    // Check both ordinary zero-initialized errno and stale error preservation.
    for (int stale = 0; stale < 2; ++stale) {
      errno = stale ? EDOM : 0;
      const uint64_t actual = cases[i].function(cases[i].first, cases[i].second);
      const int actual_error = errno;
      if (actual != cases[i].expected
          || actual_error != (cases[i].error ? cases[i].error : (stale ? EDOM : 0))) {
        fprintf(stderr, "Math case %zu failed (stale=%d, errno=%d)\n",
            i, stale, actual_error);
        exit(EXIT_FAILURE);
      }
    }
  }
}

static void check_small_domains(void)
{
  // Independent brute-force searches check nearest-value semantics.
  for (uint64_t base = 1; base <= 16; ++base) {
    for (uint64_t source = 0; source <= 256; ++source) {
      uint64_t next = source + 1;
      while (next % base != 0) {
        ++next;
      }
      uint64_t previous = source;
      while (previous % base != 0) {
        --previous;
      }
      CHECK(x_next_mul(base, source) == next);
      CHECK(x_prev_mul(base, source) == previous);

      if (base >= 2) {
        uint64_t expected_next = 0;
        uint64_t expected_previous = 0;
        for (uint64_t candidate = 1; candidate <= 4096; ++candidate) {
          uint64_t remainder = candidate;
          while (remainder > 1 && remainder % base == 0) {
            remainder /= base;
          }
          if (remainder == 1) {
            if (candidate <= source) {
              expected_previous = candidate;
            }
            if (candidate >= source && expected_next == 0) {
              expected_next = candidate;
            }
          }
        }
        CHECK(x_next_exp(base, source) == expected_next);
        CHECK(x_prev_exp(base, source) == expected_previous);
      }
    }
  }

  for (uint64_t m = 1; m <= 32; ++m) {
    for (uint64_t n = 1; n <= 32; ++n) {
      uint64_t divisor = m < n ? m : n;
      while (m % divisor != 0 || n % divisor != 0) {
        --divisor;
      }
      uint64_t multiple = m > n ? m : n;
      while (multiple % m != 0 || multiple % n != 0) {
        ++multiple;
      }
      CHECK(x_gcd(m, n) == divisor);
      CHECK(x_lcm(m, n) == multiple);
    }
  }
}

#endif
