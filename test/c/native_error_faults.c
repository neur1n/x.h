// Real domain registration, predicates, and formatters with injected platform
// results. These tests complement native SDK tests, rather than replacing them.
#if defined(__linux__) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { \
  if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    exit(EXIT_FAILURE); \
  } \
} while (false)

#ifdef TEST_CPP_DISTRIBUTION
#define X_INL inline
#else
#define X_INL static inline
#endif
#define X_WINDOWS (TEST_PLATFORM == 3)
#define X_MACOS (TEST_PLATFORM == 2)
#define X_IOS (TEST_PLATFORM == 4)
#define X_CYGWIN 0
#define X_ENABLE_SOCKET 0
#define X_ENABLE_CUDA_DRIVER 0
#define X_ENABLE_CUDA_RUNTIME 0
#define X_ENABLE_CUBLAS 0
#define X_ERROR_DOMAIN_IMPLEMENTATION
#define X_FILENAME "native_error_faults.c"

static int query_calls;

#if X_WINDOWS
typedef uint32_t DWORD;
typedef int32_t HRESULT;
typedef int32_t NTSTATUS;
typedef void* HMODULE;
#define errno_t int
#define FAILED(value) (((HRESULT)(value)) < 0)
#define NT_SUCCESS(value) (((NTSTATUS)(value)) >= 0)
#define HRESULT_FACILITY(value) (((uint32_t)(value) >> 16) & 0x1fff)
#define HRESULT_CODE(value) ((uint32_t)(value) & 0xffff)
#define FACILITY_WIN32 7
#define FORMAT_MESSAGE_FROM_SYSTEM 0x1000
#define FORMAT_MESSAGE_FROM_HMODULE 0x0800
#define FORMAT_MESSAGE_IGNORE_INSERTS 0x0200
#define FORMAT_MESSAGE_MAX_WIDTH_MASK 0xff
#define LANG_ENGLISH 9
#define SUBLANG_ENGLISH_US 1
#define MAKELANGID(language, sublanguage) ((language) | ((sublanguage) << 10))
static int module_token, module_calls;
static bool missing_module, query_failure;
static DWORD last_id;
static DWORD GetLastError(void) { return 5; }
static HMODULE GetModuleHandleA(const char* name)
{
  CHECK(strcmp(name, "ntdll.dll") == 0);
  ++module_calls;
  return missing_module ? NULL : &module_token;
}
static DWORD FormatMessageA(
    DWORD flags, const void* source, DWORD id, DWORD language,
    char* buffer, DWORD size, void* arguments)
{
  ++query_calls;
  last_id = id;
  CHECK(language == MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
  CHECK(arguments == NULL);
  CHECK((flags & FORMAT_MESSAGE_IGNORE_INSERTS) != 0);
  CHECK((flags & FORMAT_MESSAGE_MAX_WIDTH_MASK) == FORMAT_MESSAGE_MAX_WIDTH_MASK);
  const char* message = NULL;
  if ((flags & FORMAT_MESSAGE_FROM_HMODULE) != 0) {
    CHECK(source == &module_token && (flags & FORMAT_MESSAGE_FROM_SYSTEM) == 0);
    if (id == UINT32_C(0xC0000001)) message = "NT failure\r\n";
  } else {
    CHECK(source == NULL && (flags & FORMAT_MESSAGE_FROM_SYSTEM) != 0);
    if (id == UINT32_C(0x80004005)) message = "COM failure\r\n";
    if (id == 5) message = "Access denied\r\n";
  }
  if (query_failure || message == NULL || strlen(message) >= size) return 0;
  strcpy(buffer, message);
  return (DWORD)strlen(message);
}
#define strerror_s test_strerror_s
static int strerror_s(char* buffer, size_t size, int value)
{
  (void)value;
  if (size != 0) buffer[0] = '\0';
  return EINVAL;
}
#elif X_IOS || X_MACOS
typedef int32_t kern_return_t;
typedef kern_return_t mach_error_t;
#define KERN_SUCCESS 0
static bool missing_message, empty_message;
static int32_t last_mach_code;
static const char* mach_error_string(mach_error_t value)
{
  ++query_calls;
  last_mach_code = value;
  return missing_message ? NULL : empty_message ? "" : "Mach failure %s %n %%";
}
#endif

#ifdef TEST_CPP_DISTRIBUTION
#include "cpp_native_error_under_test.h"
#else
#include "native_error_under_test.h"
#endif

static x_error make_error(const char* name, int32_t value)
{
#ifdef TEST_CPP_DISTRIBUTION
  return x_error(name, value);
#else
  return x_error_set(name, value);
#endif
}
static int32_t error_value(x_error error)
{
#ifdef TEST_CPP_DISTRIBUTION
  return error.value();
#else
  return error.value;
#endif
}
static const char* error_name(x_error error)
{
#ifdef TEST_CPP_DISTRIBUTION
  return error.domain();
#else
  return error.domain->name;
#endif
}
static const char* format_error(char* buffer, size_t size, x_error error)
{
#ifdef TEST_CPP_DISTRIBUTION
  return error.message(buffer, size);
#else
  return x_error_message(buffer, size, error);
#endif
}
static void check_value(const char* name, int32_t value, bool failure)
{
  x_error error = make_error(name, value);
  CHECK(error_value(error) == value && strcmp(error_name(error), name) == 0);
  CHECK(x_fail(error) == failure && x_succ(error) == !failure);
  CHECK(_x_error_domain_lookup(name)->capture == NULL);
}
static void check_buffers(const char* name, int32_t value)
{
  char storage[66];
  x_error error = make_error(name, value);
  const int before = query_calls;
  memset(storage, 'Z', sizeof(storage));
  CHECK(format_error(NULL, 1, error) == NULL);
  CHECK(format_error(storage + 1, 0, error) == storage + 1);
  CHECK(query_calls == before);
  const size_t sizes[] = {1, 4, 64};
  for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); ++i) {
    memset(storage, 'Z', sizeof(storage));
    CHECK(format_error(storage + 1, sizes[i], error) == storage + 1);
    CHECK(memchr(storage + 1, '\0', sizes[i]) != NULL);
    CHECK(storage[0] == 'Z');
    for (size_t j = sizes[i] + 1; j < sizeof(storage); ++j) CHECK(storage[j] == 'Z');
    CHECK(error_value(error) == value);
  }
}

