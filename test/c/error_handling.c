#define X_ERROR_DOMAIN_IMPLEMENTATION
#include "error_test_support.h"

#if !X_WINDOWS
#include <sys/resource.h>
#endif

static int status_calls;
static int message_calls;
static int capture_calls;
static int domain_message_calls;

static int status(int value)
{
  ++status_calls;
  return value;
}

static const char* assertion_message(void)
{
  ++message_calls;
  fprintf(stderr, "MESSAGE_EVALUATED\n");
  return "side-effect message";
}

static int32_t capture(void)
{
  ++capture_calls;
  return 99;
}

static bool failed(const int32_t value)
{
  return value != 99;
}

static char* message(char* buffer, const size_t size, const int32_t value)
{
  ++domain_message_calls;
  snprintf(buffer, size, "test:%d", (int)value);
  return buffer;
}

static const x_error_domain test_domain = {
  capture, failed, message, "test"
};

static const x_error_domain fallback_domain = {
  NULL, NULL, NULL, "fallback"
};

static void unknown_value(void)
{
  (void)x_error_set("missing", 0);
}

static void unknown_capture(void)
{
  (void)x_error_set("missing");
}

static void null_name(void)
{
  (void)x_error_set(NULL, 0);
}

static void empty_name(void)
{
  (void)x_error_set("", 0);
}

static void unsupported_capture(void)
{
  (void)x_error_set("generic");
}

static void unsupported_custom_capture(void)
{
  CHECK(x_error_register(&fallback_domain) == 0);
  (void)x_error_set("fallback");
}

static void omitted_assertion(void)
{
  x_assert(false);
}

static void null_assertion(void)
{
  x_assert(false, NULL);
}

static void empty_assertion(void)
{
  x_assert(false, "");
}

static void message_assertion(void)
{
  x_assert(false, assertion_message());
}

static void test_values(void)
{
  CHECK(strcmp(_x_filename("file.c"), "file.c") == 0);
  CHECK(strcmp(_x_filename("dir/file.c"), "file.c") == 0);
#if X_WINDOWS
  CHECK(strcmp(_x_filename("dir\\file.c"), "file.c") == 0);
  CHECK(strcmp(_x_filename("dir\\sub/file.c"), "file.c") == 0);
  CHECK(strcmp(_x_filename("dir/sub\\file.c"), "file.c") == 0);
#else
  CHECK(strcmp(_x_filename("dir\\file.c"), "dir\\file.c") == 0);
  CHECK(strcmp(_x_filename("dir/sub\\file.c"), "sub\\file.c") == 0);
#endif
  CHECK(x_succ(x_ok()));
  x_error zero = {NULL, 0};
  CHECK(x_succ(zero));
  zero.value = 7;
  CHECK(x_fail(zero));

  CHECK(_x_error_domain_lookup(NULL) == NULL);
  CHECK(_x_error_domain_lookup("") == NULL);
  CHECK(_x_error_domain_lookup("missing") == NULL);
  CHECK(_x_error_domain_lookup("generic") != NULL);
  CHECK(_x_error_domain_lookup("posix") != NULL);
  CHECK(x_succ(x_error_set("generic", 0)));
  CHECK(x_fail(x_error_set("posix", EINVAL)));
  x_error direct = _x_error_set_domain(NULL, 7);
  CHECK(direct.value == 7 && x_fail(direct));
  CHECK(strcmp(direct.domain->name, "generic") == 0);
  CHECK(x_error_register(&test_domain) == 0);
  CHECK(x_succ(x_error_set("test", 99)));
  CHECK(x_fail(x_error_set("test", 0)));
  CHECK(x_error_register(&fallback_domain) == 0);
  CHECK(x_succ(x_error_set("fallback", 0)));
  CHECK(x_fail(x_error_set("fallback", 1)));
}

