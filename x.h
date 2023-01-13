/******************************************************************************
Copyright (c) 2022 Jihang Li
x.h is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan
PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details.


Last update: 2023-01-13 16:00
Version: v0.4.1
******************************************************************************/
#ifndef X_H
#define X_H

//******************************************************* Compiler Detection{{{
#define X_IS_CLANG (0)
#define X_IS_GCC (0)
#define X_IS_MSVC (0)

#if defined(__clang__)
#  undef X_IS_CLANG
#  define X_IS_CLANG (1)
#elif defined(__GNUC__)
#  undef X_IS_GCC
#  define X_IS_GCC (1)
#elif defined(_MSC_VER)
#  undef X_IS_MSVC
#  define X_IS_MSVC (1)
#endif
// Compiler Detection}}}

//************************************************************* OS Detection{{{
#define X_IS_CYGWIN (0)
#define X_IS_LINUX (0)
#define X_IS_MACOS (0)
#define X_IS_WINDOWS (0)

#if defined(__CYGWIX__)
#  undef X_IS_CYGWIN
#  define X_IS_CYGWIN (1)
#elif defined(linux) || defined(__linux) || defined(__linux) \
  || defined(__gnu_linux__)
#  undef X_IS_LINUX
#  define X_IS_LINUX (1)
#elif defined(__APPLE__) && defined(__MACH__)
#  undef X_IS_MACOS
#  define X_IS_MACOS (1)
#elif defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) \
  || defined(__TOS_WIX__) || defined(__WINDOWS__)
#  undef X_IS_WINDOWS
#  define X_IS_WINDOWS (1)
#endif
// OS Detection}}}

//******************************************************* Platform Detection{{{
#define X_IS_ANDROID (0)
#define X_IS_MINGW32 (0)
#define X_IS_MINGW64 (0)

#if defined(__ANDROID__)
#  undef X_IS_ANDROID
#  define X_IS_ANDROID (1)
#elif defined(__MINGW32__)
#  undef X_IS_MINGW
#  define X_IS_MINGW (1)
#  if !defined(__MINGW64__)
#    undef X_IS_MINGW32
#    define X_IS_MINGW32 (1)
#  else
#    undef X_IS_MINGW64
#    define X_IS_MINGW64 (1)
#  endif
#endif
// Platform Detection}}}

#include <assert.h>
#include <errno.h>
#include <float.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#if X_IS_WINDOWS
#include <Windows.h>
#include <conio.h>
#include <fcntl.h>
#include <process.h>
#include <synchapi.h>
#else
#define _GNU_SOURCE
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#endif

//************************************************************** OS Specific{{{
// X_EXPORT, X_IMPORT
#if X_IS_WINDOWS
#define X_EXPORT __declspec(dllexport)
#else
#define X_EXPORT __attribute__ ((visibility("default")))
#endif

#if X_IS_WINDOWS
#define X_IMPORT __declspec(dllimport)
#else
#define X_IMPORT __attribute__ ((visibility("hidden")))
#endif

// X_KEY
#define X_KEY_ESC   (0x1B)
#define X_KEY_A     (0x41)
#define X_KEY_B     (0x42)
#define X_KEY_C     (0x43)
#define X_KEY_D     (0x44)
#define X_KEY_Q     (0x51)
#if X_IS_WINDOWS
#define X_KEY_LEFT  (0x4B)
#define X_KEY_UP    (0x48)
#define X_KEY_RIGHT (0x4D)
#define X_KEY_DOWN  (0x50)
#else
#define X_KEY_LEFT  (-1)
#define X_KEY_UP    (-2)
#define X_KEY_RIGHT (-3)
#define X_KEY_DOWN  (-4)
#endif

// Misc
#if X_IS_WINDOWS
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define __PRETTY_FUNCTION__ __FUNCSIG__

#define X_PATH_MAX _MAX_PATH
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define X_PATH_MAX PATH_MAX

#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS (64)
#endif
#endif
// OS Specific}}}

#ifdef __cplusplus  //******************************************** C++ Specific
#define X_INLINE inline

template<class T, size_t N>
size_t x_count(const T (&array)[N])
{
  return N;
}

template<class T>
void x_delete(T*& ptr)
{
  if (ptr != nullptr)
  {
    delete ptr;
    ptr = nullptr;
  }
}

template<class T>
void x_delete_array(T*& arr)
{
  if (arr != nullptr)
  {
    delete[] arr;
    arr = nullptr;
  }
}
#else  //*********************************************************** C Specific
#define X_INLINE static inline

#define x_count(a) ((sizeof(a) / sizeof(*(a))) / (size_t)(!(sizeof(a) % sizeof(*(a)))))
#endif  // __cplusplus

//******************************************************************* C and C++
#ifdef __cplusplus
extern "C" {
#endif

#ifdef NDEBUG
#define x_assert(expr) do { \
  if (!(expr)) { \
    fprintf(stderr, "%s:%s: %s: Assertion '%s' failed.\n", \
        __FILENAME__, __LINE__, __PRETTY__FUNCTION__, #expr); \
    abort(); } \
} while (false)
#else
#define x_assert(expr) do {assert(expr);} while (false)
#endif

#define x_bit(bit) (1 << (bit))

#define x_free(ptr) do { \
  if (ptr != NULL) { \
    free(ptr); \
    ptr = NULL; \
  } \
} while (false)

#define x_pi(T) (T)(3.141592653589793238462643383279502884197169399375)

size_t x_cpu_count();

double x_duration(
    const struct timespec start, const struct timespec end, const char* unit);

long long x_file_size(const char* file);

