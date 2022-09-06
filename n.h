/******************************************************************************
Copyright (c) 2022 Jihang Li
n.h is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan
PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details.


Last update: 2022-09-06 16:29
Version: v0.3.2
******************************************************************************/
#ifndef N_H
#define N_H

//******************************************************* Compiler Detection{{{
#define N_IS_CLANG (0)
#define N_IS_GCC (0)
#define N_IS_MSVC (0)

#if defined(__clang__)
#  undef N_IS_CLANG
#  define N_IS_CLANG (1)
#elif defined(__GNUC__)
#  undef N_IS_GCC
#  define N_IS_GCC (1)
#elif defined(_MSC_VER)
#  undef N_IS_MSVC
#  define N_IS_MSVC (1)
#endif
// Compiler Detection}}}

//************************************************************* OS Detection{{{
#define N_IS_CYGWIN (0)
#define N_IS_LINUX (0)
#define N_IS_WINDOWS (0)

#if defined(__CYGWIN__)
#  undef N_IS_CYGWIN
#  define N_IS_CYGWIN (1)
#elif defined(linux) || defined(__linux) || defined(__linux) \
     || defined(__gnu_linux__)
#  undef N_IS_LINUX
#  define N_IS_LINUX (1)
#elif defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) \
     || defined(__TOS_WIN__) || defined(__WINDOWS__)
#  undef N_IS_WINDOWS
#  define N_IS_WINDOWS (1)
#endif
// OS Detection}}}

//******************************************************* Platform Detection{{{
#define N_IS_ANDROID (0)
#define N_IS_MINGW32 (0)
#define N_IS_MINGW64 (0)

#if defined(__ANDROID__)
#  undef N_IS_ANDROID
#  define N_IS_ANDROID (1)
#elif defined(__MINGW32__)
#  undef N_IS_MINGW
#  define N_IS_MINGW (1)
#  if !defined(__MINGW64__)
#    undef N_IS_MINGW32
#    define N_IS_MINGW32 (1)
#  else
#    undef N_IS_MINGW64
#    define N_IS_MINGW64 (1)
#  endif
#endif
// Platform Detection}}}

#include <assert.h>
#include <errno.h>
#include <float.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#if N_IS_WINDOWS
#include <Windows.h>
#include <conio.h>
#include <process.h>
#include <synchapi.h>
#else
#include <limits.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <threads.h>
#include <unistd.h>
#endif

#ifdef __cplusplus  //******************************************** C++ Specific
#define N_INLINE inline

#define n_release(x) do { \
  if (x != nullptr) { \
    delete x; \
    x = nullptr; \
  } \
} while (false)

#define n_release_array(x) do { \
  if (x != nullptr) { \
    delete[] x; \
    x = nullptr; \
  } \
} while (false)
#else  //*********************************************************** C Specific
#define N_INLINE static inline
#endif

