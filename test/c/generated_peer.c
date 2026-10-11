/* Registration in this translation unit must be visible to smoke.c. */
#include "generated_under_test.h"

static char* peer_message(char* buffer, const size_t size, const int32_t value)
{
  snprintf(buffer, size, "peer:%d", value);
  return buffer;
}

static const x_error_domain peer_domain = {
  NULL, NULL, peer_message, "generated-peer"
};

int32_t register_peer_domain(void)
{
  return x_error_register(&peer_domain);
}
