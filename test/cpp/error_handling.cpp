#include "../../cpp/x.h"
#include "support.h"
#if X_WINDOWS
#include <crtdbg.h>
#endif

static int domain_calls, call_calls, output_calls, message_calls;
static bool failed(int32_t value) { return value != 99; }
static char* message(char* buffer, size_t size, int32_t value)
{ snprintf(buffer, size, "test:%d", value); return buffer; }
static const x_error_domain domain{nullptr, failed, message, "test"};
static const x_error_domain fallback{nullptr, nullptr, nullptr, "fallback"};
static char* long_message(char* buffer, size_t size, int32_t)
{
  if (buffer == nullptr || size == 0) { return buffer; }
  const size_t count = size - 1 < 400 ? size - 1 : 400;
  memset(buffer, 'x', count);
  buffer[count] = '\0';
  return buffer;
}
static const x_error_domain long_domain{nullptr, nullptr, long_message, "long"};
// Failure predicates belong to domains, not individual x_error values.
static_assert(!std::is_constructible_v<x_error, const char*, int32_t,
    bool (*)(int32_t)>);
static_assert(!std::is_constructible_v<x_error, const x_error_domain&, int32_t,
    bool (*)(int32_t)>);
static const char* domain_name() { ++domain_calls; return "test"; }
static int32_t code(int32_t value) { ++call_calls; return value; }
static x_error value_error(int32_t value) { ++call_calls; return x_error("test", value); }
static x_error& output(x_error& error) { ++output_calls; return error; }
static const char* assertion_message()
{ ++message_calls; fprintf(stderr, "MESSAGE_EVALUATED\n"); return "side-effect message"; }
static void null_assertion() { x_assert(false, nullptr); }
static void empty_assertion() { x_assert(false, ""); }
static void message_assertion() { x_assert(false, assertion_message()); }

int main(int argc, char** argv)
{
#if X_WINDOWS
  _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
#endif
  CHECK(argc == 2);
  const char* scenario = argv[1];
  CHECK(x_error::register_domain(&domain) == 0);
  CHECK(x_error::register_domain(&fallback) == 0);
  if (strcmp(scenario, "abort_unknown_value") == 0) { (void)x_error("missing", 1); }
  else if (strcmp(scenario, "abort_unknown_capture") == 0) { (void)x_error("missing"); }
  else if (strcmp(scenario, "abort_null_name") == 0) { (void)x_error(nullptr, 1); }
  else if (strcmp(scenario, "abort_empty_name") == 0) { (void)x_error("", 1); }
  else if (strcmp(scenario, "abort_unsupported_capture") == 0) { (void)x_error("generic"); }
  else if (strcmp(scenario, "abort_unsupported_custom_capture") == 0) { (void)x_error("fallback"); }
  else if (strcmp(scenario, "abort_null_assertion") == 0) { null_assertion(); }
  else if (strcmp(scenario, "abort_empty_assertion") == 0) { empty_assertion(); }
  else if (strcmp(scenario, "abort_message_assertion") == 0) { message_assertion(); }
  else if (strcmp(scenario, "check") == 0) {
    x_error error;
    x_check("generic", output(error), code(7));
    CHECK(error.value() == 7 && x_fail(error));
    x_check(domain_name(), output(error), value_error(0));
    CHECK(error.value() == 0 && x_fail(error));
    CHECK(domain_calls == 1 && call_calls == 2 && output_calls == 2);
  } else if (strcmp(scenario, "values") == 0) {
    CHECK(x_succ(x_error("test", 99)) && x_fail(x_error("test", 0)));
    CHECK(!x_error("fallback", 0) && x_error("fallback", 1));
    CHECK(strcmp(x_error("fallback", 7).message(), "Unknown error 7") == 0);
    CHECK(x_error::register_domain(nullptr) == EINVAL);
    CHECK(x_error::register_domain(&domain) == EEXIST);
    CHECK(x_error::register_domain(&error_domain_posix) == EEXIST);
    errno = EIO;
    CHECK(x_error("posix").value() == EIO);
#if X_ENABLE_SOCKET
#if X_WINDOWS
    WSASetLastError(WSAEACCES);
    CHECK(x_error("socket").value() == WSAEACCES);
#else
    CHECK(x_error("socket").value() == EIO);
#endif
#endif
    x_error error("test", 99);
    x_check(domain_name(), output(error), code(99));
    CHECK(!error && domain_calls == 1 && call_calls == 1 && output_calls == 1);
    x_assert(true, assertion_message());
    CHECK(message_calls == 0);
    error.set("generic", 7, "owned text");
    const char* old = error.message();
    error.set("generic", 8, old);
    CHECK(strcmp(error.message(), "owned text") == 0);
    char text[32]{};
    CHECK(error.message(text, sizeof(text)) == text);
    CHECK(strcmp(text, "owned text") == 0);
    CHECK(error.message(nullptr, sizeof(text)) == nullptr);
    CHECK(error.message(text, 0) == text && strcmp(text, "owned text") == 0);
    CHECK(strcmp(error.message(), "owned text") == 0);
    error.set("generic", 5);
    CHECK(strcmp(error.message(), "Generic error 5") == 0);
    for (size_t capacity = 1; capacity <= 16; ++capacity) {
      char small[16]{};
      error.message(small, capacity);
      const size_t expected = capacity - 1 < 15 ? capacity - 1 : 15;
      CHECK(strlen(small) == expected && strncmp(small, "Generic error 5", expected) == 0);
    }
    char large[512]{};
    const x_error long_error{long_domain, 7};
    const char* cached = long_error.message();
    CHECK(strlen(cached) == 255);
    CHECK(long_error.message(large, sizeof(large)) == large && strlen(large) == 400);
    CHECK(strlen(cached) == 255); // The buffer overload does not invalidate cache.
    error.set("generic", 7, large);
    CHECK(strlen(error.message()) == 400); // Overrides bypass domain truncation.
    CHECK(error.value() == 7 && error);
    error.set(long_domain, 0);
    CHECK(!error && strcmp(error.domain(), "long") == 0 && error.value() == 0);
    CHECK(strlen(error.message()) == 255); // set clears the message override.
    // Fill the fixed registry with domains whose storage remains valid.
    static x_error_domain domains[X_ERROR_DOMAIN_LIMIT]{};
    static char names[X_ERROR_DOMAIN_LIMIT][24]{};
    const size_t remaining = X_ERROR_DOMAIN_LIMIT - error_domain_registry.count;
    for (size_t i = 0; i <= remaining; ++i) {
      snprintf(names[i], sizeof(names[i]), "capacity-%zu", i);
      domains[i] = {nullptr, nullptr, nullptr, names[i]};
      CHECK(x_error::register_domain(&domains[i]) == (i == remaining ? ENOSPC : 0));
    }
  } else if (strcmp(scenario, "logging") == 0) {
    FILE* stream = tmpfile();
    CHECK(stream != nullptr);
    x_log('I', stream, "value:%d %%", 7);
    x_log('p', stream, "plain %%");
    const std::string text = read_stream(stream);
#if X_LOG_LEVEL >= X_LOG_INFO
    CHECK(text.find("value:7 %") != std::string::npos);
#endif
#if X_LOG_LEVEL >= X_LOG_PLAIN
    CHECK(text.find("plain %") != std::string::npos);
    CHECK(text.find("\033[") == std::string::npos);
#else
    CHECK(text.empty());
#endif
    CHECK(fclose(stream) == 0);
  } else { CHECK(false); }
  return 0;
}
