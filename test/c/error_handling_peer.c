// Deliberately do not define X_ERROR_DOMAIN_IMPLEMENTATION in this TU.
#include "error_test_support.h"

static int32_t peer_capture(void)
{
  return 41;
}

static bool peer_failed(const int32_t value)
{
  return value != 41;
}

static char* peer_message(char* buffer, const size_t size, const int32_t value)
{
  snprintf(buffer, size, "peer:%d", (int)value);
  return buffer;
}

static const x_error_domain peer_domain = {
  peer_capture, peer_failed, peer_message, "peer"
};

int32_t error_peer_register(void)
{
  return x_error_register(&peer_domain);
}

x_error error_peer_set(const char* name, const int32_t value)
{
  return x_error_set(name, value);
}

x_error error_peer_capture(const char* name)
{
  return x_error_set(name);
}

void error_peer_verify(const x_error_domain* expected)
{
  x_error error = x_error_set("test", 99);
  CHECK(error.domain == expected && x_succ(error));
  error = x_error_set("test", 7);
  CHECK(error.domain == expected && x_fail(error));
  char buffer[64];
  CHECK(x_error_message(buffer, sizeof(buffer), error) == buffer);
  CHECK(strcmp(buffer, "test:7") == 0);
  CHECK(x_error_register(expected) == EEXIST);
}