const char* x_full_path(char* dst, const char* src);

int x_getch();

struct timespec x_now();

bool x_path_exists(const char* path);

int x_splitpath(
    const char *path,
    char *root, const size_t rsz, char *dir, const size_t dsz,
    char *file, const size_t fsz, char *ext, const size_t esz);

void x_sleep(const unsigned long ms);

bool x_string_empty(const char* string);

const char* x_timestamp(char* buffer, const size_t size);

//******************************************************************** x_err{{{
enum
{
  x_err_posix  = 0,
  x_err_winsa  = x_bit(1),
  x_err_win32  = x_bit(1) + x_bit(2),
  x_err_custom = x_bit(3),
#if X_IS_WINDOWS
  x_err_system = x_err_win32,
#else
  x_err_system = x_err_posix,
#endif
};

#ifndef X_ERR_MSG_LIMIT
#define X_ERR_MSG_LIMIT (128)
#endif

typedef struct _x_err_
{
  int cat;
  long long val;
  char msg[X_ERR_MSG_LIMIT];
} x_err;

bool x_succ(const long long err);

bool x_fail(const long long err);

x_err x_ok();

x_err x_get_err(const int cat);

x_err x_set_err(const int cat, const long long val, ...);
// x_err}}}

//******************************************************************** x_log{{{
#define X_LOG_NONE   (-1)
#define X_LOG_PLAIN   (0)
#define X_LOG_FATAL   (1)
#define X_LOG_ERROR   (2)
#define X_LOG_WARNING (3)
#define X_LOG_INFO    (4)
#define X_LOG_DEBUG   (5)

#ifndef X_LOG_LEVEL
#ifdef NDEBUG
#define X_LOG_LEVEL X_LOG_INFO
#else
#define X_LOG_LEVEL X_LOG_DEBUG
#endif
#endif

#ifndef X_LOG_PREFIX_LIMIT
#ifdef NDEBUG
#define X_LOG_PREFIX_LIMIT (64)
#else
#define X_LOG_PREFIX_LIMIT (256)
#endif
#endif

#ifndef X_LOG_MSG_LIMIT
#define X_LOG_MSG_LIMIT (256)
#endif

#ifdef __cplusplus
extern "C" {
void _x_log_impl(
    const char* filename, const char* function, const long line,
    const char level, FILE* file, const char* format, ...);
}
#endif

#define x_log(level, file, format, ...) do { \
  _x_log_impl(__FILENAME__, __FUNCTION__, __LINE__, level, file, format, ##__VA_ARGS__); \
} while (false)
// x_log}}}

//*************************************************************** x_timestat{{{
typedef struct _x_timestat_
{
  struct timespec start;
  double elapsed;

  struct
  {
    bool ready;
    size_t cyc;
    double sum;
    double avg;
    struct
    {
      size_t idx;
      double val;
    } max, min;
  } report;
} x_timestat;

int x_timestat_init(x_timestat* timestat);

int x_tic(x_timestat* timestat, const bool echo);

int x_toc(x_timestat* timestat, const char* unit, const bool echo);

int x_toc_ex(
    x_timestat* timestat,
    const char* unit, const long long cycle, const char* title,
    char* echo, size_t* size);
// x_timestat}}}

//************************************************************** Concurrency{{{
enum
{
  x_mtx_plain     = 0,
  x_mtx_recursive = 1,
  x_mtx_timed     = 2,
};

typedef unsigned int x_thr_rv;
typedef x_thr_rv (*x_thr_routine)(void*) ;

typedef struct _x_cnd_
{
#if X_IS_WINDOWS
  CONDITION_VARIABLE handle;
#else
  pthread_cond_t handle;
#endif
} x_cnd;

typedef struct _x_mtx_
{
#if X_IS_WINDOWS
  void* handle;
#else
  pthread_mutex_t handle;
#endif
  int type;
} x_mtx;

#if X_IS_WINDOWS
typedef HANDLE x_sem;
#else
typedef sem_t x_sem;
#endif

typedef struct _x_thr_
{
#if X_IS_WINDOWS
  HANDLE handle;
#else
  pthread_t handle;
#endif
  x_thr_rv (*fn)(void*);
  void* arg;
  bool exit;
} x_thr;

#if X_IS_WINDOWS
#define X_SEM_VALUE_MAX INT_MAX

#define X_CND_INIT {NULL}
#define X_MTX_INIT {NULL, x_mtx_plain}
#define X_SEM_INIT INVALID_HANDLE_VALUE
#define X_THR_INIT {NULL, NULL, NULL, false}
#else
#define X_SEM_VALUE_MAX SEM_VALUE_MAX

#define X_CND_INIT {PTHREAD_COND_INITIALIZER}
#define X_MTX_INIT {PTHREAD_MUTEX_INITIALIZER, x_mtx_plain}
#define X_SEM_INIT {0}
#define X_THR_INIT {0, NULL, NULL, false}
#endif

x_err x_cnd_broadcast(x_cnd* cnd);

void x_cnd_destroy(x_cnd* cnd);

x_err x_cnd_init(x_cnd* cnd);

x_err x_cnd_signal(x_cnd* cnd);

x_err x_cnd_timedwait(x_cnd* cnd, x_mtx* mtx, const struct timespec* time_point);

x_err x_cnd_wait(x_cnd* cnd, x_mtx* mtx);

void x_mtx_destroy(x_mtx* mtx);

x_err x_mtx_init(x_mtx* mtx, int type);

x_err x_mtx_lock(x_mtx* mtx);

x_err x_mtx_timedlock(x_mtx* mtx, const struct timespec* time_point);