int main(void)
{
  // The longest built-in numeric fallback must fit without internal truncation.
  char numeric[32];
  _x_error_number(numeric, sizeof(numeric), "CUDA driver error", INT32_MIN);
  CHECK(strcmp(numeric, "CUDA driver error -2147483648") == 0);
  CHECK(x_succ(x_ok()));
  CHECK(error_value(make_error("generic", 7)) == 7);
#if X_WINDOWS
  CHECK(error_domain_registry.count == 5);
  check_value("wincom", 0, false);
  check_value("wincom", 1, false); // S_FALSE is successful.
  check_value("wincom", INT32_MAX, false);
  check_value("wincom", (int32_t)UINT32_C(0x80004005), true);
  check_value("wincom", INT32_MIN, true);
  check_value("winnt", 0, false);
  check_value("winnt", INT32_C(0x3fffffff), false); // Success.
  check_value("winnt", INT32_C(0x40000000), false); // Informational.
  check_value("winnt", INT32_MAX, false);
  check_value("winnt", (int32_t)UINT32_C(0x80000005), true); // Warning.
  check_value("winnt", (int32_t)UINT32_C(0xC0000001), true); // Error.
  CHECK(_x_error_domain_lookup("mach") == NULL);
  char buffer[64];
  format_error(buffer, sizeof(buffer), make_error("wincom", (int32_t)UINT32_C(0x80004005)));
  CHECK(strcmp(buffer, "COM failure") == 0);
  int before = query_calls;
  format_error(buffer, sizeof(buffer), make_error("wincom", (int32_t)UINT32_C(0x80070005)));
  CHECK(strcmp(buffer, "Access denied") == 0 && query_calls == before + 2 && last_id == 5);
  format_error(buffer, sizeof(buffer), make_error("wincom", (int32_t)UINT32_C(0x80040005)));
  CHECK(strcmp(buffer, "HRESULT 0x80040005") == 0); // Do not unwrap other facilities.
  format_error(buffer, sizeof(buffer), make_error("winnt", (int32_t)UINT32_C(0xC0000001)));
  CHECK(strcmp(buffer, "NT failure") == 0 && module_calls == 1);
  format_error(buffer, sizeof(buffer), make_error("winnt", -1));
  CHECK(strcmp(buffer, "NTSTATUS 0xffffffff") == 0);
  missing_module = true;
  before = query_calls;
  format_error(buffer, sizeof(buffer), make_error("winnt", (int32_t)UINT32_C(0xC0000001)));
  CHECK(strcmp(buffer, "NTSTATUS 0xc0000001") == 0 && query_calls == before);
  missing_module = false;
  query_failure = true;
  format_error(buffer, sizeof(buffer), make_error("wincom", (int32_t)UINT32_C(0x80070005)));
  CHECK(strcmp(buffer, "HRESULT 0x80070005") == 0); // Preserve the original full code.
  query_failure = false;
  check_buffers("wincom", (int32_t)UINT32_C(0x80070005));
  check_buffers("winnt", (int32_t)UINT32_C(0xC0000001));
  check_buffers("wincom", -1);
  check_buffers("winnt", -1);
#elif X_IOS || X_MACOS
  CHECK(error_domain_registry.count == 3);
  CHECK(_x_error_domain_lookup("wincom") == NULL);
  CHECK(_x_error_domain_lookup("winnt") == NULL);
  check_value("mach", 0, false);
  check_value("mach", 5, true);
  check_value("mach", INT32_MIN, true);
  check_value("mach", INT32_MAX, true);
  char buffer[64];
  format_error(buffer, sizeof(buffer), make_error("mach", 5));
  CHECK(strcmp(buffer, "Mach failure %s %n %%") == 0 && last_mach_code == 5);
  check_buffers("mach", 5);
  missing_message = true;
  format_error(buffer, sizeof(buffer), make_error("mach", INT32_MIN));
  CHECK(strcmp(buffer, "Mach error -2147483648") == 0);
  check_buffers("mach", INT32_MIN);
  missing_message = false;
  empty_message = true;
  format_error(buffer, sizeof(buffer), make_error("mach", 5));
  CHECK(strcmp(buffer, "Mach error 5") == 0);
#else
  CHECK(error_domain_registry.count == 2);
  CHECK(_x_error_domain_lookup("wincom") == NULL);
  CHECK(_x_error_domain_lookup("winnt") == NULL);
  CHECK(_x_error_domain_lookup("mach") == NULL);
  (void)query_calls;
  // Keep shared helpers exercised even when platform domains are disabled.
  CHECK(strcmp(error_name(make_error("generic", 7)), "generic") == 0);
  check_value("generic", 7, true);
  check_buffers("generic", 7);
#endif
  return 0;
}
