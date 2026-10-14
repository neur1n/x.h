/* Integration tests shared by independently extracted module combinations. */
#define X_ERROR_DOMAIN_IMPLEMENTATION
#include "generated_under_test.h"
#include "generated_under_test.h"

#if X_H != x_version(0, 9, 2)
#error "X_H must retain the encoded header version"
#endif
#if defined(X_H_MAJOR) || defined(X_H_MINOR) || defined(X_H_PATCH)
#error "Version component macros must not be reintroduced"
#endif

#define CHECK(condition) do { \
  if (!(condition)) { \
    fprintf(stderr, "Check failed: %s (%s:%d)\n", #condition, __FILE__, __LINE__); \
    return 1; \
  } \
} while (false)

extern int32_t register_peer_domain(void);

int main(void)
{
  char message[128] = {0};
  CHECK(strcmp(X_H_VERBOSE, TEST_DESCRIPTION) == 0);
  x_assert(true, NULL);
  CHECK(x_succ(x_ok()));
  CHECK(register_peer_domain() == 0);
  x_error err = x_error_set("generated-peer", 7);
  CHECK(x_fail(err));
  CHECK(strcmp(x_error_message(message, sizeof(message), err), "peer:7") == 0);
#ifdef TEST_LOGGING
  /* Exercise optional logging without writing output to the console. */
  FILE* output = tmpfile();
  CHECK(output != NULL);
  x_log('e', output, "Logging is present");
  CHECK(ftell(output) > 0);
  CHECK(fclose(output) == 0);
#else
#ifdef x_log
#error "Logging must not be included unless selected."
#endif
#endif

#ifdef TEST_CHECKSUM
  CHECK(x_checksum_crc32("123456789", 9, NULL) == UINT32_C(0xCBF43926));
#endif
#ifdef TEST_DATE_TIME
  char timestamp[26] = {0};
  CHECK(x_timestamp(timestamp, sizeof(timestamp)) == timestamp);
  x_event event = {0};
  CHECK(x_succ(x_event_init(&event, "cpu", 0)));
  CHECK(x_succ(event.tic(&event, NULL, 0)));
  CHECK(x_succ(event.toc(&event, NULL, 0)));
  CHECK(event.dur(&event, "ns") >= 0.0);
  CHECK(x_succ(x_event_term(&event)));
#endif
#ifdef TEST_FILESYSTEM
  CHECK(x_fexist("."));
  CHECK(x_fail(x_fopen(NULL, "unused", "r")));
#endif
#ifdef TEST_HARDWARE
  // A native query may legitimately fail and return zero.
  (void)x_ncpu();
#endif
#ifdef TEST_MATH
  CHECK(x_gcd(1536, 1024) == 512);
  CHECK(x_lcm(1536, 1024) == 3072);
#endif
#ifdef TEST_MEMORY
  void* memory = NULL;
  CHECK(x_succ(x_malloc(&memory, 8)));
  CHECK(memory != NULL);
  x_free(memory);
  CHECK(memory == NULL);
#endif
#ifdef TEST_STRING
  char copied[8] = {0};
  CHECK(x_succ(x_strcpy(copied, sizeof(copied), "abcdefghi")));
  CHECK(strcmp(copied, "abcdefg") == 0);
#endif

  /* SDK substitutes exercise Core with every GPU error domain enabled while
     the CUDA utilities, timing, and memory modules are absent. */
#if X_ENABLE_CUDA_DRIVER
  err = x_error_set("cudadrv", CUDA_SUCCESS);
  CHECK(x_succ(err));
  err = x_error_set("cudadrv", CUDA_ERROR_TEST);
  CHECK(x_fail(err));
  CHECK(strcmp(x_error_message(message, sizeof(message), err), "driver:test") == 0);
#endif
#if X_ENABLE_CUDA_RUNTIME
  test_cuda_last_error = cudaErrorTest;
  err = x_error_set("cudart");
  CHECK(err.value == cudaErrorTest && x_fail(err));
  CHECK(test_cuda_capture_calls == 1 && test_cuda_last_error == cudaSuccess);
  CHECK(x_succ(x_error_set("cudart")) && test_cuda_capture_calls == 2);
  CHECK(err.value == cudaErrorTest);
  err = x_error_set("cudart", cudaSuccess);
  CHECK(x_succ(err));
  err = x_error_set("cudart", cudaErrorTest);
  CHECK(x_fail(err));
  CHECK(strcmp(x_error_message(message, sizeof(message), err), "runtime:test") == 0);
  CHECK(test_cuda_capture_calls == 2);
#endif
#if X_ENABLE_CUBLAS
  err = x_error_set("cublas", CUBLAS_STATUS_SUCCESS);
  CHECK(x_succ(err));
  err = x_error_set("cublas", CUBLAS_STATUS_TEST);
  CHECK(x_fail(err));
  CHECK(strcmp(x_error_message(message, sizeof(message), err), "cublas:test") == 0);
#endif
  return 0;
}
