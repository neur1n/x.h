#if TEST_GENERATED
#include "generated_under_test.h"
#else
#include "../../cpp/x.h"
#endif
#include "date_time_support.h"

static std::string echo(
    const x_event_agg& stats, const char* unit, const char* title)
{
  const x_event_agg snapshot = stats;
  FILE* stream = tmpfile();
  CHECK(stream != nullptr);
  stats.echo(unit, title, stream);
  const std::string printed = read_stream(stream);
  CHECK(fclose(stream) == 0);
  check_stats(stats, snapshot);
  return printed;
}

static void test_timestamp()
{
  char buffer[26]{};
  const char* (*function)(char*, size_t) = &x_timestamp;
  CHECK(function(buffer, sizeof(buffer)) == buffer);
  CHECK(strlen(buffer) == 24 && strchr(buffer, '\n') == nullptr);
  CHECK(strcmp(function(nullptr, 26), "") == 0);
  buffer[0] = 'x';
  CHECK(strcmp(function(buffer, 0), "") == 0 && buffer[0] == 'x');
  CHECK(function(buffer, 25) == buffer && buffer[0] == '\0');
  buffer[0] = 'x';
  CHECK(function(buffer, 1) == buffer && buffer[0] == '\0');
}

static void test_stats()
{
  x_event_agg stats;
  check_reset(stats);
  const std::string empty = echo(stats, "ms", "EMPTY");
  CHECK(empty == "[EMPTY] no samples (0 cycles)\n");

  stats.ready = true;
  stats.cyc = 2;
  stats.sum = 3e6;
  stats.avg = 1.5e6;
  stats.min = {0, 1e6};
  stats.max = {1, 2e6};
  CHECK(echo(stats, "ms", "TITLE")
      == "[TITLE] 3.000000ms in 2 cycles - avg: 1.500000ms, "
         "min(0): 1.000000ms, max(1): 2.000000ms\n");
  const char* units[]{nullptr, "", "unknown", "ns"};
  for (const char* unit : units) {
    CHECK(echo(stats, unit, "TITLE")
        == "[TITLE] 3000000.000000ns in 2 cycles - avg: 1500000.000000ns, "
           "min(0): 1000000.000000ns, max(1): 2000000.000000ns\n");
  }

  const char* titles[]{nullptr, ""};
  for (const char* title : titles) {
    const std::string printed = echo(stats, "ms", title);
    CHECK(printed.size() > 27 && printed[0] == '[' && printed[25] == ']');
    CHECK(printed.substr(26, 12) == " 3.000000ms ");
  }

  stats.reset();
  check_reset(stats);
  stats.reset();
  check_reset(stats);
}

static void test_event()
{
  static_assert(!std::is_copy_constructible_v<x_event>);
  static_assert(!std::is_copy_assignable_v<x_event>);
  static_assert(!std::is_move_constructible_v<x_event>);
  static_assert(std::is_nothrow_destructible_v<x_event>);
  expect_throw<std::invalid_argument>([] { x_event event{nullptr}; });
  expect_throw<std::invalid_argument>([] { x_event event{""}; });
  expect_throw<std::invalid_argument>([] { x_event event{"unknown"}; });

  x_event event{"cpu"};
  x_event_agg stats;
  const x_event& query = event;
  static_assert(std::is_same_v<decltype(query.status()), const x_error&>);
  static_assert(noexcept(query.status()));
  const x_error& live_status = query.status();
  CHECK(x_succ(live_status));
  check_event_status(event.toc(), "posix", ENODATA);
  CHECK(x_succ(event.status()));
  CHECK(event.dur("bad") == -1.0 && x_succ(event.status()));
  check_duration_error(query, "posix", ENODATA);
  CHECK(live_status.value() == ENODATA);
  const x_error saved_status = query.status();

  const char* units[]{"ns", "us", "ms", "s", "m", "h", nullptr, ""};
  const double scales[]{1.0, 1e3, 1e6, 1e9, 6e10, 3.6e12, 1.0, 1.0};
  const x_event_agg snapshot = stats;
  check_event_status(event.agg(stats, 2), "posix", ENODATA);
  check_stats(stats, snapshot);

  CHECK(x_succ(event.tic()));
  CHECK(x_succ(live_status));
  CHECK(saved_status.value() == ENODATA);
  check_duration_error(query, "posix", ENODATA);
  x_sleep(1);
  CHECK(x_succ(event.toc()));
  const double ns = query.dur();
  CHECK(std::isfinite(ns) && ns >= 0.0 && x_succ(event.status()));
  for (size_t idx{0}; idx < sizeof(units) / sizeof(units[0]); ++idx) {
    CHECK(query.dur(units[idx]) == ns / scales[idx]);
  }
  CHECK(query.dur("bad") == -1.0 && x_succ(event.status()));
  CHECK(query.dur() == ns);

  CHECK(x_succ(event.agg(stats, 0)));
  CHECK(query.dur() == ns);
  for (size_t idx{0}; idx < 3; ++idx) {
    CHECK(x_succ(event.tic()));
    CHECK(x_succ(event.agg(stats, 3)));
    CHECK(stats.cyc == idx + 1 && stats.ready == (idx == 2));
    CHECK(stats.avg == stats.sum / static_cast<double>(stats.cyc));
    CHECK(stats.min.val <= stats.max.val);
    CHECK(stats.min.idx < stats.cyc && stats.max.idx < stats.cyc);
  }
  const x_event_agg ready = stats;
  CHECK(x_succ(event.agg(stats, 3)));
  check_stats(stats, ready);
  CHECK(x_succ(event.agg(stats, 0)));
  check_reset(stats);

  stats.cyc = SIZE_MAX;
  const x_event_agg overflow = stats;
  check_event_status(event.agg(stats, 1), "posix", EOVERFLOW);
  check_stats(stats, overflow);
  CHECK(x_succ(event.status()) && query.dur() >= 0.0);
}

int main()
{
  test_timestamp();
  test_stats();
  test_event();
  return 0;
}