//**************************************************************** C and C++{{{
#ifdef __cplusplus
extern "C" {
#endif
//***************************************************** Special Declarations{{{
N_INLINE const char* n_full_path(char* dst, const char* src);

N_INLINE errno_t n_log_to(const char* file, const char* format, ...);

N_INLINE struct timespec n_now();

N_INLINE bool n_string_empty(const char* string);

N_INLINE const char* n_timestamp(char* buffer, const size_t size);
// Special Declarations}}}

//******************************************************* N_EXPORT, N_IMPORT{{{
#ifndef N_EXPORT
#if N_IS_WINDOWS
#define N_EXPORT __declspec(dllexport)
#else
#define N_EXPORT __attribute__ ((visibility("default")))
#endif
#endif

#ifndef N_IMPORT
#if N_IS_WINDOWS
#define N_IMPORT __declspec(dllimport)
#else
#define N_IMPORT __attribute__ ((visibility("hidden")))
#endif
#endif
// N_EXPORT, N_IMPORT}}}

//******************************************************************** N_KEY{{{
#define N_KEY_ESC   (0x1B)
#define N_KEY_A     (0x41)
#define N_KEY_B     (0x42)
#define N_KEY_C     (0x43)
#define N_KEY_D     (0x44)
#define N_KEY_Q     (0x51)
#if N_IS_WINDOWS
#define N_KEY_LEFT  (0x4B)
#define N_KEY_UP    (0x48)
#define N_KEY_RIGHT (0x4D)
#define N_KEY_DOWN  (0x50)
#else
#define N_KEY_LEFT  (-1)
#define N_KEY_UP    (-2)
#define N_KEY_RIGHT (-3)
#define N_KEY_DOWN  (-4)
#endif
// N_KEY}}}

//******************************************************************** n_log{{{
#ifdef NDEBUG
#ifndef N_LOG_LEVEL
#error N_LOG_LEVEL is not specified!
#endif
#else
#ifndef N_LOG_LEVEL
#define N_LOG_LEVEL (6)
#endif
#endif

#define _N_COLOR_BLACK   "\033[30m"
#define _N_COLOR_RED     "\033[31m"
#define _N_COLOR_GREEN   "\033[32m"
#define _N_COLOR_YELLOW  "\033[33m"
#define _N_COLOR_BLUE    "\033[34m"
#define _N_COLOR_MAGENTA "\033[35m"
#define _N_COLOR_CYAN    "\033[36m"
#define _N_COLOR_WHITE   "\033[37m"
#define _N_COLOR_RESET   "\033[0m"

#define _N_LOG_COLOR_P _N_COLOR_WHITE
#define _N_LOG_COLOR_F _N_COLOR_MAGENTA
#define _N_LOG_COLOR_E _N_COLOR_RED
#define _N_LOG_COLOR_W _N_COLOR_YELLOW
#define _N_LOG_COLOR_I _N_COLOR_GREEN
#define _N_LOG_COLOR_D _N_COLOR_BLUE

#if N_IS_WINDOWS
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

N_INLINE errno_t _n_log_prefix(
    char* buffer, size_t* size, const char level, const char* filename,
    const char* function, const long line)
{
  if (buffer == NULL || size == NULL)
  {
    return EINVAL;
  }

  char timestamp[26] = {0};
  n_timestamp(timestamp, 26);

  size_t bytes = 0;

#ifdef NDEBUG
  bytes = (size_t)snprintf(buffer, size, "[%c %s] ", level, timestamp) + 1;
#else
  bytes = (size_t)snprintf(
      buffer, *size, "[%c %s | %s - %s - %ld] ",
      level, timestamp, filename, function, line) + 1;
#endif

  if (bytes > *size)
  {
    *size = bytes;
    return ENOBUFS;
  }

  return 0;
}

N_INLINE void _n_log_internal(
    const char level, const char* filename, const char* function,
    const long line, const char* file, const char* format, ...)
{
  char lvl = (char)toupper(level);
  char color_level[8] = {0};
  char color_reset[8] = {0};

  if ('P' == lvl)
  {
#if N_LOG_LEVEL >= 1
    snprintf(color_level, 8, _N_LOG_COLOR_P);
#else
    return;
#endif
  }
  else if ('F' == lvl)
  {
#if N_LOG_LEVEL >= 2
    snprintf(color_level, 8, _N_LOG_COLOR_F);
#else
    return;
#endif
  }
  else if ('E' == lvl)
  {
#if N_LOG_LEVEL >= 3
    snprintf(color_level, 8, _N_LOG_COLOR_E);
#else
    return;
#endif
  }
  else if ('W' == lvl)
  {
#if N_LOG_LEVEL >= 4
    snprintf(color_level, 8, _N_LOG_COLOR_W);
#else
    return;
#endif
  }
  else if ('I' == lvl)
  {
#if N_LOG_LEVEL >= 5
    snprintf(color_level, 8, _N_LOG_COLOR_I);
#else
    return;
#endif
  }
  else if ('D' == lvl)
  {
#if N_LOG_LEVEL >= 6
    snprintf(color_level, 8, _N_LOG_COLOR_D);
#else
    return;
#endif
  }

  snprintf(color_reset, 8, _N_COLOR_RESET);

  size_t prefix_sz = 256;
  char* prefix = (char*)malloc(prefix_sz);

  errno_t err = _n_log_prefix(prefix, &prefix_sz, lvl, filename, function, line);
  if (err < 0)
  {
    free(prefix);
    prefix = (char*)malloc(prefix_sz);
    _n_log_prefix(prefix, &prefix_sz, lvl, filename, function, line);
  }

  size_t msg_sz = 256;
  char* msg = (char*)malloc(msg_sz);
  size_t bytes = 0;

  va_list data;
  va_start(data, format);

  bytes = (size_t)vsnprintf(msg, msg_sz, format, data) + 1;
  if (bytes > msg_sz)
  {
    free(msg);
    msg = (char*)malloc(bytes);
    vsnprintf(msg, bytes, format, data);
  }

  va_end(data);

  if (!n_string_empty(file))
  {
    n_log_to(file, "%s%s\n", prefix, msg);
  }

  printf("%s%s%s%s\n", color_level, prefix, msg, color_reset);

  free(prefix);
  free(msg);
}

#define n_log(level, file, format, ...) do { \
  _n_log_internal(level, __FILENAME__, __FUNCTION__, __LINE__, file, format, ##__VA_ARGS__); \
} while (false)
// n_log}}}

#define n_array_size(a) ((sizeof(a) / sizeof(*(a))) / (size_t)(!(sizeof(a) % sizeof(*(a)))))

#ifdef NDEBUG
#define n_assert(expr) do { \
  if (!(expr)) { \
    char ts[26] = {0}; \
    char fp[256] = {0}; \
    fprintf(stderr, "\n[ASSERTION FAILURE %s | %s - %s - %d] \n%s", \
        n_timestamp(ts, 26), n_full_path(fp, __FILENAME__), __FUNCTION__, __LINE__, #expr); \
    abort(); } \
} while (false)
#else
#define n_assert(expr) do {assert(expr);} while (false)
#endif

#define n_bit(bit) (1 << bit)

#define n_pi(T) (T)(3.141592653589793238462643383279502884197169399375)

//***************************************************************** Function{{{
N_INLINE bool n_succ(const errno_t err)
{
  return (err == 0);
}

N_INLINE bool n_fail(const errno_t err)
{
  return (err != 0);
}

N_INLINE double n_duration(
    const struct timespec start, const struct timespec end, const char* unit)
{
  double diff = (double)(
      (end.tv_sec - start.tv_sec) * 1000000000LL + end.tv_nsec - start.tv_nsec);

  if (strcmp(unit, "h") == 0)
  {
    return diff / 3600000000000.0;
  }
  else if (strcmp(unit, "m") == 0)
  {
    return diff / 60000000000.0;
  }
  else if (strcmp(unit, "s") == 0)
  {
    return diff / 1000000000.0;
  }
  else if (strcmp(unit, "ms") == 0)
  {
    return diff / 1000000.0;
  }
  else if (strcmp(unit, "us") == 0)
  {
    return diff / 1000.0;
  }
  else  // if (strcmp(unit, "ns") == 0)
  {
    return diff;
  }
}

N_INLINE const char* n_full_path(char* dst, const char* src)
{
  if (dst == NULL)
  {
    return NULL;
  }

#if N_IS_WINDOWS
  return _fullpath(dst, src, _MAX_PATH);
#else
  return realpath(src, dst);
#endif
}

N_INLINE errno_t n_log_to(const char* file, const char* format, ...)
{
  if (n_string_empty(file))
  {
    return EINVAL;
  }

  FILE* stream = NULL;
  errno_t err = 0;

  err = fopen_s(&stream, file, "a");
  if (n_fail(err))
  {
    perror("fopen_s failed");
    return err;
  }

  va_list args;
  va_start(args, format);
  vfprintf(stream, format, args);
  va_end(args);

  if (stream != NULL)
  {
    err = fclose(stream);
    if (n_fail(err))
    {
      perror("fclose failed");
      return err;
    }
  }

  return err;
}

N_INLINE bool n_path_exists(const char* path)
{
  int result = -1;

#if N_IS_WINDOWS
  struct _stat buffer;
  result = _stat(path, &buffer);
#else
  struct stat buffer;
  result = stat(path, &buffer);
#endif

  return (result == 0);
}

#if !N_IS_WINDOWS
N_INLINE int _kbhit()
{
  static bool initialized = false;
  if (!initialized)
  {
    struct termios settings;
    tcgetattr(STDIN_FILENO, &settings);
    settings.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &settings);
    setbuf(stdin, NULL);
    initialized = true;
  }

  int bytes = 0;
  ioctl(STDIN_FILENO, FIONREAD, &bytes);
  return bytes;
}
#endif