static void test_registration(void)
{
  CHECK(x_error_register(NULL) == EINVAL);
  x_error_domain invalid = {NULL, NULL, NULL, NULL};
  CHECK(x_error_register(&invalid) == EINVAL);
  invalid.name = "";
  CHECK(x_error_register(&invalid) == EINVAL);
  invalid.name = "generic";
  CHECK(x_error_register(&invalid) == EEXIST);
  invalid.name = "posix";
  CHECK(x_error_register(&invalid) == EEXIST);

  CHECK(x_error_register(&test_domain) == 0);
  CHECK(x_error_register(&test_domain) == EEXIST);
  x_error_domain duplicate = test_domain;
  char duplicate_name[] = "test";
  duplicate.name = duplicate_name;
  CHECK(x_error_register(&duplicate) == EEXIST);
  CHECK(x_error_set("test", 99).domain == &test_domain);
}

static void test_capture(void)
{
  CHECK(x_error_register(&test_domain) == 0);
  CHECK(x_succ(x_error_set("test")));
  CHECK(capture_calls == 1);
  (void)x_error_set("test", 7);
  CHECK(capture_calls == 1);
  errno = EINVAL;
  x_error error = x_error_set("posix");
  CHECK(error.value == EINVAL && x_fail(error));
  errno = 0;
  CHECK(x_succ(x_error_set("posix")));
  CHECK(error.value == EINVAL);
}

static void test_messages(void)
{
  CHECK(x_error_register(&test_domain) == 0);
  CHECK(x_error_register(&fallback_domain) == 0);
  x_error error = x_error_set("posix", EINVAL);
  char buffer[64];
  CHECK(x_error_message(buffer, sizeof(buffer), x_error_set("test", 7)) == buffer);
  CHECK(strcmp(buffer, "test:7") == 0);
  x_error_message(buffer, sizeof(buffer), x_error_set("generic", 7));
  CHECK(strcmp(buffer, "Generic error 7") == 0);
  x_error_message(buffer, sizeof(buffer), x_error_set("fallback", 7));
  CHECK(strcmp(buffer, "Unknown error 7") == 0);
  // Numeric fallback messages must truncate safely, including int32 extremes.
  const char* names[] = {"generic", "fallback"};
  const char* prefixes[] = {"Generic error", "Unknown error"};
  const int32_t values[] = {0, INT32_MIN, INT32_MAX};
  const size_t sizes[] = {1, 4, 64};
  for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); ++i) {
    for (size_t j = 0; j < sizeof(values) / sizeof(values[0]); ++j) {
      char expected[64];
      snprintf(expected, sizeof(expected), "%s %d", prefixes[i], values[j]);
      for (size_t k = 0; k < sizeof(sizes) / sizeof(sizes[0]); ++k) {
        char storage[66];
        memset(storage, 'Z', sizeof(storage));
        char* msg = storage + 1;
        const size_t size = sizes[k];
        CHECK(x_error_message(msg, size, x_error_set(names[i], values[j])) == msg);
        const size_t len = strlen(expected) < size - 1
            ? strlen(expected) : size - 1;
        CHECK(strlen(msg) == len && memcmp(msg, expected, len) == 0);
        CHECK(storage[0] == 'Z');
        for (size_t n = size + 1; n < sizeof(storage); ++n) {
          CHECK(storage[n] == 'Z');
        }
      }
    }
  }
  x_error zero = {NULL, 7};
  x_error_message(buffer, sizeof(buffer), zero);
  CHECK(strcmp(buffer, "Generic error 7") == 0);
  x_error_message(buffer, sizeof(buffer), error);
  CHECK(buffer[0] != '\0' && memchr(buffer, '\0', sizeof(buffer)) != NULL);
  x_error_message(buffer, sizeof(buffer), x_error_set("posix", INT32_MIN));
  CHECK(buffer[0] != '\0' && memchr(buffer, '\0', sizeof(buffer)) != NULL);

  struct { char before; char text[5]; char after; } guarded = {
    'L', {'x', 'x', 'x', 'x', 'x'}, 'R'
  };
  x_error_message(guarded.text, sizeof(guarded.text), x_error_set("test", 7));
  CHECK(strcmp(guarded.text, "test") == 0);
  CHECK(guarded.before == 'L' && guarded.after == 'R');
  char tiny[1] = {'x'};
  x_error_message(tiny, sizeof(tiny), error);
  CHECK(tiny[0] == '\0');
  tiny[0] = 'x';
  x_error_message(tiny, sizeof(tiny), x_error_set("posix", INT32_MIN));
  CHECK(tiny[0] == '\0');
  CHECK(x_error_message(NULL, 1, error) == NULL);
  tiny[0] = 'x';
  CHECK(x_error_message(tiny, 0, error) == tiny && tiny[0] == 'x');
  int calls = domain_message_calls;
  error = x_error_set("test", 7);
  CHECK(x_error_message(NULL, 64, error) == NULL);
  CHECK(x_error_message(tiny, 0, error) == tiny && tiny[0] == 'x');
  CHECK(domain_message_calls == calls);
}

