#include "x.h"


// #define x_fmt(buffer, size, format, ...) do { \
//   if (buffer == NULL || size == 0) { \
//     break; \
//   } \
//   if (x_argc(format, ##__VA_ARGS__) == 1) { \
//     printf("WTF\n"); \
//     memcpy(buffer, format, size < strlen(format) ? size : strlen(format)); \
//   } else { \
//     const char* substr = NULL; \
//     size_t offset = 0; \
//     size_t begin = 0; \
//     size_t end = 0; \
//     while ((substr = strstr(format + begin, "{}")) != NULL) { \
//       end = strlen(format) - strlen(substr); \
//       memcpy(buffer + offset, format + begin, end - begin); \
//       offset += end - begin; \
//       _x_fmt_to_string(&offset, buffer + offset, size - offset, ##__VA_ARGS__); \
//       begin = end + 2; \
//       printf("WTH %s - (%zu, %zu, %zu)\n", substr, offset, begin, end); \
//     } \
//   } \
// } while (false)


int main()
{
  float a = x_Pi(float);
  int b = x_Pi(int);

  x_log('w', NULL, "Hello world!");
  x_log('i', NULL, "float: %.5f, int: %d", a, b);

  char dst[256] = {0};
  _x_fmt_process(dst, 256, "Hello {} world!", 1);

  // size_t offset = 0;
  // _x_fmt_to_string(&offset, dst, 256, "hello world {}", 1);


  x_log('w', NULL, dst);


  return 0;
}