N_INLINE int n_pressed_key()
{
  int key = 0;

#if N_IS_WINDOWS
  if (_kbhit())
  {
    key = toupper(_getch());
  }
  return key;
#else
  int bytes_waiting = _kbhit();
  if (bytes_waiting <= 0) { return 0; }

  struct termios old_settings = {0};
  struct termios new_settings = {0};
  union
  {
    int in;
    char ch[4];
  } buffer = {0};
  int err = 0;
  ssize_t bytes_read = 0;

  err = tcgetattr(0, &old_settings);
  if (err)
  {
    return 0;
  }

  new_settings = old_settings;
  new_settings.c_lflag &= ~ICANON;
  new_settings.c_lflag &= ~ECHO;

  err = tcsetattr(0, TCSANOW, &new_settings);
  if (err)
  {
    tcsetattr(0, TCSANOW, &old_settings);
    return 0;
  }

  bytes_read = read(STDIN_FILENO, &buffer.in, bytes_waiting);
  if (bytes_read <= 0)
  {
    tcsetattr(0, TCSANOW, &old_settings);
    return 0;
  }
  else if (bytes_read == 3)
  {
    if (buffer.ch[0] == 0x1B && buffer.ch[1] == 0x5B)
    {
      switch (buffer.ch[2])
      {
        case NKEY_A:
          key = NKEY_UP;
          break;
        case NKEY_B:
          key = NKEY_DOWN;
          break;
        case NKEY_C:
          key = NKEY_RIGHT;
          break;
        case NKEY_D:
          key = NKEY_LEFT;
          break;
      }
    }
    else
    {
      key = buffer.ch[0];
    }
  }
  else
  {
    key = buffer.ch[0];
  }

  tcsetattr(0, TCSADRAIN, &old_settings);

  if (isalpha(key))
  {
    return toupper(key);
  }
  else
  {
    return key;
  }
#endif
}