static void test_socket(void)
{
  CHECK(_x_error_domain_lookup("socket") != NULL);
  CHECK(x_succ(x_error_set("socket", 0)));
#if X_WINDOWS
  const int32_t value = WSAEINVAL;
  WSASetLastError(value);
#else
  const int32_t value = EINVAL;
  errno = value;
#endif
  x_error error = x_error_set("socket");
  CHECK(error.value == value && x_fail(error));
  CHECK(strcmp(error.domain->name, "socket") == 0);
  x_error peer = error_peer_set("socket", value);
  CHECK(peer.domain == error.domain && peer.value == value && x_fail(peer));
#if X_WINDOWS
  WSASetLastError(0);
#else
  errno = 0;
#endif
  CHECK(x_succ(x_error_set("socket")));
  CHECK(error.value == value);

  char buffer[256];
  CHECK(x_error_message(buffer, sizeof(buffer), error) == buffer);
  CHECK(buffer[0] != '\0' && memchr(buffer, '\0', sizeof(buffer)) != NULL);
  char tiny[1] = {'x'};
  x_error_message(tiny, sizeof(tiny), error);
  CHECK(tiny[0] == '\0');
  x_error_domain duplicate = {NULL, NULL, NULL, "socket"};
  CHECK(x_error_register(&duplicate) == EEXIST);
}

static void test_check(void)
{
  CHECK(x_error_register(&test_domain) == 0);
  // The caller's variable can be named msg; the output lvalue is used once.
  x_error msg = x_ok();
  x_check("generic", msg, status(7));
  CHECK(msg.value == 7 && x_fail(msg) && status_calls == 1);
  x_error errors[2];
  size_t index = 0;
  const char* names[] = {"test"};
  size_t name_index = 0;
  x_check(names[name_index++], errors[index++], status(99));
  CHECK(index == 1 && name_index == 1 && status_calls == 2);
  CHECK(errors[0].value == 99 && x_succ(errors[0]));
  CHECK(domain_message_calls == 0);
  x_check("test", msg, status(0));
  CHECK(msg.domain == &test_domain && msg.value == 0 && x_fail(msg));
  CHECK(status_calls == 3);
}

static void test_check_success(void)
{
  CHECK(x_error_register(&test_domain) == 0);
  x_error error;
  x_check("test", error, status(99));
  CHECK(x_succ(error) && error.value == 99 && status_calls == 1);
  CHECK(domain_message_calls == 0);
  x_check("generic", error, status(0));
  CHECK(x_succ(error) && status_calls == 2);
}

static void test_logging(void)
{
  FILE* stream = tmpfile();
  CHECK(stream != NULL);
  x_log('e', stream, "ready");
  const char* format = "%s: %d";
  x_log('e', stream, format, "value", 42);
  CHECK(fflush(stream) == 0);
#if X_LOG_LEVEL >= X_LOG_ERROR
  rewind(stream);
  char line[X_LOG_PREFIX_LIMIT + 64];
  CHECK(fgets(line, sizeof(line), stream) != NULL);
  CHECK(strstr(line, "] ready\n") != NULL);
  CHECK(fgets(line, sizeof(line), stream) != NULL);
  CHECK(strstr(line, "] value: 42\n") != NULL);
  CHECK(fgets(line, sizeof(line), stream) == NULL);
#else
  CHECK(ftell(stream) == 0);
#endif
  CHECK(fclose(stream) == 0);
}