x_err x_mtx_trylock(x_mtx* mtx);

x_err x_mtx_unlock(x_mtx* mtx);

x_err x_sem_close(x_sem* sem);

x_err x_sem_destroy(x_sem* sem);

x_err x_sem_getvalue(x_sem* sem, int* sval);

x_err x_sem_init(x_sem* sem, int pshared, unsigned int value);

x_err x_sem_open(x_sem* sem, const char* name, int oflag, ...);

x_err x_sem_post(x_sem* sem);

x_err x_sem_unlink(const char* name);

x_err x_sem_timedwait(x_sem* sem, const struct timespec* abs_timeout);

x_err x_sem_wait(x_sem* sem);

x_err x_sem_trywait(x_sem* sem);

#if X_IS_WINDOWS
HANDLE x_thr_current();
#else
pthread_t x_thr_current();
#endif

x_err x_thr_detach(x_thr* thr);

bool x_thr_equal(x_thr lhs, x_thr rhs);

void x_thr_exit(int exit_code);

x_err x_thr_init(x_thr* thr, x_thr_routine fn, void* arg);

x_err x_thr_join(x_thr* thr, int* exit_code);

x_err x_thr_yield();
// Concurrency}}}

#ifdef __cplusplus
}
#endif

#endif  // X_H


#ifdef X_H_IMPLEMENTATION
//******************************************************************* C and C++
#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************* Compat{{{
#if !X_IS_WINDOWS
int _kbhit()
{
  static bool initialized = false;
  if (!initialized)
  {
    struct termios settings = {0};
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
// Compat}}}

//******************************************************************* Gadget{{{
size_t x_cpu_count()
{
#if X_IS_WINDOWS
  SYSTEM_INFO info = {0};
  GetSystemInfo(&info);
  return (size_t)info.dwNumberOfProcessors;
#else
  return (size_t)sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

double x_duration(
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

long long x_file_size(const char* file)
{
  int err = 0;

#if X_IS_WINDOWS
  struct _stat64 s = {0};
  err = _stat64(file, &s);
#else
  struct stat s = {0};
  err = stat(file, &s);
#endif

  if (err != 0)
  {
    return -1;
  }

  return s.st_size;
}

const char* x_full_path(char* dst, const char* src)
{
  if (dst == NULL)
  {
    return NULL;
  }

#if X_IS_WINDOWS
  return _fullpath(dst, src, X_PATH_MAX);
#else
  return realpath(src, dst);
#endif
}

int x_getch()
{
  int key = 0;

#if X_IS_WINDOWS
  if (_kbhit())
  {
    key = toupper(_getch());
  }
  return key;
#else
  int bytes_waiting = _kbhit();
  if (bytes_waiting <= 0)
  {
    return 0;
  }

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
  else if (bytes_read >= 2)
  {
    if (buffer.ch[0] == 0x1B && buffer.ch[1] == 0x5B)
    {
      if (bytes_read == 2)
      {
        key = X_KEY_ESC;
      }
      else
      {
        switch (buffer.ch[2])
        {
          case X_KEY_A:
            key = X_KEY_UP;
            break;
          case X_KEY_B:
            key = X_KEY_DOWN;
            break;
          case X_KEY_C:
            key = X_KEY_RIGHT;
            break;
          case X_KEY_D:
            key = X_KEY_LEFT;
            break;
        }
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

struct timespec x_now()
{
  struct timespec ts = {0};

#if X_IS_WINDOWS || __STDC_VERSION__ >= 201112L
  timespec_get(&ts, TIME_UTC);
#else
  clock_gettime(CLOCK_MONOTONIC, &ts);
#endif

  return ts;
}

bool x_path_exists(const char* path)
{
  int err = -1;

#if X_IS_WINDOWS
  struct _stat64 s = {0};
  err = _stat64(path, &s);
#else
  struct stat s = {0};
  err = stat(path, &s);
#endif

  return (err == 0);
}

int x_splitpath(
    const char *path,
    char *root, const size_t rsz, char *dir, const size_t dsz,
    char *file, const size_t fsz, char *ext, const size_t esz)
{
  char full[X_PATH_MAX] = {0};
  x_full_path(full, path);

  if (!x_path_exists(full))
  {
    return ENOENT;
  }

#if X_IS_WINDOWS
  return _splitpath_s(full, root, rsz, dir, dsz, file, fsz, ext, esz);
#else
  if ((root == NULL && rsz != 0) || (root != NULL && rsz == 0)
      || (dir == NULL && dsz != 0) || (dir != NULL && dsz == 0)
      || (file == NULL && fsz != 0) || (file != NULL && fsz == 0)
      || (ext == NULL && esz != 0) || (ext != NULL && esz == 0))
  {
    return EINVAL;
  }

  if (root != NULL) { root[0] = '\0'; }
  if (dir != NULL) { dir[0] = '\0'; }
  if (file != NULL) { file[0] = '\0'; }
  if (ext != NULL) { ext[0] = '\0'; }

  size_t psz = strlen(full);
  size_t sz = 0;
  char *start = NULL;
  char *end = NULL;

  // root
  start = strchr((char*)path, '/');
  if (start == NULL)
  {
    return ENOENT;
  }

  end = strchr(start + 1, '/');
  if (end == NULL)
  {
    end = full + psz;
  }

  if (root != NULL)
  {
    sz = end - start;
    if (sz >= rsz)
    {
      return ENOBUFS;
    }

    memcpy(root, start, sz);
    root[sz] = '\0';

    if (end == NULL)
    {
      return 0;
    }
  }

  // dir
  start = strchr(end, '/');
  if (start == NULL)
  {
    return 0;
  }

  end = strrchr((char*)path, '/');
  if (end <= start)
  {
    return 0;
  };
  if (end == NULL)
  {
    end = full + psz;
  }

  if (dir != NULL)
  {
    sz = end - start;
    if (sz >= dsz)
    {
      return ENOBUFS;
    }

    memcpy(dir, start, sz);
    dir[sz] = '\0';

    if (end == NULL)
    {
      return 0;
    }
  }

  // file
  start = end + 1;
  if ((start - full) >= 0)
  {
    return 0;
  }

  end = strrchr((char*)path, '.');
  if (end <= start)
  {
    return 0;
  }
  if (end == NULL)
  {
    end = full + psz;
  }

  if (file != NULL)
  {
    sz = end - start;
    if (sz >= fsz)
    {
      return ENOBUFS;
    }

    memcpy(file, start, sz);
    file[sz] = '\0';
  }

  // ext
  if (ext != NULL)
  {
    start = end;
    end = full + psz;

    if (end <= start)
    {
      return 0;
    }

    sz = end - start;
    memcpy(ext, start, sz);
    ext[sz] = '\0';
  }

  return 0;
#endif
}

void x_sleep(const unsigned long ms)
{
#if X_IS_WINDOWS
  Sleep(ms);
#else
  struct timespec duration;
  duration.tv_sec = 0;
  duration.tv_nsec = (long)(ms * 1000000UL);
  nanosleep(&duration, NULL);
#endif
}

bool x_string_empty(const char* string)
{
  return (string == NULL || string[0] == '\0');
}

const char* x_timestamp(char* buffer, const size_t size)
{
  if (buffer == NULL)
  {
    return "";
  }

  time_t now = time(NULL);

#if X_IS_WINDOWS
  int err = ctime_s(buffer, size, &now);
  if (err != 0)
  {
    return "";
  }
#else
  ctime_r(&now, buffer);
#endif

  buffer[strlen(buffer) - 1] = '\0';

  return buffer;
}
// Gadget}}}

//******************************************************************** x_err{{{
bool x_succ(const long long err)
{
  return (err == 0);
}

bool x_fail(const long long err)
{
  return (err != 0);
}

void _x_err_msg(x_err *err, ...)
{
  if (err == NULL)
  {
    return;
  }

#if X_IS_WINDOWS
  if (err->cat & x_err_win32)
  {
    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM
        | FORMAT_MESSAGE_IGNORE_INSERTS
        | FORMAT_MESSAGE_MAX_WIDTH_MASK,
        NULL, (DWORD)err->val, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)err->msg, sizeof(err->msg), NULL);
  }
  else if (err->cat == x_err_custom)
  {
    va_list args;
    va_start(args, err);
    const char* msg = va_arg(args, const char*);
    strcpy_s(err->msg, sizeof(err->msg), msg);
    va_end(args);
  }
  else  // x_err_posix
  {
    strerror_s(err->msg, sizeof(err->msg), (int)err->val);
  }
#else
  if (err->cat == x_err_custom)
  {
    va_list args;
    va_start(args, err);
    const char* msg = va_arg(args, const char*);
    strcpy(err->msg, msg);
    va_end(args);
  }
  else  // x_err_posix, x_err_win32 has no effect
  {
    strcpy(err->msg, strerror((int)err->val));
  }
#endif
}

x_err x_ok()
{
  static x_err err = {x_err_system, 0, "success"};
  return err;
}

x_err x_get_err(const int cat)
{
  x_err err = {x_err_system, 0, {0}};

#if X_IS_WINDOWS
  switch (cat)
  {
    case x_err_win32:
      err.cat = cat;
      err.val = (long long)GetLastError();
      break;
    case x_err_winsa:
      err.cat = cat;
      err.val = (long long)WSAGetLastError();
      break;
    default:
      err.cat = x_err_posix;
      err.val = (long long)errno;
      break;
  }
#endif

  _x_err_msg(&err);

  return err;
}

x_err x_set_err(const int cat, const long long val, ...)
{
  x_err err = {x_err_system, 0, {0}};

  err.cat = cat;
  err.val = val;

  va_list args;
  va_start(args, val);
  _x_err_msg(&err, args);
  va_end(args);

  return err;
}
// x_err}}}

//******************************************************************** x_log{{{
#define _X_COLOR_BLACK   "\033[90m"
#define _X_COLOR_RED     "\033[91m"
#define _X_COLOR_GREEN   "\033[92m"
#define _X_COLOR_YELLOW  "\033[93m"
#define _X_COLOR_BLUE    "\033[94m"
#define _X_COLOR_MAGENTA "\033[95m"
#define _X_COLOR_CYAN    "\033[96m"
#define _X_COLOR_WHITE   "\033[97m"
#define _X_COLOR_RESET   "\033[0m"

#define _X_LOG_COLOR_P _X_COLOR_WHITE
#define _X_LOG_COLOR_F _X_COLOR_MAGENTA
#define _X_LOG_COLOR_E _X_COLOR_RED
#define _X_LOG_COLOR_W _X_COLOR_YELLOW
#define _X_LOG_COLOR_I _X_COLOR_GREEN
#define _X_LOG_COLOR_D _X_COLOR_CYAN

void _x_log_prefix(
    char* buffer, const size_t size, const int level, const char* filename,
    const char* function, const long line)
{
  static char lvl[] = {'P', 'F', 'E', 'W', 'I', 'D'};
  static char timestamp[26] = {0};

  x_timestamp(timestamp, 26);

#ifdef NDEBUG
  (void)snprintf(buffer, size, "[%c %s] ", lvl[level], timestamp);
#else
  (void)snprintf(
      buffer, size, "[%c %s | %s - %s - %ld] ",
      lvl[level], timestamp, filename, function, line);
#endif
}

int _x_log_level(const char level)
{
  switch (tolower(level)) {
    case 'p':
      return X_LOG_PLAIN;
    case 'f':
      return X_LOG_FATAL;
    case 'e':
      return X_LOG_ERROR;
    case 'w':
      return X_LOG_WARNING;
    case 'i':
      return X_LOG_INFO;
    case 'd':
      return X_LOG_DEBUG;
    default:
      return X_LOG_NONE;
  }
}

void _x_log_impl(
    const char* filename, const char* function, const long line,
    const char level, FILE* file, const char* format, ...)
{
  static char color_level[8] = {0};
  static char color_reset[8] = {0};

  int lvl = _x_log_level(level);

  if (lvl == X_LOG_PLAIN)
  {
#if X_LOG_LEVEL >= X_LOG_PLAIN
    snprintf(color_level, 8, _X_LOG_COLOR_P);
#else
    return;
#endif
  }
  else if (lvl == X_LOG_FATAL)
  {
#if X_LOG_LEVEL >= X_LOG_FATAL
    snprintf(color_level, 8, _X_LOG_COLOR_F);
#else
    return;
#endif
  }
  else if (lvl == X_LOG_ERROR)
  {
#if X_LOG_LEVEL >= X_LOG_ERROR
    snprintf(color_level, 8, _X_LOG_COLOR_E);
#else
    return;
#endif
  }
  else if (lvl == X_LOG_WARNING)
  {
#if X_LOG_LEVEL >= X_LOG_WARNING
    snprintf(color_level, 8, _X_LOG_COLOR_W);
#else
    return;
#endif
  }
  else if (lvl == X_LOG_INFO)
  {
#if X_LOG_LEVEL >= X_LOG_INFO
    snprintf(color_level, 8, _X_LOG_COLOR_I);
#else
    return;
#endif
  }
  else if (lvl == X_LOG_DEBUG)
  {
#if X_LOG_LEVEL >= X_LOG_DEBUG
    snprintf(color_level, 8, _X_LOG_COLOR_D);
#else
    return;
#endif
  }
  else
  {
    return;
  }

  snprintf(color_reset, 8, _X_COLOR_RESET);

  static char prefix[X_LOG_PREFIX_LIMIT] = {0};
  _x_log_prefix(prefix, X_LOG_PREFIX_LIMIT, lvl, filename, function, line);

  static char msg[X_LOG_MSG_LIMIT] = {0};
  va_list args;
  va_start(args, format);
  (void)vsnprintf(msg, X_LOG_MSG_LIMIT, format, args);
  va_end(args);

  if (file != NULL)
  {
    fprintf(file, "%s%s\n", prefix, msg);
  }

  printf("%s%s%s%s\n", color_level, prefix, msg, color_reset);
}
// x_log}}}

//*************************************************************** x_timestat{{{
int x_timestat_init(x_timestat* timestat)
{
  if (timestat == NULL)
  {
    return EINVAL;
  }

  timestat->start = x_now();
  timestat->elapsed = 0.0;
  timestat->report.ready = false;
  timestat->report.cyc = 0;
  timestat->report.sum = 0.0;
  timestat->report.avg = 0.0;
  timestat->report.max.idx = 0;
  timestat->report.max.val = DBL_MIN;
  timestat->report.min.idx = 0;
  timestat->report.min.val = DBL_MAX;

  return 0;
}

int x_tic(x_timestat* timestat, const bool echo)
{
  if (timestat == NULL)
  {
    return EINVAL;
  }

  timestat->start = x_now();

  if (echo)
  {
    char ts[26] = {0};
    printf("Timing starts at: %s.\n", x_timestamp(ts, 26));
  }

  return 0;
}

int x_toc(x_timestat* timestat, const char* unit, const bool echo)
{
  if (timestat == NULL)
  {
    return EINVAL;
  }

  if (x_string_empty(unit))
  {
    unit = "ms";
  }

  timestat->elapsed = x_duration(timestat->start, x_now(), unit);

  if (echo)
  {
    char ts[26] = {0};
    printf("Timing stops at:  %s.\n", x_timestamp(ts, 26));
    printf("Time elapsed: %f%s.\n", timestat->elapsed, unit);
  }

  return 0;
}

int x_toc_ex(
    x_timestat* timestat,
    const char* unit, const long long cycle, const char* title,
    char* echo, size_t* size)
{
  if (timestat == NULL || cycle <= 0LL)
  {
    return EINVAL;
  }

  if (x_string_empty(unit))
  {
    unit = "ms";
  }

  int err = x_toc(timestat, unit, false);
  if (err < 0)
  {
    return err;
  }

  if (timestat->elapsed > timestat->report.max.val)
  {
    timestat->report.max.idx = timestat->report.cyc;
    timestat->report.max.val = timestat->elapsed;
  }
  if (timestat->elapsed < timestat->report.min.val)
  {
    timestat->report.min.idx = timestat->report.cyc;
    timestat->report.min.val = timestat->elapsed;
  }

  timestat->report.sum += timestat->elapsed;
  timestat->report.cyc += 1LL;
  timestat->report.avg = timestat->report.sum / timestat->report.cyc;

  if (timestat->report.cyc % cycle == 0LL)
  {
    timestat->report.ready = true;

    if (echo != NULL && size != NULL && *size > 0)
    {
      const char* ttl = x_string_empty(title) ? "REPORT" : title;

      size_t bytes = (size_t)snprintf(echo, *size,
          "[%s] %f%s in %zu cycles - avg: %f%s, min(%zu): %f%s, max(%zu): %f%s",
          ttl,
          timestat->report.sum, unit,
          timestat->report.cyc,
          timestat->report.avg, unit,
          timestat->report.min.idx, timestat->report.min.val, unit,
          timestat->report.max.idx, timestat->report.max.val, unit);

      if (bytes >= *size)
      {
        *size = bytes + 1;
        return ERANGE;
      }
    }
  }

  return 0;
}
// x_timestat}}}

//************************************************************** Concurrency{{{
x_err x_cnd_broadcast(x_cnd* cnd)
{
  if (cnd == NULL)
  {
    return x_set_err(x_err_posix, EINVAL);
  }

#if X_IS_WINDOWS
  WakeAllConditionVariable(&cnd->handle);
  return x_ok();
#else
  return x_set_err(x_err_posix, (long long)pthread_cond_broadcast(&cnd->handle));
#endif
}

void x_cnd_destroy(x_cnd* cnd)
{
  if (cnd == NULL)
  {
    return;
  }

#if X_IS_WINDOWS
  (void)cnd;
#else
  pthread_cond_destroy(&cnd->handle);
#endif
}

x_err x_cnd_init(x_cnd* cnd)
{
  if (cnd == NULL)
  {
    return x_set_err(x_err_posix, EINVAL);
  }

#if X_IS_WINDOWS
  InitializeConditionVariable(&cnd->handle);
  return x_ok();
#else
  return x_set_err(x_err_posix, (long long)pthread_cond_init(&cnd->handle, NULL));
#endif
}

x_err x_cnd_signal(x_cnd* cnd)
{
  if (cnd == NULL)
  {
    return x_set_err(x_err_posix, EINVAL);
  }

#if X_IS_WINDOWS
  WakeConditionVariable(&cnd->handle);
  return x_ok();
#else
  return x_set_err(x_err_posix, (long long)pthread_cond_signal(&cnd->handle));
#endif
}

x_err x_cnd_timedwait(x_cnd* cnd, x_mtx* mtx, const struct timespec* time_point)
{
  if (cnd == NULL || mtx == NULL || time_point == NULL)
  {
    return x_set_err(x_err_posix, EINVAL);
  }

#if X_IS_WINDOWS
  DWORD d = (DWORD)x_duration(x_now(), *time_point, "ms");
  if (mtx->type & x_mtx_recursive)
  {
    return (
        SleepConditionVariableCS(&cnd->handle, (CRITICAL_SECTION*)mtx->handle, d)
        ? x_ok() : x_get_err(x_err_win32));
  }
  else
  {
    return (
        SleepConditionVariableSRW(&cnd->handle, (SRWLOCK*)mtx->handle, d, 0UL)
        ? x_ok() : x_get_err(x_err_win32));
  }
#else
  return x_set_err(
      x_err_posix,
      (long long)pthread_cond_timedwait(&cnd->handle, &mtx->handle, time_point));
#endif
}

x_err x_cnd_wait(x_cnd* cnd, x_mtx* mtx)
{
  if (cnd == NULL || mtx == NULL)
  {
    return x_set_err(x_err_posix, EINVAL);
  }

#if X_IS_WINDOWS
  if (mtx->type & x_mtx_recursive)
  {
    SleepConditionVariableCS(
        &cnd->handle, (CRITICAL_SECTION*)mtx->handle, INFINITE);
  }
  else
  {
    SleepConditionVariableSRW(
        &cnd->handle, (SRWLOCK*)mtx->handle, INFINITE, 0UL);
  }

  return x_ok();
#else
  return x_set_err(
      x_err_posix, (long long)pthread_cond_wait(&cnd->handle, &mtx->handle));
#endif
}

void x_mtx_destroy(x_mtx* mtx)
{
  if (mtx == NULL)
  {
    return;
  }

#if X_IS_WINDOWS
  if (mtx->type & x_mtx_recursive)
  {
    CRITICAL_SECTION* m = (CRITICAL_SECTION*)mtx->handle;
    DeleteCriticalSection(m);
    free(m);
  }
  else
  {
    SRWLOCK* m = (SRWLOCK*)mtx->handle;
    free(m);
  }
#else
  pthread_mutex_destroy(&mtx->handle);
#endif
}

x_err x_mtx_init(x_mtx* mtx, int type)
{
  if (mtx == NULL)
  {
    return x_set_err(x_err_posix, EINVAL);
  }

#if X_IS_WINDOWS
  if (type & x_mtx_recursive)
  {
    mtx->handle = malloc(sizeof(CRITICAL_SECTION));
    if (mtx->handle == NULL)
    {
      return x_set_err(x_err_posix, ENOMEM);
    }
    InitializeCriticalSection((CRITICAL_SECTION*)mtx->handle);
  }
  else
  {
    mtx->handle = malloc(sizeof(SRWLOCK));
    if (mtx->handle == NULL)
    {
      return x_set_err(x_err_posix, ENOMEM);
    }
    InitializeSRWLock((SRWLOCK*)mtx->handle);
  }

  mtx->type = type;

  return x_ok();
#else
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);

  if (type & x_mtx_recursive)
  {
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  }
  else
  {
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
  }

  mtx->type = type;

  return x_set_err(x_err_posix, (long long)pthread_mutex_init(&mtx->handle, &attr));
#endif
}

x_err x_mtx_lock(x_mtx* mtx)
{
  if (mtx == NULL)
  {
    return x_set_err(x_err_posix, EINVAL);
  }

#if X_IS_WINDOWS
  if (mtx->type & x_mtx_recursive)
  {
    EnterCriticalSection((CRITICAL_SECTION*)mtx->handle);
  }
  else
  {
    AcquireSRWLockExclusive((SRWLOCK*)mtx->handle);
  }

  return x_ok();
#else
  return x_set_err(x_err_posix, (long long)pthread_mutex_lock(&mtx->handle));
#endif
}

x_err x_mtx_timedlock(x_mtx* mtx, const struct timespec* time_point)
{
  if (mtx == NULL || (mtx->type & x_mtx_timed) == 0 || time_point == NULL)
  {
    return x_set_err(x_err_posix, EINVAL);
  }

#if X_IS_WINDOWS
  DWORD ms = (DWORD)x_duration(x_now(), *time_point, "ms");
  return (WaitForSingleObject(mtx->handle, ms) == WAIT_OBJECT_0
      ? x_ok() : x_get_err(x_err_win32));
#else
  return x_set_err(
      x_err_posix, (long long)pthread_mutex_timedlock(&mtx->handle, time_point));
#endif
}

x_err x_mtx_trylock(x_mtx* mtx)
{
  if (mtx == NULL)
  {
    return x_set_err(x_err_posix, EINVAL);
  }

#if X_IS_WINDOWS
  if (mtx->type & x_mtx_recursive)
  {
    TryEnterCriticalSection((CRITICAL_SECTION*)mtx->handle);
  }
  else
  {
    TryAcquireSRWLockExclusive((SRWLOCK*)mtx->handle);
  }

  return x_ok();
#else
  return x_set_err(x_err_posix, (long long)pthread_mutex_trylock(&mtx->handle));
#endif
}

x_err x_mtx_unlock(x_mtx* mtx)
{
  if (mtx == NULL)
  {
    return x_set_err(x_err_posix, EINVAL);
  }

#if X_IS_WINDOWS
  if (mtx->type & x_mtx_recursive)
  {
    LeaveCriticalSection((CRITICAL_SECTION*)mtx->handle);
  }
  else
  {
    ReleaseSRWLockExclusive((SRWLOCK*)mtx->handle);
  }

  return x_ok();
#else
  return x_set_err(x_err_posix, (long long)pthread_mutex_unlock(&mtx->handle));
#endif
}

x_err x_sem_close(x_sem* sem)
{
  if (sem == NULL)
  {
    return x_set_err(x_err_posix, EINVAL);
  }

#if X_IS_WINDOWS
  return (CloseHandle(sem) ? x_ok() : x_get_err(x_err_win32));
#else
  return (sem_close(sem) == 0 ? x_ok() : x_get_err(x_err_posix));
#endif
}

x_err x_sem_destroy(x_sem* sem)
{
  if (sem == NULL)
  {
    return x_set_err(x_err_posix, EINVAL);
  }

#if X_IS_WINDOWS
  return (CloseHandle(sem) ? x_ok() : x_get_err(x_err_win32));
#else
  return (sem_destroy(sem) == 0 ? x_ok() : x_get_err(x_err_posix));
#endif
}

x_err x_sem_getvalue(x_sem* sem, int* sval)
{
  if (sem == NULL)
  {
    return x_set_err(x_err_posix, EINVAL);
  }

#if X_IS_WINDOWS
  if (WaitForSingleObject(sem, 0) != WAIT_OBJECT_0)
  {
    return x_get_err(x_err_win32);
  }

  long value = 0;
  if (!ReleaseSemaphore(sem, 1, &value))
  {
    return x_get_err(x_err_win32);
  }

  *sval = (int)value;

  return x_ok();
#else
  return (sem_getvalue(sem, sval) == 0 ? x_ok() : x_get_err(x_err_posix));
#endif
}

x_err x_sem_init(x_sem* sem, int pshared, unsigned int value)
{
  if (sem == NULL || value > (unsigned int)X_SEM_VALUE_MAX)
  {
    return x_set_err(x_err_posix, EINVAL);
  }

#if X_IS_WINDOWS
  HANDLE s = CreateSemaphore(NULL, value, X_SEM_VALUE_MAX, NULL);
  if (s == NULL)
  {
    return x_get_err(x_err_win32);
  }

  *sem = s;

  return x_ok();
#else
  return (sem_init(sem, pshared, value) == 0 ? x_ok() : x_get_err(x_err_posix));
#endif
}

x_err x_sem_open(x_sem* sem, const char* name, int oflag, ...)
{
  if (sem == NULL)
  {
    return x_set_err(x_err_posix, EINVAL);
  }

#if X_IS_WINDOWS
  HANDLE s = INVALID_HANDLE_VALUE;

  if (oflag & _O_CREAT)
  {
    x_sem sem = X_SEM_INIT;

    va_list args;
    va_start(args, oflag);
    int mode = va_arg(args, int);
    unsigned int value = va_arg(args, unsigned int);
    s = CreateSemaphore(NULL, value, X_SEM_VALUE_MAX, name);
    va_end(args);
  }
  else
  {
    s = OpenSemaphore(STANDARD_RIGHTS_ALL, TRUE, name);
  }

  if (s == NULL)
  {
    return x_get_err(x_err_win32);
  }

  *sem = s;
#else
  sem_t* s = SEM_FAILED;

  if (oflag & O_CREAT)
  {
    va_list args;
    va_start(args, oflag);
    s = sem_open(name, oflag, args);
    va_end(args);
  }
  else
  {
    s = sem_open(name, oflag);
  }

  if (s == SEM_FAILED)
  {
    return x_get_err(x_err_posix);
  }

  sem = s;
#endif

  return x_ok();
}

x_err x_sem_post(x_sem* sem)
{
  if (sem == NULL)
  {
    return x_set_err(x_err_posix, EINVAL);
  }

#if X_IS_WINDOWS
  return (ReleaseSemaphore(sem, 1, NULL) ? x_ok() : x_get_err(x_err_win32));
#else
  return (sem_post(sem) == 0 ? x_ok() : x_get_err(x_err_posix));
#endif
}

x_err x_sem_unlink(const char* name)
{
#if X_IS_WINDOWS
  HANDLE s = OpenSemaphore(STANDARD_RIGHTS_ALL, TRUE, name);
  if (s == NULL)
  {
    return x_get_err(x_err_win32);
  }

  return (CloseHandle(s) ? x_ok() : x_get_err(x_err_win32));
#else
  return (sem_unlink(name) == 0 ? x_ok() : x_get_err(x_err_posix));
#endif
}

x_err x_sem_timedwait(x_sem* sem, const struct timespec* abs_timeout)
{
  if (sem == NULL)
  {
    return x_set_err(x_err_posix, EINVAL);
  }

#if X_IS_WINDOWS
  DWORD ms = (DWORD)x_duration(x_now(), *abs_timeout, "ms");
  return (WaitForSingleObject(sem, ms) == WAIT_OBJECT_0
      ? x_ok() : x_get_err(x_err_win32));
#else
  return (sem_timedwait(sem, abs_timeout) == 0 ? x_ok() : x_get_err(x_err_posix));
#endif
}

x_err x_sem_trywait(x_sem* sem)
{
  if (sem == NULL)
  {
    return x_set_err(x_err_posix, EINVAL);
  }

#if X_IS_WINDOWS
  return (WaitForSingleObject(sem, 0) == WAIT_OBJECT_0
      ? x_ok() : x_get_err(x_err_win32));
#else
  return (sem_trywait(sem) == 0 ? x_ok() : x_get_err(x_err_posix));
#endif
}

x_err x_sem_wait(x_sem* sem)
{
  if (sem == NULL)
  {
    return x_set_err(x_err_posix, EINVAL);
  }

#if X_IS_WINDOWS
  return (WaitForSingleObject(sem, INFINITE) == WAIT_OBJECT_0
      ? x_ok() : x_get_err(x_err_win32));
#else
  return (sem_wait(sem) == 0 ? x_ok() : x_get_err(x_err_posix));
#endif
}

#if X_IS_WINDOWS
HANDLE x_thr_current()
{
  return GetCurrentThread();
}
#else
pthread_t x_thrd_current()
{
  return pthread_self();
}
#endif

x_err x_thr_detach(x_thr* thr)
{
  if (thr == NULL)
  {
    return x_set_err(x_err_posix, EINVAL);
  }

  thr->exit = true;

#if X_IS_WINDOWS
  return (CloseHandle(thr->handle) ? x_ok() : x_get_err(x_err_win32));
#else
  return (pthread_detach(thr->handle) == 0 ? x_ok() : x_get_err(x_err_posix));
#endif
}

bool x_thr_equal(x_thr lhs, x_thr rhs)
{
  return lhs.handle == rhs.handle;
}

void x_thr_exit(int exit_code)
{
#if X_IS_WINDOWS
  _endthreadex((unsigned int)exit_code);
#else
  pthread_exit((void*)(unsigned long)exit_code);
#endif
}

x_err x_thr_init(x_thr* thr, x_thr_routine fn, void* arg)
{
  if (thr == NULL)
  {
    return x_set_err(x_err_posix, EINVAL);
  }

#if X_IS_WINDOWS
  thr->handle = (HANDLE)_beginthreadex(NULL, 0, fn, arg, 0, NULL);
  if (thr->handle == 0)
  {
    return x_get_err(x_err_posix);
  }
#else
#if __STDC_VERSION__ >= 201112L
  int err = pthread_create(&thr->handle, ATTR_C11_THREAD, fn, arg);
#else
  int err = pthread_create(&thr->handle, NULL, (void*(*)(void*))fn, arg);
#endif
  if (err != 0)
  {
    return x_set_err(x_err_posix, (long long)err);
  }
#endif

  thr->fn = fn;
  thr->arg = arg;
  thr->exit = false;

  return x_ok();
}

x_err x_thr_join(x_thr* thr, int* exit_code)
{
  if (thr == NULL)
  {
    return x_set_err(x_err_posix, EINVAL);
  }

#if X_IS_WINDOWS
  if (WaitForSingleObject(thr->handle, INFINITE) != WAIT_OBJECT_0)
  {
    return x_get_err(x_err_win32);
  }

  DWORD err = 0UL;
  if (GetExitCodeThread(thr->handle, &err) == 0)
  {
    return x_get_err(x_err_win32);
  }

  if (exit_code != NULL)
  {
    *exit_code = (int)err;
  }

  return x_thr_detach(thr);
#else
  void* rv = NULL;
  int err = pthread_join(thr->handle, &rv);

  if (exit_code != NULL)
  {
    *exit_code = err;
  }

  return x_set_err(x_err_posix, (long long)err);
#endif
}

x_err x_thr_yield()
{
#if X_IS_WINDOWS
  return (SwitchToThread() ? x_ok() : x_get_err(x_err_win32));
#else
  return x_set_err(x_err_posix, (long long)pthread_yield());
#endif
}
// Concurrency}}}

#ifdef __cplusplus
}
#endif

#endif  // X_H_IMPLEMENTATION