N_INLINE void n_sleep(const unsigned long ms)
{
#if N_IS_WINDOWS
  Sleep(ms);
#else
  struct timespec duration;
  duration.tv_nsec = (long)(ms * 1000000L);
  nanosleep(&duration, NULL);
#endif
}

N_INLINE bool n_string_empty(const char* string)
{
  return (string == NULL || string[0] == '\0');
}

N_INLINE const char* n_timestamp(char* buffer, const size_t size)
{
  time_t now = time(NULL);

  errno_t err = ctime_s(buffer, size, &now);
  if (n_fail(err))
  {
    return "";
  }

  buffer[strlen(buffer) - 1] = '\0';

  return buffer;
}
// Function}}}

//************************************************************** Concurrency{{{
#if N_IS_WINDOWS
enum
{
  thrd_success  = 0,
  thrd_busy     = 1,
  thrd_error    = 2,
  thrd_nomem    = 3,
  thrd_timedout = 4,
};

enum
{
  mtx_plain     = 0,
  mtx_recursive = 1,
  mtx_timed     = 2,
};

typedef CONDITION_VARIABLE cnd_t;
typedef HANDLE mtx_t;
typedef HANDLE thrd_t;
typedef unsigned int (*thrd_start_t)(void*) ;
#endif

enum n_thrd_ctrl_code
{
  THRD_NONE  = 0,
  THRD_JOIN  = 1,
  THRD_WAIT  = 2,
  THRD_EXIT  = 3,
};

struct n_mutex
{
  int type;
  mtx_t mtx;
};

struct n_thread
{
  enum n_thrd_ctrl_code ctl;
  thrd_t thd;
  cnd_t cnd;
  struct n_mutex mtx;
};

//******************************************************************** n_cnd{{{
N_INLINE int n_cnd_broadcast(cnd_t* cnd)
{
  if (cnd == NULL)
  {
    return thrd_error;
  }

#if N_IS_WINDOWS
  WakeAllConditionVariable(cnd);
  return thrd_success;
#else
  return cnd_broadcast(cnd);
#endif
}

N_INLINE void n_cnd_destroy(cnd_t* cnd)
{
  if (cnd == NULL)
  {
    return;
  }

#if N_IS_WINDOWS
  (void)cnd;
#else
  return cnd_destroy(cnd);
#endif
}

N_INLINE int n_cnd_init(cnd_t* cnd)
{
  if (cnd == NULL)
  {
    return thrd_error;
  }

#if N_IS_WINDOWS
  InitializeConditionVariable(cnd);
  if (cnd == NULL)
  {
    return thrd_error;
  }
  return thrd_success;
#else
  return cnd_init(cnd);
#endif
}

N_INLINE int n_cnd_signal(cnd_t* cnd)
{
  if (cnd == NULL)
  {
    return thrd_error;
  }

#if N_IS_WINDOWS
  WakeConditionVariable(cnd);
  return thrd_success;
#else
  return cnd_signal(cnd);
#endif
}

N_INLINE int n_cnd_timedwait(
    cnd_t* cnd, struct n_mutex* mutex, const struct timespec* time_point)
{
  if (cnd == NULL || mutex == NULL || time_point == NULL)
  {
    return thrd_error;
  }

#if N_IS_WINDOWS
  DWORD d = (DWORD)n_duration(n_now(), *time_point, "ms");
  BOOL succeeded = FALSE;

  if (mutex->type & mtx_recursive)
  {
    succeeded = SleepConditionVariableCS(cnd, (CRITICAL_SECTION*)mutex->mtx, d);
  }
  else
  {
    succeeded = SleepConditionVariableSRW(cnd, (SRWLOCK*)mutex->mtx, d, 0UL);
  }

  if (!succeeded)
  {
    if (GetLastError() == ERROR_TIMEOUT)
    {
      return thrd_timedout;
    }
    else {
      return thrd_error;
    }
  }

  return thrd_success;
#else
  return cnd_timedwait(cnd, mutex->mtx, time_point);
#endif
}

