#ifndef X_TEST_CPP_SUPPORT_H
#define X_TEST_CPP_SUPPORT_H
// These checks intentionally remain active in Release/NDEBUG builds.
#define CHECK(expression) do { \
  if (!(expression)) { \
    fprintf(stderr, "Check failed: %s (%s:%d)\n", #expression, __FILE__, __LINE__); \
    exit(EXIT_FAILURE); \
  } \
} while (false)

template<typename Exception, typename Function>
void expect_throw(Function&& function)
{
  try { function(); }
  catch (const Exception&) { return; }
  CHECK(false);
}

inline std::string read_stream(FILE* stream)
{
  CHECK(fflush(stream) == 0);
  rewind(stream);
  std::string text;
  char buffer[512]{};
  while (fgets(buffer, sizeof(buffer), stream) != nullptr) { text += buffer; }
  CHECK(!ferror(stream));
  return text;
}
#endif
