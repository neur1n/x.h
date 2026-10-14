#if TEST_MALLOC_FAULTS
// Load real Core and system headers before injecting only the Memory section.
#include "generated_under_test.h"
static size_t allocation_calls{0}, allocation_size{0};
static void* test_malloc(size_t size)
{
  ++allocation_calls;
  allocation_size = size;
  errno = ENOMEM;
  return nullptr;
}
#include "cpp_memory_under_test.h"
#elif TEST_GENERATED
#include "generated_under_test.h"
#else
#include "../../cpp/x.h"
#endif
#include <initializer_list>
#include "support.h"

static void check_invalid(const x_error& error)
{
  CHECK(x_fail(error));
  CHECK(error.value() == EINVAL && strcmp(error.domain(), "posix") == 0);
}

template<typename T, typename = void>
struct accepts_count : std::false_type {};
template<typename T>
struct accepts_count<T, std::void_t<decltype(x_count(std::declval<T&>()))>>
  : std::true_type {};

static void check_count()
{
  int array[3]{};
  const int constant[4]{};
  volatile int qualified[5]{};
  int matrix[2][3]{};
  static_assert(x_count(array) == 3);
  static_assert(x_count(constant) == 4);
  static_assert(x_count(qualified) == 5);
  static_assert(x_count(matrix) == 2 && x_count(matrix[0]) == 3);
  static_assert(!accepts_count<int*>::value);
  static_assert(!accepts_count<const int*>::value);
}

struct tracked
{
  inline static int destroyed{0};
  ~tracked() { ++destroyed; }
};

static void check_delete()
{
  tracked* object{new tracked};
  x_delete<false>(object);
  CHECK(object == nullptr && tracked::destroyed == 1);
  x_delete<false>(object);
  CHECK(tracked::destroyed == 1);
  tracked* array{new tracked[3]};
  x_delete<true>(array);
  CHECK(array == nullptr && tracked::destroyed == 4);
  x_delete<true>(array);
  CHECK(tracked::destroyed == 4);
}

template<typename T>
static void check_allocation()
{
  T* pointer{nullptr};
  x_free(pointer);
  CHECK(pointer == nullptr);
  check_invalid(x_malloc(&pointer, 0));
  check_invalid(x_malloc<T>(nullptr, 16));
#if TEST_MALLOC_FAULTS
  const size_t before{allocation_calls};
  errno = EBUSY;
  const x_error error{x_malloc(&pointer, 16)};
  CHECK(x_fail(error) && error.value() == ENOMEM);
  CHECK(strcmp(error.domain(), "posix") == 0);
  CHECK(pointer == nullptr && allocation_calls == before + 1);
  CHECK(allocation_size == 16);
  // An occupied output must be rejected before entering the allocator.
  int storage{0};
  void* occupied{&storage};
  check_invalid(x_malloc(&occupied, 16));
  CHECK(occupied == &storage && allocation_calls == before + 1);
#else
  CHECK(x_succ(x_malloc(&pointer, 16)) && pointer != nullptr);
  T* const before{pointer};
  check_invalid(x_malloc(&pointer, 16));
  CHECK(pointer == before);
  x_free(pointer);
  CHECK(pointer == nullptr);
  x_free(pointer);
#endif
}

static void check_free_evaluation()
{
  void* pointers[2]{nullptr, nullptr};
  size_t index{0};
  x_free(pointers[index++]);
  CHECK(index == 1 && pointers[0] == nullptr && pointers[1] == nullptr);
}

static void check_copy()
{
  char unchanged[]{"unchanged"};
  check_invalid(x_memcpy(nullptr, unchanged, 1));
  check_invalid(x_memcpy(unchanged, nullptr, 1));
  CHECK(strcmp(unchanged, "unchanged") == 0);
  CHECK(x_succ(x_memcpy(nullptr, nullptr, 0)));

  // Independent byte-range oracle covers both directions and adjacency.
  for (size_t size{0}; size <= 16; ++size) {
    for (size_t offset{0}; offset <= 48; ++offset) {
      char buffer[64];
      char expected[sizeof(buffer)];
      for (size_t i{0}; i < sizeof(buffer); ++i) {
        buffer[i] = expected[i] = static_cast<char>(i);
      }
      bool overlap{false};
      for (size_t i{0}; i < size; ++i) {
        for (size_t j{0}; j < size; ++j) {
          if (16 + i == offset + j) {
            overlap = true;
          }
        }
      }
      const x_error error{x_memcpy(buffer + offset, buffer + 16, size)};
      if (overlap) {
        check_invalid(error);
      } else {
        CHECK(x_succ(error));
        for (size_t i{0}; i < size; ++i) {
          expected[offset + i] = static_cast<char>(16 + i);
        }
      }
      CHECK(memcmp(buffer, expected, sizeof(buffer)) == 0);
    }
  }
}

static void check_queries()
{
  size_t available{7}, total{9};
  check_invalid(x_meminfo("cpu", nullptr, nullptr));
  check_invalid(x_meminfo(nullptr, &available, &total));
  for (const char* type : {"", "missing", "cudadrv", "cudart"}) {
    const x_error error{x_meminfo(type, &available, &total)};
    CHECK(x_fail(error) && error.value() == ENOTSUP);
    CHECK(strcmp(error.domain(), "posix") == 0);
    CHECK(available == 7 && total == 9);
  }
#if X_LINUX || X_IOS || X_MACOS || X_WINDOWS
  CHECK(x_succ(x_meminfo("cpu", &available, &total)));
  CHECK(total > 0 && available <= total);
  CHECK(x_succ(x_meminfo("cpu", nullptr, &total)) && total > 0);
  CHECK(x_succ(x_meminfo("cpu", &available, nullptr)));
#else
  CHECK(x_meminfo("cpu", &available, &total).value() == ENOTSUP);
  CHECK(available == 7 && total == 9);
#endif
}

int main()
{
  check_count();
  check_delete();
  check_allocation<void>();
  check_allocation<char>();
  check_allocation<int>();
  check_allocation<const int>();
  check_allocation<volatile int>();
  check_allocation<const volatile int>();
#if TEST_MALLOC_FAULTS
  CHECK(allocation_calls == 6);
#endif
  check_free_evaluation();
  check_copy();
  check_queries();
  return 0;
}