N_INLINE int n_cnd_wait(cnd_t* cnd, struct n_mutex* mutex)
{
  if (cnd == NULL || mutex == NULL)
  {
    return thrd_error;
  }

#if N_IS_WINDOWS
  if (mutex->type & mtx_recursive)
  {
    SleepConditionVariableCS(cnd, (CRITICAL_SECTION*)mutex->mtx, INFINITE);
  }
  else
  {
    SleepConditionVariableSRW(cnd, (SRWLOCK*)mutex->mtx, INFINITE, 0UL);
  }

  return thrd_success;
#else
  return cnd_wait(cnd, mutex->mtx);
#endif
}
// n_cnd}}}

//******************************************************************** n_mtx{{{
N_INLINE void n_mtx_destroy(struct n_mutex* mutex)
{
  if (mutex == NULL)
  {
    return;
  }

#if N_IS_WINDOWS
  if (mutex->type & mtx_recursive)
  {
    CRITICAL_SECTION* m = (CRITICAL_SECTION*)mutex->mtx;
    DeleteCriticalSection(m);
    free(m);
  }
  else
  {
    SRWLOCK* m = (SRWLOCK*)mutex->mtx;
    free(m);
  }
#else
  mtx_destroy(mutex->mtx);
#endif
}

N_INLINE int n_mtx_init(struct n_mutex* mutex, int type)
{
  if (mutex == NULL)
  {
    return thrd_error;
  }

#if N_IS_WINDOWS
  if (type & mtx_recursive)
  {
    mutex->mtx = malloc(sizeof(CRITICAL_SECTION));
    if (mutex->mtx == NULL)
    {
      return thrd_nomem;
    }
    InitializeCriticalSection((CRITICAL_SECTION*)mutex->mtx);
  }
  else
  {
    mutex->mtx = malloc(sizeof(SRWLOCK));
    if (mutex->mtx == NULL)
    {
      return thrd_nomem;
    }
    InitializeSRWLock((SRWLOCK*)mutex->mtx);
  }

  mutex->type = type;

  return thrd_success;
#else
  return mtx_init(mutex->mtx, type);
#endif
}

N_INLINE int n_mtx_lock(struct n_mutex* mutex)
{
  if (mutex == NULL || mutex->mtx == NULL)
  {
    return thrd_error;
  }

#if N_IS_WINDOWS
  if (mutex->type & mtx_recursive)
  {
    EnterCriticalSection((CRITICAL_SECTION*)mutex->mtx);
  }
  else
  {
    AcquireSRWLockExclusive((SRWLOCK*)mutex->mtx);
  }

  return thrd_success;
#else
  return mtx_lock(mutex->mtx);
#endif
}

N_INLINE int n_mtx_timedlock(struct n_mutex* mutex, const struct timespec* time_point)
{
  if (mutex == NULL || (mutex->type & mtx_timed) == 0 || time_point == NULL)
  {
    return thrd_error;
  }

#if N_IS_WINDOWS
  DWORD err = WaitForSingleObject(
      mutex->mtx, (DWORD)n_duration(n_now(), *time_point, "ms"));

  switch (err)
  {
    case WAIT_ABANDONED:
      return thrd_busy;
    case WAIT_TIMEOUT:
      return thrd_timedout;
    default:
      return thrd_error;
  }

  return thrd_success;
#else
  return mtx_timedlock(mutex->mtx, time_point);
#endif
}

N_INLINE int n_mtx_trylock(struct n_mutex* mutex)
{
  if (mutex == NULL)
  {
    return thrd_error;
  }

#if N_IS_WINDOWS
  if (mutex->type & mtx_recursive)
  {
    TryEnterCriticalSection((CRITICAL_SECTION*)mutex->mtx);
  }
  else
  {
    TryAcquireSRWLockExclusive((SRWLOCK*)mutex->mtx);
  }

  return thrd_success;
#else
  return mtx_trylock(mutex->mtx);
#endif
}

N_INLINE int n_mtx_unlock(struct n_mutex* mutex)
{
  if (mutex == NULL)
  {
    return thrd_error;
  }

#if N_IS_WINDOWS
  if (mutex->type & mtx_recursive)
  {
    LeaveCriticalSection((CRITICAL_SECTION*)mutex->mtx);
  }
  else
  {
    ReleaseSRWLockExclusive((SRWLOCK*)mutex->mtx);
  }

  return thrd_success;
#else
  return mtx_unlock(mutex->mtx);
#endif
}
// n_mtx}}}

