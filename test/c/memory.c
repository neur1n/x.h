#define X_ERROR_DOMAIN_IMPLEMENTATION
#include "error_test_support.h"

int main(void)
{
  size_t available = 1, total = 2;
  CHECK(x_succ(x_meminfo("cpu", &available, &total)));
  CHECK(total > 0 && available <= total);
  CHECK(x_succ(x_meminfo("cpu", NULL, &total)));
  CHECK(total > 0);
  CHECK(x_succ(x_meminfo("cpu", &available, NULL)));
  CHECK(x_meminfo("cpu", NULL, NULL).value == EINVAL);
  available = 1;
  total = 2;
  CHECK(x_meminfo("unknown", &available, &total).value == ENOTSUP);
  CHECK(x_meminfo(NULL, &available, &total).value == EINVAL);
  CHECK(available == 1 && total == 2);

  void* memory = NULL;
  CHECK(x_succ(x_malloc(&memory, 16)) && memory != NULL);
  CHECK(x_malloc(&memory, 16).value == EINVAL);
  CHECK(x_succ(x_memcpy(memory, "test", 5)));
  CHECK(strcmp((const char*)memory, "test") == 0);
  CHECK(x_succ(x_memcpy(NULL, NULL, 0)));
  CHECK(x_memcpy(NULL, memory, 1).value == EINVAL);

  char ranges[8] = "abcdefg";
  CHECK(x_memcpy(ranges + 1, ranges, 4).value == EINVAL);
  CHECK(strcmp(ranges, "abcdefg") == 0);
  CHECK(x_memcpy(ranges, ranges + 1, 4).value == EINVAL);
  CHECK(strcmp(ranges, "abcdefg") == 0);
  CHECK(x_memcpy(ranges, ranges, 1).value == EINVAL);
  CHECK(x_succ(x_memcpy(ranges, ranges, 0)));
  CHECK(x_succ(x_memcpy(ranges + 4, ranges, 4)));
  CHECK(memcmp(ranges, "abcdabcd", sizeof(ranges)) == 0);
  CHECK(x_succ(x_memcpy(ranges, ranges + 4, 4)));
  CHECK(memcmp(ranges, "abcdabcd", sizeof(ranges)) == 0);

  x_free(memory);
  CHECK(memory == NULL);
  return 0;
}
