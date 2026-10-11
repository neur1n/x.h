#ifndef X_TEST_CPP_DATE_TIME_SUPPORT_H
#define X_TEST_CPP_DATE_TIME_SUPPORT_H
#include "support.h"

inline void check_stats(
    const x_event_agg& actual, const x_event_agg& expected)
{
  CHECK(actual.ready == expected.ready && actual.cyc == expected.cyc);
  CHECK(actual.sum == expected.sum && actual.avg == expected.avg);
  CHECK(actual.min.idx == expected.min.idx);
  CHECK(actual.min.val == expected.min.val);
  CHECK(actual.max.idx == expected.max.idx);
  CHECK(actual.max.val == expected.max.val);
}

inline void check_reset(const x_event_agg& stats)
{
  CHECK(!stats.ready && stats.cyc == 0);
  CHECK(stats.sum == 0.0 && stats.avg == 0.0);
  CHECK(stats.min.idx == 0 && stats.min.val == DBL_MAX);
  CHECK(stats.max.idx == 0 && stats.max.val == -DBL_MAX);
}

inline void check_event_status(
    const x_error& status, const char* domain, int32_t value)
{
  CHECK(strcmp(status.domain(), domain) == 0 && status.value() == value);
}

inline void check_duration_error(
    const x_event& event, const char* domain, int32_t value)
{
  const char* units[]{"ns", "us", "ms", "s", "m", "h", nullptr, ""};
  for (const char* unit : units) {
    CHECK(event.dur(unit) == -1.0);
    check_event_status(event.status(), domain, value);
  }
}

template<typename Function>
void expect_event_error(Function&& function, const char* domain, int32_t value)
{
  try {
    function();
  } catch (const std::runtime_error& error) {
    const std::string prefix = std::string("x_event: ") + domain
        + ":" + std::to_string(value) + ": ";
    CHECK(std::string(error.what()).find(prefix) == 0);
    return;
  }

  CHECK(false);
}
#endif
