#if TEST_GENERATED
#include "generated_under_test.h"
#else
#include "../../cpp/x.h"
#endif
#include <initializer_list>
#include "support.h"
#include "../mathematics_cases.h"

static_assert(x_Pi<float> == static_cast<float>(x_Pi<long double>));
static_assert(x_Pi<double> == static_cast<double>(x_Pi<long double>));
static_assert(x_Pi<long double>
    == 3.141592653589793238462643383279502884197169399375L);
static_assert(std::is_same_v<decltype(x_KiB(1)), int>);
static_assert(std::is_same_v<decltype(x_TiB<uint64_t>(1)), uint64_t>);
static_assert(std::is_same_v<decltype(x_KiB(1.5)), double>);
static_assert(std::is_same_v<decltype(x_bit(uint8_t{63})), uint64_t>);
static_assert(x_KiB<uint64_t>(1 + 1) == UINT64_C(2048));
static_assert(x_MiB<uint64_t>(1 + 1) == UINT64_C(2097152));
static_assert(x_GiB<uint64_t>(1 + 1) == UINT64_C(2147483648));
static_assert(x_TiB<uint64_t>(1 + 1) == UINT64_C(2199023255552));
static_assert(x_PiB<uint64_t>(1 + 1) == UINT64_C(2251799813685248));
static_assert(x_KiB(1.5) == 1536.0);
static_assert(x_bit(0) == 1);
static_assert(x_bit(31) == UINT64_C(2147483648));
static_assert(x_bit(63) == UINT64_C(9223372036854775808));

template<typename T>
constexpr bool check_constexpr()
{
  constexpr T maximum = (std::numeric_limits<T>::max)();
  constexpr T power = static_cast<T>(
      UINT64_C(1) << (std::numeric_limits<T>::digits - 1));
  static_assert(std::is_same_v<decltype(x_gcd(T{1}, T{1})), T>);
  static_assert(std::is_same_v<decltype(x_lcm(T{1}, T{1})), T>);
  static_assert(std::is_same_v<decltype(x_next_exp(T{2}, T{1})), T>);
  static_assert(std::is_same_v<decltype(x_prev_exp(T{2}, T{1})), T>);
  static_assert(std::is_same_v<decltype(x_next_mul(T{1}, T{1})), T>);
  static_assert(std::is_same_v<decltype(x_prev_mul(T{1}, T{1})), T>);
  return x_gcd(T{0}, T{0}) == 0
      && x_gcd(T{24}, T{18}) == 6
      && x_gcd(maximum, T{0}) == maximum
      && x_lcm(T{12}, T{18}) == 36
      && x_lcm(maximum, maximum) == maximum
      && x_lcm(T{0}, maximum) == 0
      && x_next_exp(T{2}, T{0}) == 1
      && x_next_exp(T{3}, T{10}) == 27
      && x_next_exp(T{2}, power) == power
      && x_prev_exp(T{2}, T{0}) == 0
      && x_prev_exp(T{3}, T{10}) == 9
      && x_prev_exp(T{2}, maximum) == power
      && x_next_mul(T{3}, T{6}) == 9
      && x_next_mul(maximum, T{0}) == maximum
      && x_prev_mul(T{3}, T{6}) == 6
      && x_prev_mul(maximum, maximum) == maximum;
}