static void test_assert_success(void)
{
  int expression_calls = 0;
  x_assert(++expression_calls == 1, assertion_message());
  CHECK(expression_calls == 1 && message_calls == 0);
  x_assert(++expression_calls == 2);
  CHECK(expression_calls == 2 && message_calls == 0);
  x_assert(true, NULL);
  x_assert(true, "");
}

static void test_capacity(void)
{
  CHECK(x_error_register(&test_domain) == 0);
  CHECK(x_error_register(&fallback_domain) == 0);
  static x_error_domain domains[X_ERROR_DOMAIN_LIMIT];
  static char domain_names[X_ERROR_DOMAIN_LIMIT][32];
  size_t available = X_ERROR_DOMAIN_LIMIT - error_domain_registry.count;
  for (size_t i = 0; i < available; ++i) {
    snprintf(domain_names[i], sizeof(domain_names[i]), "capacity-%zu", i);
    domains[i].name = domain_names[i];
    CHECK(x_error_register(&domains[i]) == 0);
  }
  CHECK(error_domain_registry.count == X_ERROR_DOMAIN_LIMIT);
  x_error_domain invalid = {NULL, NULL, NULL, NULL};
  invalid.name = "overflow";
  CHECK(x_error_register(&invalid) == ENOSPC);
  CHECK(x_error_register(&test_domain) == EEXIST);
  CHECK(x_error_register(NULL) == EINVAL);
  x_error error = x_error_set("test", 99);
  CHECK(error.domain == &test_domain && x_succ(error));
  CHECK(x_fail(x_error_set("posix", EINVAL)));
  char buffer[64];
  x_error_message(buffer, sizeof(buffer), x_error_set("test", 7));
  CHECK(strcmp(buffer, "test:7") == 0);
}

static void test_cross_tu(void)
{
  CHECK(x_error_register(&test_domain) == 0);
  error_peer_verify(&test_domain);
  x_error error = error_peer_set("test", 7);
  CHECK(error.domain == &test_domain && x_fail(error));
  error = error_peer_capture("test");
  CHECK(error.domain == &test_domain && x_succ(error) && capture_calls == 1);

  CHECK(error_peer_register() == 0);
  error = x_error_set("peer");
  CHECK(error.value == 41 && x_succ(error));
  CHECK(x_fail(x_error_set("peer", 0)));
  char buffer[64];
  x_error_message(buffer, sizeof(buffer), x_error_set("peer", 7));
  CHECK(strcmp(buffer, "peer:7") == 0);
  CHECK(error_peer_register() == EEXIST);
}

#if X_WINDOWS
static void test_win32(void)
{
  CHECK(x_succ(x_error_set("wincom", S_OK)));
  CHECK(x_succ(x_error_set("wincom", S_FALSE)));
  CHECK(x_fail(x_error_set("wincom", E_FAIL)));
  CHECK(x_succ(x_error_set("winnt", 0x40000000)));
  CHECK(x_fail(x_error_set("winnt", (int32_t)UINT32_C(0x80000005))));
  CHECK(x_fail(x_error_set("winnt", (int32_t)UINT32_C(0xC0000001))));
  CHECK(_x_error_domain_lookup("wincom")->capture == NULL);
  CHECK(_x_error_domain_lookup("winnt")->capture == NULL);
  SetLastError(ERROR_ACCESS_DENIED);
  x_error error = x_error_set("win32");
  CHECK(error.value == ERROR_ACCESS_DENIED && x_fail(error));
  SetLastError(ERROR_SUCCESS);
  CHECK(x_succ(x_error_set("win32")));
  CHECK(error.value == ERROR_ACCESS_DENIED);
  char buffer[256];
  x_error_message(buffer, sizeof(buffer), error);
  CHECK(buffer[0] != '\0' && memchr(buffer, '\0', sizeof(buffer)) != NULL);
  size_t length = strlen(buffer);
  CHECK(buffer[length - 1] != '\r' && buffer[length - 1] != '\n');
  char tiny[1] = {'x'};
  x_error_message(tiny, sizeof(tiny), error);
  CHECK(tiny[0] == '\0');
  x_error_message(buffer, sizeof(buffer), x_error_set("win32", -1));
  CHECK(strcmp(buffer, "Win32 error -1") == 0);
  x_error_message(buffer, sizeof(buffer), x_error_set("wincom", E_ACCESSDENIED));
  CHECK(buffer[0] != '\0' && memchr(buffer, '\0', sizeof(buffer)) != NULL);
  x_error_message(buffer, sizeof(buffer), x_error_set("winnt", (int32_t)UINT32_C(0xC0000001)));
  CHECK(buffer[0] != '\0' && memchr(buffer, '\0', sizeof(buffer)) != NULL);
}
#endif

