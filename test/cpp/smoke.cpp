#include "generated_under_test.h"
#include <new>
#include "support.h"

#if X_H != x_version(0, 9, 0)
#error "X_H must retain the encoded header version"
#endif

const x_error_domain* peer_posix();
const x_error_domain_registry* peer_registry();
int32_t peer_register();
x_error peer_error();

int main()
{
  CHECK(strcmp(X_H_VERBOSE, TEST_DESCRIPTION) == 0);
  CHECK(peer_registry() == &error_domain_registry);
  CHECK(peer_posix() == &error_domain_posix);
  CHECK(peer_register() == 0);
  CHECK(strcmp(x_error("peer", 9).message(), "peer:9") == 0);
  CHECK(strcmp(peer_error().domain(), "peer") == 0);
  CHECK(peer_error().value() == 9);
  CHECK(x_succ(x_ok()));
  errno = EACCES;
  CHECK(x_error("posix").value() == EACCES);
  x_assert(true);
  x_assert(true, "message");
  char message[1]{'x'};
  CHECK(x_error("generic", 7).message(message, sizeof(message)) == message);
  CHECK(message[0] == '\0');
#if TEST_LOGGING
  FILE* log = tmpfile();
  CHECK(log != nullptr);
  x_log('e', log, "Logging is present");
  const std::string log_text = read_stream(log);
  CHECK(log_text.find("] Logging is present\n") != std::string::npos);
  CHECK(fclose(log) == 0);
#else
#ifdef x_log
#error "Logging must not be included unless selected."
#endif
#endif
#if TEST_GPU_ERRORS
  CHECK(x_fail(x_error("cu", 1)) && x_fail(x_error("cuda", 1)));
  CHECK(x_fail(x_error("cublas", 1)));
  CHECK(!std::string(x_error("cu", 1).message()).empty());
#endif
#if TEST_CHECKSUM
  CHECK(x_checksum_crc32("123456789", 9) == 0xCBF43926u);
  const uint8_t carry[]{0, 1, 0xFF, 0xFF};
  const uint16_t previous = x_checksum_rfc1071(carry, 2);
  CHECK(x_checksum_rfc1071(carry + 2, 2, &previous) == 0xFFFE);
  CHECK(x_checksum_xor(carry, sizeof(carry)) == 1);
#endif
#if TEST_DATE_TIME
  x_event event{"cpu"};
  x_event_agg stats;
  CHECK(event.toc().value() == ENODATA && x_succ(event.status()));
  CHECK(event.dur() == -1.0 && event.status().value() == ENODATA);
  CHECK(x_succ(event.tic()));
  CHECK(event.dur() == -1.0 && event.status().value() == ENODATA);
  CHECK(x_succ(event.agg(stats, 1)));
  CHECK(stats.ready && stats.cyc == 1 && stats.sum >= 0.0);
  CHECK(stats.max.idx == 0 && stats.min.idx == 0);
  const double sum = stats.sum;
  CHECK(x_succ(event.agg(stats, 1)));
  CHECK(stats.sum == sum && stats.cyc == 1);
  CHECK(event.dur("bad") == -1.0 && x_succ(event.status()));
  CHECK(event.dur("") >= 0.0);
  CHECK(x_succ(event.agg(stats, 0)));
  CHECK(!stats.ready && stats.cyc == 0);
  stats.cyc = SIZE_MAX;
  CHECK(event.agg(stats, 1).value() == EOVERFLOW);
  CHECK(x_succ(event.status()));
  CHECK(stats.cyc == SIZE_MAX);
  FILE* output = tmpfile();
  CHECK(output != nullptr);
  stats.reset();
  stats.echo("bad", nullptr, output);
  const std::string printed = read_stream(output);
  CHECK(printed.find("no samples (0 cycles)") != std::string::npos);
  CHECK(fclose(output) == 0);
  expect_throw<std::invalid_argument>([] { x_event invalid{nullptr}; });
  expect_throw<std::invalid_argument>([] { x_event invalid{"missing"}; });
  static_assert(!std::is_copy_constructible_v<x_event>);
  static_assert(std::is_nothrow_destructible_v<x_event>);
  char timestamp[26]{};
  CHECK(x_timestamp(timestamp, sizeof(timestamp)) == timestamp);
  CHECK(strlen(timestamp) == 24);
  timestamp[0] = 'x';
  CHECK(x_timestamp(timestamp, 1) == timestamp && timestamp[0] == '\0');
  x_sleep(0);
#endif
#if TEST_FILESYSTEM
  CHECK(x_fexist("."));
  CHECK(!x_fexist(nullptr) && x_fsize(nullptr) == -1);
  char path[4096]{};
  CHECK(x_fpath(path, sizeof(path), ".") == path);
  char small[2]{'x', '\0'};
  errno = 0;
  CHECK(x_fpath(small, sizeof(small), ".") == nullptr);
  CHECK(strcmp(small, "x") == 0 && errno == ENOBUFS);
  CHECK(x_fpath(small, sizeof(small), "") == nullptr && errno == EINVAL);
  FILE* stream = stdout;
  CHECK(x_fopen(&stream, "", "r").value() == EINVAL && stream == stdout);
  CHECK(x_fopen(&stream, ".", "").value() == EINVAL && stream == stdout);
#endif
#if TEST_HARDWARE
  static_assert(std::is_same_v<decltype(x_ncpu()), size_t>);
  // A native query may legitimately fail and return zero.
  (void)x_ncpu();
#endif
#if TEST_MATH
  static_assert(x_Pi<double> > 3.14 && x_Pi<double> < 3.15);
  static_assert(x_PiB<uint64_t>(1) == UINT64_C(1125899906842624));
  static_assert(x_gcd(1536, 1024) == 512);
  static_assert(x_gcd(0, 0) == 0);
  static_assert(x_lcm(1536, 1024) == 3072);
  static_assert(x_next_exp(3, 10) == 27 && x_prev_exp(3, 10) == 9);
  static_assert(x_next_exp(2, 0) == 1 && x_prev_exp(2, 0) == 0);
  static_assert(x_next_mul(3, 6) == 9 && x_prev_mul(3, 6) == 6);
  static_assert(x_bit(63) == (UINT64_C(1) << 63));
  CHECK(x_next_exp<uint64_t>(3, UINT64_C(4052555153018976267))
      == UINT64_C(4052555153018976267));
  errno = EBUSY;
  CHECK(x_prev_exp<uint64_t>(2, UINT64_MAX) == (UINT64_C(1) << 63) && errno == EBUSY);
  CHECK(x_next_exp<uint64_t>(2, UINT64_MAX) == 0 && errno == ERANGE);
  CHECK(x_lcm(INT_MAX, 2) == 0 && errno == ERANGE);
  CHECK(x_lcm<uint8_t>(255, 2) == 0 && errno == ERANGE);
  CHECK(x_next_mul<uint8_t>(16, 255) == 0 && errno == ERANGE);
  CHECK(x_next_exp(1, 0) == 0 && errno == EINVAL);
  CHECK(x_gcd(INT_MIN, 1) == 0 && errno == EINVAL);
  CHECK(x_prev_exp(2, -1) == 0 && errno == EINVAL);
  CHECK(x_prev_mul(0, 3) == 0 && errno == EINVAL);
#endif
#if TEST_MEMORY
  int array[3]{};
  static_assert(x_count(array) == 3);
  double* value{nullptr};
  CHECK(!x_malloc(&value, sizeof(*value)));
  CHECK(x_malloc(&value, sizeof(*value)).value() == EINVAL);
  // x_malloc returns raw storage; explicitly begin the C++17 object lifetime.
  ::new (static_cast<void*>(value)) double{17.0};
  x_free(value);
  CHECK(value == nullptr);
  CHECK(x_malloc(&value, 0).value() == EINVAL && value == nullptr);
  int* objects = new int[3];
  x_delete<true>(objects);
  CHECK(objects == nullptr);
  char buffer[]{"abcdefg"};
  CHECK(!x_memcpy(nullptr, nullptr, 0));
  CHECK(x_memcpy(buffer + 1, buffer, 4).value() == EINVAL);
  CHECK(strcmp(buffer, "abcdefg") == 0);
  CHECK(x_memcpy(buffer, buffer, 1).value() == EINVAL);
  CHECK(x_memcpy(nullptr, buffer, 1).value() == EINVAL);
  CHECK(!x_memcpy(buffer, buffer + 4, 3));
  CHECK(memcmp(buffer, "efg", 3) == 0);
  size_t available{7}, total{9};
  CHECK(x_meminfo("cpu", nullptr, nullptr).value() == EINVAL);
  CHECK(x_meminfo("missing", &available, &total).value() == ENOTSUP);
  CHECK(available == 7 && total == 9);
  CHECK(!x_meminfo("cpu", &available, &total));
  CHECK(total > 0 && available <= total);
#endif
#if TEST_STRING
  char copied[8]{};
  CHECK(!x_strcpy(copied, sizeof(copied), "abcdefghi"));
  CHECK(strcmp(copied, "abcdefg") == 0);
  CHECK(x_strcpy(copied, sizeof(copied), copied).value() == EINVAL);
  CHECK(x_strcpy(copied + 1, 2, copied).value() == EINVAL);
  CHECK(strcmp(copied, "abcdefg") == 0);
  CHECK(x_strmty(nullptr) && x_strmty("") && !x_strmty("a"));
#endif
  return 0;
}
