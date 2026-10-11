#define X_ERROR_DOMAIN_IMPLEMENTATION
#include "error_test_support.h"

static void check_same_aggregation(const x_event_agg* a, const x_event_agg* b)
{
  CHECK(a->ready == b->ready && a->cyc == b->cyc);
  CHECK(a->sum == b->sum && a->avg == b->avg);
  CHECK(a->min.idx == b->min.idx && a->min.val == b->min.val);
  CHECK(a->max.idx == b->max.idx && a->max.val == b->max.val);
  CHECK(a->echo == b->echo && a->reset == b->reset);
}

static void check_empty_echo(x_event_agg* aggregation)
{
  x_event_agg snapshot = *aggregation;
  FILE* stream = tmpfile();
  CHECK(stream != NULL);
  aggregation->echo(aggregation, "ms", "EMPTY", stream);
  rewind(stream);

  char line[256];
  CHECK(fgets(line, sizeof(line), stream) != NULL);
  CHECK(strncmp(line, "[EMPTY] ", 8) == 0);
  CHECK(strstr(line, "no samples") != NULL && strstr(line, "0 cycles") != NULL);
  CHECK(strstr(line, "min(") == NULL && strstr(line, "max(") == NULL);
  CHECK(fgetc(stream) == EOF && !ferror(stream));
  CHECK(fclose(stream) == 0);
  check_same_aggregation(aggregation, &snapshot);
}

int main(void)
{
  char timestamp[26];
  const char* (*timestamp_function)(char*, size_t) = &x_timestamp;
  memset(timestamp, 'x', sizeof(timestamp));
  CHECK(timestamp_function(timestamp, sizeof(timestamp)) == timestamp);
  CHECK(timestamp[0] != '\0' && strchr(timestamp, '\n') == NULL);
  CHECK(strlen(timestamp) < sizeof(timestamp));
  CHECK(strcmp(x_timestamp(NULL, 26), "") == 0);
  CHECK(strcmp(x_timestamp(timestamp, 0), "") == 0);
  CHECK(x_timestamp(timestamp, 1) == timestamp && timestamp[0] == '\0');

  x_event event;
  CHECK(x_fail(x_event_init(NULL, "cpu", 0)));
  CHECK(x_fail(x_event_init(&event, NULL, 0)) && event.type == -1);
  CHECK(x_succ(x_event_term(&event)));
  CHECK(x_fail(x_event_init(&event, "unknown", 0)) && event.type == -1);
  CHECK(x_succ(x_event_term(&event)));
  CHECK(x_succ(x_event_init(&event, "cpu", 0)));
  CHECK(x_succ(event.status));
  CHECK(event.agg(&event, NULL, 1, NULL, 0).value == EINVAL);
  CHECK(x_succ(event.status));
  CHECK(event.dur(&event, "ns") == -1.0 && event.status.value == ENODATA);
  CHECK(event.dur(&event, "bogus") == -1.0 && event.status.value == ENODATA);
  CHECK(event.toc(&event, NULL, 0).value == ENODATA);
  CHECK(event.status.value == ENODATA);

  const char* units[] = {"ns", "us", "ms", "s", "m", "h"};
  const double scales[] = {1, 1e3, 1e6, 1e9, 60e9, 3600e9};
  for (size_t i = 0; i < sizeof(units) / sizeof(units[0]); ++i) {
    CHECK(event.dur(&event, units[i]) == -1.0 && x_fail(event.status));
    CHECK(event.status.value == ENODATA);
  }
  CHECK(event.toc(&event, NULL, 0).value == ENODATA);
  x_event_agg aggregation;
  x_event_agg_init(&aggregation);
  check_empty_echo(&aggregation);
  x_event_agg snapshot = aggregation;
  CHECK(event.agg(&event, &aggregation, 1, NULL, 0).value == ENODATA);
  check_same_aggregation(&aggregation, &snapshot);
  CHECK(x_succ(event.tic(&event, NULL, 0)));
  CHECK(event.dur(&event, "ns") == -1.0 && event.status.value == ENODATA);
  x_sleep(20);
  CHECK(x_succ(event.toc(&event, NULL, 0)));
  double ns = event.dur(&event, "ns");
  CHECK(ns >= 1e6 && isfinite(ns) && x_succ(event.status));
  for (size_t i = 0; i < sizeof(units) / sizeof(units[0]); ++i) {
    CHECK(event.dur(&event, units[i]) == ns / scales[i]);
  }
  CHECK(event.dur(&event, NULL) == ns && event.dur(&event, "") == ns);
  CHECK(event.dur(&event, "bogus") == -1.0 && x_succ(event.status));
  CHECK(event.started && event.stopped);
  CHECK(event.dur(&event, "ms") == ns / 1e6 && x_succ(event.status));

  CHECK(x_succ(event.agg(&event, &aggregation, 0, NULL, 0)));
  CHECK(aggregation.cyc == 0 && !aggregation.ready);
  CHECK(aggregation.echo != NULL && aggregation.reset != NULL);
  CHECK(x_succ(event.tic(&event, NULL, 0)));
  CHECK(event.dur(&event, "ns") == -1.0 && event.status.value == ENODATA);
  CHECK(x_succ(event.agg(&event, &aggregation, 2, NULL, 0)));
  CHECK(aggregation.cyc == 1 && !aggregation.ready);
  CHECK(x_succ(event.tic(&event, NULL, 0)));
  CHECK(x_succ(event.agg(&event, &aggregation, 2, NULL, 0)));
  CHECK(aggregation.cyc == 2 && aggregation.ready);
  CHECK(aggregation.avg == aggregation.sum / 2.0);
  CHECK(aggregation.min.val <= aggregation.max.val);
  CHECK(aggregation.min.idx < 2 && aggregation.max.idx < 2);
  snapshot = aggregation;
  CHECK(x_succ(event.agg(&event, &aggregation, 2, NULL, 0)));
  check_same_aggregation(&aggregation, &snapshot);

  FILE* output = tmpfile();
  CHECK(output != NULL);
  aggregation.echo(&aggregation, "bogus", "TITLE", output);
  rewind(output);
  char line[512];
  CHECK(fgets(line, sizeof(line), output) != NULL);
  CHECK(strncmp(line, "[TITLE] ", 8) == 0 && strstr(line, "ns in 2 cycles") != NULL);
  CHECK(fclose(output) == 0);

  aggregation.reset(&aggregation);
  check_empty_echo(&aggregation);
  snapshot = aggregation;
  CHECK(x_succ(x_event_term(&event)) && event.type == -1);
  CHECK(!event.started && !event.stopped && x_succ(event.status));
  CHECK(event.dur(&event, "ms") == -1.0 && x_succ(event.status));
  CHECK(event.toc(&event, NULL, 0).value == EINVAL);
  CHECK(event.tic(&event, NULL, 0).value == EINVAL);
  CHECK(event.agg(&event, &aggregation, 1, NULL, 0).value == EINVAL);
  CHECK(x_succ(event.status));
  check_same_aggregation(&aggregation, &snapshot);
  CHECK(x_succ(x_event_term(&event)));
  CHECK(x_fail(x_event_term(NULL)));
  return 0;
}