//******************************************************************* n_thrd{{{
N_INLINE thrd_t n_thrd_current()
{
#if N_IS_WINDOWS
  return GetCurrentThread();
#else
  return thrd_current();
#endif
}

N_INLINE int n_thrd_detach(struct n_thread* thread)
{
  if (thread == NULL)
  {
    return thrd_error;
  }

#if N_IS_WINDOWS
  BOOL succeeded = CloseHandle(thread->thd);
  if (!succeeded)
  {
    return thrd_error;
  }
#else
  int result = thrd_detach(thread->thd);
  if (result != thrd_success)
  {
    return result;
  }
#endif

  n_cnd_destroy(&thread->cnd);
  n_mtx_destroy(&thread->mtx);

  return thrd_success;
}

N_INLINE bool n_thrd_equal(struct n_thread lhs, struct n_thread rhs)
{
  return lhs.thd == rhs.thd;
}

N_INLINE void n_thrd_exit(int exit_code)
{
#if N_IS_WINDOWS
  _endthreadex((unsigned int)exit_code);
#else
  thrd_exit(exit_code);
#endif
}

N_INLINE int n_thrd_init(struct n_thread* thread, thrd_start_t fn, void* arg)
{
  if (thread == NULL)
  {
    return thrd_error;
  }

  int result = thrd_success;

#if N_IS_WINDOWS
  thread->thd = (thrd_t)_beginthreadex(NULL, 0, fn, arg, 0, NULL);
  if (!thread->thd)
  {
    return thrd_error;
  }
#else
  result = thrd_create(&thread->thd, fn, arg);
  if (result != thrd_success)
  {
    return result;
  }
#endif

  thread->ctl = THRD_NONE;

  result = n_cnd_init(&thread->cnd);
  if (result != thrd_success)
  {
    return result;
  }

  result = n_mtx_init(&thread->mtx, mtx_plain);
  if (result != thrd_success)
  {
    return result;
  }

  return thrd_success;
}

N_INLINE int n_thrd_join(struct n_thread* thread, int* exit_code)
{
  if (thread == NULL)
  {
    return thrd_error;
  }

#if N_IS_WINDOWS
  if (WaitForSingleObject(thread->thd, INFINITE) != WAIT_OBJECT_0)
  {
    return thrd_error;
  }

  DWORD result = 0UL;
  if (GetExitCodeThread(thread->thd, &result) == 0)
  {
    return thrd_error;
  }

  if (exit_code != NULL)
  {
    *exit_code = (int)result;
  }

  return n_thrd_detach(thread);
#else
  return thrd_join(thread->thd, exit_code);
#endif
}

N_INLINE int n_thrd_wait(struct n_thread* thread, bool ready)
{
  if (thread == NULL)
  {
    return thrd_error;
  }

  n_mtx_lock(&thread->mtx);

  while (!ready)
  {
    n_cnd_wait(&thread->cnd, &thread->mtx);
  }

  n_mtx_unlock(&thread->mtx);

  return thrd_success;
}

N_INLINE int n_thrd_yield()
{
#if N_IS_WINDOWS
  return SwitchToThread() ? thrd_success : thrd_error;
#else
  thrd_yield();
  return thrd_success;
#endif
}
// n_thrd}}}
// Concurrency}}}

//******************************************************************* n_cmem{{{
struct n_cmem
{
  bool full;
  size_t capacity;
  size_t rptr;  // read pointer (head)
  size_t wptr;  // write pointer (tail)
  struct n_mutex mutex;
  uint8_t* buffer;
};

N_INLINE void n_cmem_clear(struct n_cmem* cm)
{
  if (cm == NULL)
  {
    return;
  }

  n_mtx_lock(&cm->mutex);

  cm->full = false;
  cm->rptr = 0;
  cm->wptr = 0;

  n_mtx_unlock(&cm->mutex);
}

N_INLINE void n_cmem_destroy(struct n_cmem* cm)
{
  if (cm == NULL)
  {
    return;
  }

  n_mtx_destroy(&cm->mutex);
  free(cm->buffer);
}