template<typename T>
static void check_type()
{
  static_assert(check_constexpr<T>());
  using function = T (*)(T, T);
  struct test_case {
    function call;
    T first, second, expected;
    int error;
  };
  const T maximum = (std::numeric_limits<T>::max)();
  const T power = static_cast<T>(
      UINT64_C(1) << (std::numeric_limits<T>::digits - 1));
  const T before_maximum = static_cast<T>(maximum - 1);
  const test_case cases[] = {
    {x_gcd, 0, 0, 0, 0},
    {x_gcd, maximum, 0, maximum, 0},
    {x_gcd, maximum, before_maximum, 1, 0},
    {x_lcm, maximum, maximum, maximum, 0},
    {x_lcm, 0, maximum, 0, 0},
    {x_lcm, maximum, 0, 0, 0},
    {x_lcm, maximum, 2, 0, ERANGE},
    {x_lcm, 2, maximum, 0, ERANGE},
    {x_next_exp, 0, 0, 0, EINVAL},
    {x_next_exp, 1, 0, 0, EINVAL},
    {x_next_exp, 2, 0, 1, 0},
    {x_next_exp, 2, power, power, 0},
    {x_next_exp, 2, static_cast<T>(power + 1), 0, ERANGE},
    {x_next_exp, 2, maximum, 0, ERANGE},
    {x_next_exp, maximum, maximum, maximum, 0},
    {x_prev_exp, 0, 0, 0, EINVAL},
    {x_prev_exp, 1, 0, 0, EINVAL},
    {x_prev_exp, 2, 0, 0, 0},
    {x_prev_exp, 2, maximum, power, 0},
    {x_prev_exp, maximum, before_maximum, 1, 0},
    {x_next_mul, 0, 0, 0, EINVAL},
    {x_next_mul, 1, before_maximum, maximum, 0},
    {x_next_mul, 1, maximum, 0, ERANGE},
    {x_next_mul, maximum, 0, maximum, 0},
    {x_next_mul, maximum, before_maximum, maximum, 0},
    {x_next_mul, maximum, maximum, 0, ERANGE},
    {x_prev_mul, 0, 0, 0, EINVAL},
    {x_prev_mul, 1, maximum, maximum, 0},
    {x_prev_mul, maximum, before_maximum, 0, 0},
    {x_prev_mul, maximum, maximum, maximum, 0},
  };
  for (const auto& test : cases) {
    for (const int stale : {0, EDOM}) {
      errno = stale;
      const T actual = test.call(test.first, test.second);
      const int error = errno;
      CHECK(actual == test.expected);
      CHECK(error == (test.error == 0 ? stale : test.error));
    }
  }

  if constexpr (std::is_signed_v<T>) {
    const function functions[] = {
      x_gcd, x_lcm, x_next_exp, x_prev_exp, x_next_mul, x_prev_mul,
    };
    for (const auto call : functions) {
      for (const T negative : {T{-1}, (std::numeric_limits<T>::min)()}) {
        for (const T other : {T{0}, T{2}, maximum}) {
          errno = EDOM;
          const T first = call(negative, other);
          const int first_error = errno;
          CHECK(first == 0 && first_error == EINVAL);
          errno = EDOM;
          const T second = call(other, negative);
          const int second_error = errno;
          CHECK(second == 0 && second_error == EINVAL);
        }
      }
    }
  }
}

int main()
{
  check_cases();
  check_small_domains();
  check_type<int8_t>();
  check_type<uint8_t>();
  check_type<int16_t>();
  check_type<uint16_t>();
  check_type<int32_t>();
  check_type<uint32_t>();
  check_type<int64_t>();
  check_type<uint64_t>();

  unsigned int bit = 31;
  errno = EDOM;
  const uint64_t mask = x_bit(bit++);
  const int error = errno;
  CHECK(mask == UINT64_C(2147483648) && bit == 32 && error == EDOM);
  using capacity_function = uint64_t (*)(uint64_t);
  const capacity_function helpers[] = {x_KiB, x_MiB, x_GiB, x_TiB, x_PiB};
  const uint64_t units[] = {
    UINT64_C(1024), UINT64_C(1048576), UINT64_C(1073741824),
    UINT64_C(1099511627776), UINT64_C(1125899906842624),
  };
  for (size_t i = 0; i < sizeof(helpers) / sizeof(helpers[0]); ++i) {
    uint64_t scale = 2;
    errno = EDOM;
    const uint64_t result = helpers[i](scale++);
    const int capacity_error = errno;
    CHECK(result == 2 * units[i] && scale == 3 && capacity_error == EDOM);
  }
  uint64_t scale = 2;
  CHECK(x_GiB(scale) == UINT64_C(2147483648));
  CHECK(x_KiB(1.5) == 1536.0);
  return 0;
}
