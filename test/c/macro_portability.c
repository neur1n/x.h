#define X_ERROR_DOMAIN_IMPLEMENTATION
#include "../../c/x.h"

// Isolate standard macro dispatch from the library's extended signature text.
// The real error-handling tests retain X_PRETTY_FUNCTION and check diagnostics.
#undef X_PRETTY_FUNCTION
#define X_PRETTY_FUNCTION __func__

static int calls, domain_calls, output_calls;

static int32_t status(void)
{
  ++calls;
  return 0;
}

static int32_t add(int a, int b)
{
  ++calls;
  return a + b;
}

static const char* domain_name(void)
{
  ++domain_calls;
  return "generic";
}

static x_error* output(x_error* err)
{
  ++output_calls;
  return err;
}

int main(void)
{
  x_error err = x_error_set("generic", 0);
  if (x_fail(err)) {
    return EXIT_FAILURE;
  }
  errno = EINVAL;
  err = x_error_set("posix");
  if (err.value != EINVAL || !x_fail(err)) {
    return EXIT_FAILURE;
  }

  // The natural zero-argument form is valid under strict C99/C11.
  x_check(domain_name(), *output(&err), status());
  if (calls != 1 || domain_calls != 1 || output_calls != 1 || x_fail(err)) {
    return EXIT_FAILURE;
  }
  int value = 0;
  x_check("generic", err, add(value++, 0));
  if (value != 1 || calls != 2 || x_fail(err)) {
    return EXIT_FAILURE;
  }
  int32_t (*function)(void) = status;
  x_check("generic", err, function());
  x_check("generic", err, (status(), 0));
  int32_t expression = 0;
  x_check("generic", err, expression);
  if (calls != 4 || x_fail(err)) {
    return EXIT_FAILURE;
  }
  // Both arities must compile in strict C99/C11 and evaluate the expression once.
  x_assert(++value == 2);
  x_assert(++value == 3, "optional message");
  if (value != 3) {
    return EXIT_FAILURE;
  }
  x_assert(true);
  x_log('p', NULL, "format only");
  x_log('p', NULL, "%s: %d", "formatted", 42);
  return EXIT_SUCCESS;
}