N_INLINE errno_t n_cmem_get(void* buffer, const size_t size, struct n_cmem* cm)
{
  if (cm == NULL || cm->buffer == NULL || size < 1)
  {
    return EINVAL;
  }

  n_mtx_lock(&cm->mutex);

  size_t avail = 0;

  if (cm->full)
  {
    avail = cm->capacity;
  }
  else
  {
    if (cm->wptr >= cm->rptr)
    {
      avail = cm->wptr - cm->rptr;
    }
    else
    {
      avail = cm->capacity - cm->rptr + cm->wptr;
    }
  }

  if (avail < size)
  {
    n_mtx_unlock(&cm->mutex);
    return ENODATA;
  }

  uint8_t* start = cm->buffer + cm->rptr;
  size_t left = cm->capacity - cm->rptr;

  if (left >= size)
  {
    memcpy(buffer, start, size);
  }
  else
  {
    memcpy(buffer, start, left);
    memcpy((uint8_t*)buffer + left, cm->buffer, size - left);
  }

  cm->rptr = (cm->rptr + size) % cm->capacity;
  cm->full = false;

  n_mtx_unlock(&cm->mutex);

  return 0;
}

N_INLINE errno_t n_cmem_init(struct n_cmem* cm, const size_t size)
{
  if (cm == NULL || size < 1)
  {
    return EINVAL;
  }

  cm->buffer = (uint8_t*)malloc(size);
  if (cm->buffer == NULL)
  {
    return ENOMEM;
  }

  int err = n_mtx_init(&cm->mutex, mtx_plain);
  if (n_fail(err))
  {
    free(cm->buffer);

    switch (err)
    {
      case thrd_nomem:
        return ENOMEM;
      case thrd_timedout:
        return ETIMEDOUT;
      case thrd_busy:
        return EAGAIN;
      default:
        return EAGAIN;
    }
  }

  cm->full = false;
  cm->capacity = size;
  cm->rptr = 0;
  cm->wptr = 0;

  return 0;
}

N_INLINE errno_t n_cmem_put(struct n_cmem* cm, const void* buffer, const size_t size)
{
  if (cm == NULL || cm->buffer == NULL || size < 1)
  {
    return EINVAL;
  }

  n_mtx_lock(&cm->mutex);

  if (size > cm->capacity || cm->full)
  {
    n_mtx_unlock(&cm->mutex);
    return ENOMEM;
  }

  size_t used = 0;

  if (cm->full)
  {
    used = cm->capacity;
  }
  else
  {
    if (cm->wptr >= cm->rptr)
    {
      used = cm->wptr - cm->rptr;
    }
    else
    {
      used = cm->capacity - cm->rptr + cm->wptr;
    }
  }

  size_t free = cm->capacity - used;

  if (free < size)
  {
    n_mtx_unlock(&cm->mutex);
    return ENOMEM;
  }

  uint8_t* start = cm->buffer + cm->wptr;

  if (cm->wptr >= cm->rptr)
  {
    size_t left = cm->capacity - cm->wptr;

    if (left >= size)
    {
      memcpy(start, buffer, size);
    }
    else
    {
      memcpy(start, buffer, left);
      memcpy(cm->buffer, (uint8_t*)buffer + left, size - left);
    }
  }
  else
  {
    memcpy(start, buffer, size);
  }

  cm->wptr = (cm->wptr + size) % cm->capacity;
  if (cm->rptr == cm->wptr)
  {
    cm->full = true;
  }

  n_mtx_unlock(&cm->mutex);

  return 0;
}

N_INLINE size_t n_cmem_size(struct n_cmem* cm)
{
  if (cm == NULL)
  {
    return 0;
  }

  n_mtx_lock(&cm->mutex);

  size_t size = 0;

  if (cm->full)
  {
    size = cm->capacity;
  }
  else
  {
    if (cm->wptr >= cm->rptr)
    {
      size = cm->wptr - cm->rptr;
    }
    else
    {
      size = cm->capacity - cm->rptr + cm->wptr;
    }
  }

  n_mtx_unlock(&cm->mutex);

  return size;
}
// n_cmem}}}

//****************************************************************** n_timer{{{
struct n_timer
{
  struct timespec start;
  double elapsed;

  struct
  {
    bool ready;
    long long cyc;
    double sum;
    double avg;
    struct
    {
      long long index;
      double value;
    } max, min;
  } report;
};

N_INLINE struct timespec n_now()
{
  struct timespec ts;
  timespec_get(&ts, TIME_UTC);
  return ts;
}

N_INLINE errno_t n_timer_init(struct n_timer* timer)
{
  if (timer == NULL)
  {
    return EINVAL;
  }

  timer->start = n_now();
  timer->elapsed = 0.0;
  timer->report.ready = false;
  timer->report.cyc = 0LL;
  timer->report.sum = 0.0;
  timer->report.avg = 0.0;
  timer->report.max.index = 0LL;
  timer->report.max.value = DBL_MIN;
  timer->report.min.index = 0LL;
  timer->report.min.value = DBL_MAX;

  return 0;
}

