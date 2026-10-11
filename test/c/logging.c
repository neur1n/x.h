#define X_ERROR_DOMAIN_IMPLEMENTATION
#define X_LOG_LEVEL X_LOG_PLAIN
#include "generated_under_test.h"

#if defined(x_timestamp)
#error "Standalone Logging must not include Date and Time."
#endif

int main(void)
{
  char ts[26] = {0};
  if (_x_timestamp(ts, sizeof(ts)) != ts || ts[0] == '\0'
      || strchr(ts, '\n') != NULL || strlen(ts) >= sizeof(ts)) {
    return EXIT_FAILURE;
  }
  x_log('p', NULL, "standalone Logging");
  return EXIT_SUCCESS;
}
