/* Core-only diagnostics must work even with logging explicitly disabled. */
#define X_ERROR_DOMAIN_IMPLEMENTATION
#define X_LOG_LEVEL (-1)
#include "generated_under_test.h"

#ifdef x_log
#error "The minimal header must not include Logging."
#endif

static int32_t status(int32_t value)
{
  return value;
}

int main(void)
{
  x_error err = x_ok();
  x_check("generic", err, status(7));
  if (!x_fail(err)) {
    return EXIT_FAILURE;
  }
  x_check("generic", err, status(8));
  return x_fail(err) ? EXIT_SUCCESS : EXIT_FAILURE;
}