#if X_IOS || X_MACOS
static void test_mach(void)
{
  CHECK(x_succ(x_error_set("mach", KERN_SUCCESS)));
  x_error error = x_error_set("mach", KERN_INVALID_ARGUMENT);
  CHECK(x_fail(error) && strcmp(error.domain->name, "mach") == 0);
  CHECK(error.domain->capture == NULL);
  x_error peer = error_peer_set("mach", KERN_INVALID_ARGUMENT);
  CHECK(peer.domain == error.domain && peer.value == error.value);
  char buffer[256];
  x_error_message(buffer, sizeof(buffer), error);
  CHECK(strcmp(buffer, mach_error_string(KERN_INVALID_ARGUMENT)) == 0);
  x_error_domain duplicate = {NULL, NULL, NULL, "mach"};
  CHECK(x_error_register(&duplicate) == EEXIST);
}
#endif

typedef struct test_case {
  const char* name;
  void (*run)(void);
  bool aborts;
} test_case;

static const test_case cases[] = {
  {"values", test_values, false},
  {"registration", test_registration, false},
  {"capture", test_capture, false},
  {"messages", test_messages, false},
  {"socket", test_socket, false},
  {"check", test_check, false},
  {"check_success", test_check_success, false},
  {"logging", test_logging, false},
  {"assert_success", test_assert_success, false},
  {"capacity", test_capacity, false},
  {"cross_tu", test_cross_tu, false},
#if X_WINDOWS
  {"win32", test_win32, false},
#endif
#if X_IOS || X_MACOS
  {"mach", test_mach, false},
#endif
  {"abort_unknown_value", unknown_value, true},
  {"abort_unknown_capture", unknown_capture, true},
  {"abort_null_name", null_name, true},
  {"abort_empty_name", empty_name, true},
  {"abort_unsupported_capture", unsupported_capture, true},
  {"abort_unsupported_custom_capture", unsupported_custom_capture, true},
  {"abort_omitted_assertion", omitted_assertion, true},
  {"abort_null_assertion", null_assertion, true},
  {"abort_empty_assertion", empty_assertion, true},
  {"abort_message_assertion", message_assertion, true}
};

static void prepare_abort(void)
{
#if X_WINDOWS
  SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  // Both MSVC and MinGW expose the CRT abort-report controls.
  _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
#else
  struct rlimit limit = {0, 0};
  CHECK(setrlimit(RLIMIT_CORE, &limit) == 0);
#endif
}

int main(int argc, char** argv)
{
  if (argc != 2) {
    fprintf(stderr, "Usage: %s CASE\n", argv[0]);
    return EXIT_FAILURE;
  }
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
    if (strcmp(argv[1], cases[i].name) == 0) {
      if (cases[i].aborts) {
        prepare_abort();
      }
      cases[i].run();
      CHECK(!cases[i].aborts);
      return EXIT_SUCCESS;
    }
  }
  fprintf(stderr, "Unknown test case: %s\n", argv[1]);
  return EXIT_FAILURE;
}
