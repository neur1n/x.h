#define X_ERROR_DOMAIN_IMPLEMENTATION
// Include the header directly so no test-only declaration can mask a failure.
#include "../../c/x.h"

#define CHECK(expression) do { \
  if (!(expression)) { \
    fprintf(stderr, "Check failed: %s (%s:%d)\n", \
        #expression, __FILE__, __LINE__); \
    exit(EXIT_FAILURE); \
  } \
} while (false)

static void check_invalid(const x_error error)
{
  CHECK(x_fail(error));
  CHECK(error.value == EINVAL);
  CHECK(error.domain != NULL);
  CHECK(strcmp(error.domain->name, "posix") == 0);
}

static void check_arguments(void)
{
  char destination[16] = "unchanged";
  char before[sizeof(destination)];
  memcpy(before, destination, sizeof(before));
  check_invalid(x_strcpy(NULL, sizeof(destination), "abc"));
  check_invalid(x_strcpy(destination, sizeof(destination), NULL));
  check_invalid(x_strcpy(destination, 0, "abc"));
  CHECK(memcmp(destination, before, sizeof(before)) == 0);

  // Separate objects must work in either address order, including truncation.
  char source[16] = "abcdef";
  CHECK(x_succ(x_strcpy(destination, sizeof(destination), source)));
  CHECK(strcmp(destination, "abcdef") == 0);
  CHECK(x_succ(x_strcpy(source, 4, destination)));
  CHECK(strcmp(source, "abc") == 0);
  CHECK(x_succ(x_strcpy(destination, 1, source)));
  CHECK(destination[0] == '\0');
  CHECK(x_succ(x_strcpy(source, sizeof(source), destination)));
  CHECK(source[0] == '\0');
}

static void check_overlap_boundaries(void)
{
  // Sweep both address orders, identical pointers, adjacent ranges, empty
  // strings, truncation, and overlap involving only a terminating byte.
  const size_t source_offset = 16;
  for (size_t length = 0; length < 16; ++length) {
    for (size_t capacity = 1; capacity <= 16; ++capacity) {
      for (size_t destination_offset = 0; destination_offset <= 48;
          ++destination_offset) {
        char buffer[64];
        char expected[sizeof(buffer)];
        memset(buffer, '!', sizeof(buffer));
        for (size_t i = 0; i < length; ++i) {
          buffer[source_offset + i] = (char)('a' + i);
        }
        buffer[source_offset + length] = '\0';
        memcpy(expected, buffer, sizeof(expected));

        size_t copied = 0;
        while (copied < length && copied + 1 < capacity) {
          ++copied;
        }
        // Independent byte-by-byte oracle, rather than address subtraction.
        bool overlaps = false;
        for (size_t i = 0; i <= length; ++i) {
          for (size_t j = 0; j <= copied; ++j) {
            if (source_offset + i == destination_offset + j) {
              overlaps = true;
            }
          }
        }

        const x_error error = x_strcpy(
            buffer + destination_offset, capacity, buffer + source_offset);
        if (overlaps) {
          check_invalid(error);
        } else {
          CHECK(x_succ(error));
          for (size_t i = 0; i < copied; ++i) {
            expected[destination_offset + i] = (char)('a' + i);
          }
          expected[destination_offset + copied] = '\0';
        }
        CHECK(memcmp(buffer, expected, sizeof(buffer)) == 0);
      }
    }
  }
}

int main(void)
{
  check_arguments();
  check_overlap_boundaries();
  CHECK(x_strmty(NULL));
  CHECK(x_strmty(""));
  CHECK(!x_strmty("abc"));
  return EXIT_SUCCESS;
}
