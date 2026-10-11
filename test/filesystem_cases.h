// Shared behavior checks for the maintained C/C++ headers and extracted module.
// The runner provides an isolated directory, including cleanup after failures.
#define CHECK(expression) do { \
  if (!(expression)) { \
    fprintf(stderr, "Check failed: %s (%s:%d)\n", \
        #expression, __FILE__, __LINE__); \
    exit(EXIT_FAILURE); \
  } \
} while (false)

static void check_error(const x_error error, int value)
{
  CHECK(x_fail(error));
#ifdef __cplusplus
  CHECK(error.value() == value);
  CHECK(strcmp(error.domain(), "posix") == 0);
#else
  CHECK(error.value == value);
  CHECK(error.domain != NULL);
  CHECK(strcmp(error.domain->name, "posix") == 0);
#endif
}

static void check_arguments(void)
{
  FILE* stream = stdout;
  check_error(x_fopen(NULL, "unused", "rb"), EINVAL);
  check_error(x_fopen(&stream, NULL, "rb"), EINVAL);
  CHECK(stream == stdout);
  check_error(x_fopen(&stream, "", "rb"), EINVAL);
  CHECK(stream == stdout);
  check_error(x_fopen(&stream, "unused", NULL), EINVAL);
  CHECK(stream == stdout);
  check_error(x_fopen(&stream, "unused", ""), EINVAL);
  CHECK(stream == stdout);

  char destination[16] = "unchanged";
  char before[sizeof(destination)];
  memcpy(before, destination, sizeof(before));
  errno = 0;
  CHECK(x_fpath(NULL, sizeof(destination), ".") == NULL && errno == EINVAL);
  errno = 0;
  CHECK(x_fpath(destination, 0, ".") == NULL && errno == EINVAL);
  CHECK(memcmp(destination, before, sizeof(before)) == 0);
  errno = 0;
  CHECK(x_fpath(destination, sizeof(destination), NULL) == NULL && errno == EINVAL);
  CHECK(memcmp(destination, before, sizeof(before)) == 0);
  errno = 0;
  CHECK(x_fpath(destination, sizeof(destination), "") == NULL && errno == EINVAL);
  CHECK(memcmp(destination, before, sizeof(before)) == 0);
  CHECK(!x_fexist(NULL) && !x_fexist(""));
  CHECK(x_fsize(NULL) == -1 && x_fsize("") == -1);
}

static void check_files(void)
{
  const char* file = "filesystem.bin";
  // Binary data includes zero, CR/LF, and high bytes to catch text-mode changes.
  const unsigned char data[] = {0, 13, 10, 0x80, 0xFF, 42};
  CHECK(x_fexist("."));
  CHECK(!x_fexist(file) && x_fsize(file) == -1);

  FILE* stream = NULL;
  CHECK(x_succ(x_fopen(&stream, file, "wb")) && stream != NULL);
  CHECK(fwrite(data, 1, sizeof(data), stream) == sizeof(data));
  CHECK(fclose(stream) == 0);
  CHECK(x_fexist(file));
  CHECK(x_fsize(file) == (int64_t)sizeof(data));

  // A stale errno must not turn a successful open into an error.
  stream = NULL;
  errno = EACCES;
  CHECK(x_succ(x_fopen(&stream, file, "rb")) && stream != NULL);
  unsigned char readback[sizeof(data)];
  CHECK(fread(readback, 1, sizeof(readback), stream) == sizeof(readback));
  CHECK(memcmp(readback, data, sizeof(data)) == 0);
  CHECK(fgetc(stream) == EOF && !ferror(stream));
  CHECK(fclose(stream) == 0);

  CHECK(x_succ(x_fopen(&stream, file, "wb")) && stream != NULL);
  CHECK(fclose(stream) == 0);
  CHECK(x_fsize(file) == 0);
  CHECK(remove(file) == 0);
  CHECK(!x_fexist(file) && x_fsize(file) == -1);
  stream = NULL;
  errno = EBUSY;
  check_error(x_fopen(&stream, file, "rb"), ENOENT);
  // An open failure has no cross-platform promise to preserve the output.
}

static void check_paths(void)
{
  char expected[4096];
  CHECK(x_fpath(expected, sizeof(expected), ".") == expected);
  const size_t len = strlen(expected);
  CHECK(len > 0 && len + 1 < sizeof(expected));
#if X_WINDOWS
  CHECK((len >= 3 && expected[1] == ':'
      && (expected[2] == '\\' || expected[2] == '/'))
      || (expected[0] == '\\' && expected[1] == '\\'));
#else
  CHECK(expected[0] == '/');
#endif

  char destination[sizeof(expected)];
  char before[sizeof(destination)];
  memset(destination, '!', sizeof(destination));
  CHECK(x_fpath(destination, len + 1, ".") == destination);
  CHECK(strcmp(destination, expected) == 0);
  CHECK(destination[len + 1] == '!');

  memset(destination, '!', sizeof(destination));
  memcpy(before, destination, sizeof(before));
  errno = 0;
  CHECK(x_fpath(destination, len, ".") == NULL && errno == ENOBUFS);
  CHECK(memcmp(destination, before, sizeof(before)) == 0);
  errno = 0;
  CHECK(x_fpath(destination, 1, ".") == NULL && errno == ENOBUFS);
  CHECK(memcmp(destination, before, sizeof(before)) == 0);

  // The resolver consumes the source before committing any destination bytes.
  memcpy(destination, ".", 2);
  CHECK(x_fpath(destination, sizeof(destination), destination) == destination);
  CHECK(strcmp(destination, expected) == 0);
  memcpy(destination + 4, ".", 2);
  CHECK(x_fpath(destination, sizeof(destination), destination + 4) == destination);
  CHECK(strcmp(destination, expected) == 0);
  memcpy(destination, ".", 2);
  CHECK(x_fpath(destination + 1, sizeof(destination) - 1, destination)
      == destination + 1);
  CHECK(strcmp(destination + 1, expected) == 0);

  // A missing child has deliberately different Windows/POSIX semantics.
  const char* missing = "missing-directory/child.bin";
  memset(destination, '!', sizeof(destination));
  memcpy(before, destination, sizeof(before));
  errno = 0;
#if X_WINDOWS
  CHECK(x_fpath(destination, sizeof(destination), missing) == destination);
  CHECK(strlen(destination) > strlen(missing));
#else
  CHECK(x_fpath(destination, sizeof(destination), missing) == NULL);
  CHECK(errno == ENOENT);
  CHECK(memcmp(destination, before, sizeof(before)) == 0);
#endif
}

#if TEST_RESOLVE_FAULTS
static void check_resolver_failures(void)
{
  char destination[16] = "unchanged";
  char before[sizeof(destination)];
  memcpy(before, destination, sizeof(before));
  const int errors[] = {EACCES, ENOMEM};
  for (size_t idx = 0; idx < sizeof(errors) / sizeof(errors[0]); ++idx) {
    test_resolve_error = errors[idx];
    const size_t calls = test_resolve_calls;
    errno = 0;
    CHECK(x_fpath(destination, sizeof(destination), ".") == NULL);
    CHECK(test_resolve_calls == calls + 1);
    CHECK(errno == errors[idx]);
    CHECK(memcmp(destination, before, sizeof(before)) == 0);
  }
  test_resolve_error = 0;
}
#endif

int main(void)
{
  check_arguments();
  check_files();
  check_paths();
#if TEST_RESOLVE_FAULTS
  check_resolver_failures();
#endif
  return 0;
}
