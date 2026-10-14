#include "generated_under_test.h"

static char* peer_message(char* buffer, size_t size, int32_t value)
{
  snprintf(buffer, size, "peer:%d", value);
  return buffer;
}
static const x_error_domain peer_domain{nullptr, nullptr, peer_message, "peer"};
const x_error_domain* peer_posix() { return &error_domain_posix; }
const x_error_domain_registry* peer_registry() { return &error_domain_registry; }
const x_error_domain* peer_lookup(const char* name) { return _x_error_domain_lookup(name); }
int32_t peer_register() { return x_error::register_domain(&peer_domain); }
x_error peer_error() { return x_error("peer", 9); }
