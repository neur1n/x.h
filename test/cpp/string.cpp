#if TEST_GENERATED
#include "generated_under_test.h"
#else
#include "../../cpp/x.h"
#endif
#include "support.h"

static void check_invalid(const x_error& error)
{
  CHECK(x_fail(error));
  CHECK(error.value() == EINVAL && strcmp(error.domain(), "posix") == 0);
}

static void check_arguments()
{
  char destination[16]{"unchanged"};
  char before[sizeof(destination)];
  memcpy(before, destination, sizeof(before));
  check_invalid(x_strcpy(nullptr, sizeof(destination), "abc"));
  check_invalid(x_strcpy(destination, sizeof(destination), nullptr));
  check_invalid(x_strcpy(destination, 0, "abc"));
  CHECK(memcmp(destination, before, sizeof(before)) == 0);

  char source[16]{"abcdef"};
  CHECK(x_succ(x_strcpy(destination, sizeof(destination), source)));
  CHECK(strcmp(destination, "abcdef") == 0);
  CHECK(x_succ(x_strcpy(source, 4, destination)));
  CHECK(strcmp(source, "abc") == 0);
  CHECK(x_succ(x_strcpy(destination, 1, source)));
  CHECK(destination[0] == '\0');
  CHECK(x_succ(x_strcpy(source, sizeof(source), destination)));
  CHECK(source[0] == '\0');
}

static void check_overlap_boundaries()
{
  // Port the C suite's independent oracle: full source versus actual writes,
  // including terminators, not destination capacity or only copied characters.
  constexpr size_t source_offset{16};
  for (size_t length{0}; length < 16; ++length) {
    for (size_t capacity{1}; capacity <= 16; ++capacity) {
      for (size_t destination_offset{0}; destination_offset <= 48;
          ++destination_offset) {
        char buffer[64];
        char expected[sizeof(buffer)];
        memset(buffer, '!', sizeof(buffer));
        for (size_t i{0}; i < length; ++i) {
          buffer[source_offset + i] = static_cast<char>('a' + i);
        }
        buffer[source_offset + length] = '\0';
        memcpy(expected, buffer, sizeof(expected));

        size_t copied{0};
        while (copied < length && copied + 1 < capacity) {
          ++copied;
        }
        bool overlaps{false};
        for (size_t i{0}; i <= length; ++i) {
          for (size_t j{0}; j <= copied; ++j) {
            if (source_offset + i == destination_offset + j) {
              overlaps = true;
            }
          }
        }

        const x_error error{x_strcpy(
            buffer + destination_offset, capacity, buffer + source_offset)};
        if (overlaps) {
          check_invalid(error);
        } else {
          CHECK(x_succ(error));
          for (size_t i{0}; i < copied; ++i) {
            expected[destination_offset + i] = static_cast<char>('a' + i);
          }
          expected[destination_offset + copied] = '\0';
        }
        CHECK(memcmp(buffer, expected, sizeof(buffer)) == 0);
      }
    }
  }
}

int main()
{
  check_arguments();
  check_overlap_boundaries();
  CHECK(x_strmty(nullptr) && x_strmty(""));
  CHECK(!x_strmty("abc"));
  return 0;
}