N_INLINE errno_t n_tic(struct n_timer* timer, const bool echo)
{
  if (timer == NULL)
  {
    return EINVAL;
  }

  timer->start = n_now();

  if (echo)
  {
    char ts[26] = {0};
    printf("Timer starts at: %s.\n", n_timestamp(ts, 26));
  }

  return 0;
}

N_INLINE errno_t n_toc(struct n_timer* timer, const bool echo, const char* unit)
{
  if (timer == NULL)
  {
    return EINVAL;
  }

  if (n_string_empty(unit))
  {
    unit = "ms";
  }

  timer->elapsed = n_duration(timer->start, n_now(), unit);

  if (echo)
  {
    char ts[26] = {0};
    printf("Timer stops at:  %s.\n", n_timestamp(ts, 26));
    printf("Time elapsed: %f%s.\n", timer->elapsed, unit);
  }

  return 0;
}

N_INLINE errno_t n_toc_profile(
    struct n_timer* timer, const bool echo, const long long cycle,
    const char* unit, const char* title)
{
  if (cycle <= 0LL || timer == NULL)
  {
    return EINVAL;
  }

  if (n_string_empty(unit))
  {
    unit = "ms";
  }

  if (n_string_empty(title))
  {
    title = "";
  }

  errno_t err = n_toc(timer, false, unit);
  if (n_fail(err))
  {
    return err;
  }

  if (timer->elapsed > timer->report.max.value)
  {
    timer->report.max.index = timer->report.cyc;
    timer->report.max.value = timer->elapsed;
  }
  if (timer->elapsed < timer->report.min.value)
  {
    timer->report.min.index = timer->report.cyc;
    timer->report.min.value = timer->elapsed;
  }

  timer->report.sum += timer->elapsed;
  timer->report.cyc += 1LL;
  timer->report.avg = timer->report.sum / timer->report.cyc;

  if (timer->report.cyc % cycle == 0LL)
  {
    timer->report.ready = true;

    if (echo)
    {
      char buf[512] = {0};
      char num[64] = {0};
      size_t bsz = sizeof(buf);
      size_t nsz = sizeof(num);

      // [(title)]
      if (!n_string_empty(title))
      {
        strcat_s(buf, bsz, "[");
        strcat_s(buf, bsz, title);
        strcat_s(buf, bsz, "] ");
      }

      // [(title)] a cycles - total:
      snprintf(num, nsz, "%lld", timer->report.cyc);
      strcat_s(buf, bsz, num);
      strcat_s(buf, bsz, " cycles - total: ");

      // [title] a cycles - total: b(unit), avg:
      snprintf(num, nsz, "%f", timer->report.sum);
      strcat_s(buf, bsz, num);
      strcat_s(buf, bsz, unit);
      strcat_s(buf, bsz, ", avg: ");

      // [title] a cycles - total: b(unit), avg: c(unit), min(
      snprintf(num, nsz, "%f", timer->report.avg);
      strcat_s(buf, bsz, num);
      strcat_s(buf, bsz, unit);
      strcat_s(buf, bsz, ", min (");

      // [title] a cycles - total: b(unit), avg: c (unit), min((index)):
      snprintf(num, nsz, "%lld", timer->report.min.index);
      strcat_s(buf, bsz, num);
      strcat_s(buf, bsz, "): ");

      // [title] a cycles - total: b(unit), avg: c (unit), min((index)): (value)(unit), max (
      snprintf(num, nsz, "%f", timer->report.min.value);
      strcat_s(buf, bsz, num);
      strcat_s(buf, bsz, unit);
      strcat_s(buf, bsz, ", max (");

      // [title] a cycles - total: b(unit), avg: c (unit), min((index)): (value)(unit), max ((index):
      snprintf(num, nsz, "%lld", timer->report.max.index);
      strcat_s(buf, bsz, num);
      strcat_s(buf, bsz, "): ");

      // [title] a cycles - total: b(unit), avg: c (unit), min((index)): (value)(unit), max ((index)): (value)(unit)
      snprintf(num, nsz, "%f", timer->report.max.value);
      strcat_s(buf, bsz, num);
      strcat_s(buf, bsz, unit);

      printf("%s\n", buf);
    }

    n_timer_init(timer);
  }

  return 0;
}
// n_timer}}}
#ifdef __cplusplus
}
#endif
// C and C++}}}

#endif  // N_H
