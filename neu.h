/******************************************************************************
Copyright (c) 2022 Jihang Li
neu.h is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan
PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details.


Last update: 2022-06-13 10:20
******************************************************************************/
#ifndef NEU_H
#define NEU_H

#include <assert.h>
#include <float.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#if defined(_MSC_VER)
#include <conio.h>
#include <process.h>
#include <Windows.h>
#else
#include <linux/limits.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <threads.h>
#include <unistd.h>
#endif


//******************************************************************** C/C++{{{
#ifdef __cplusplus
extern "C" {
#endif
//****************************************************************** Special{{{
typedef CONDITION_VARIABLE cnd_t;
typedef CRITICAL_SECTION mtx_t;

static cnd_t _n_log_cnd;
static mtx_t _n_log_mtx;
static bool _n_log_done;
static bool _n_log_sync;

void n_cnd_destroy(cnd_t* cnd);
int n_cnd_init(cnd_t* cnd);
int n_cnd_notify_all(cnd_t* cnd);
int n_cnd_wait(cnd_t* cnd, mtx_t* mutex);

void n_mtx_destroy(mtx_t* mtx);
int n_mtx_init(mtx_t* mtx, int type);

void n_log_sync_destroy();

int n_log_sync_init();

errno_t n_log_to(const char* file, const char* format, ...);

bool n_string_empty(const char* string);

const char* n_timestamp(char* buffer, const size_t size);
//Special}}}

//******************************************************* N_EXPORT, N_IMPORT{{{
#ifndef N_EXPORT
#if defined(_MSC_VER)
#define N_EXPORT __declspec(dllexport)
#else
#define N_EXPORT __attribute__ ((visibility("default")))
#endif
#endif

#ifndef N_IMPORT
#if defined(_MSC_VER)
#define N_IMPORT __declspec(dllimport)
#else
#define N_IMPORT __attribute__ ((visibility("hidden")))
#endif
#endif
//N_EXPORT, N_IMPORT}}}

//******************************************************************** N_KEY{{{
#define N_KEY_ESC   (0x1B)
#define N_KEY_A     (0x41)
#define N_KEY_B     (0x42)
#define N_KEY_C     (0x43)
#define N_KEY_D     (0x44)
#define N_KEY_Q     (0x51)
#if defined(_MSC_VER)
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
//N_KEY}}}

//******************************************************************** n_log{{{
#ifndef N_LOG_BOLD
#define N_LOG_BOLD (true)
#endif

#ifdef NDEBUG
#ifndef N_LOG_LEVEL
#error N_LOG_LEVEL is not specified!
#endif
#else
#ifndef N_LOG_LEVEL
#define N_LOG_LEVEL (6)
#endif
#endif

#if N_LOG_BOLD
#define _N_COLOR_BLACK   "\033[1;30m"
#define _N_COLOR_RED     "\033[1;31m"
#define _N_COLOR_GREEN   "\033[1;32m"
#define _N_COLOR_YELLOW  "\033[1;33m"
#define _N_COLOR_BLUE    "\033[1;34m"
#define _N_COLOR_MAGENTA "\033[1;35m"
#define _N_COLOR_CYAN    "\033[1;36m"
#define _N_COLOR_WHITE   "\033[1;37m"
#else
#define _N_COLOR_BLACK   "\033[0;30m"
#define _N_COLOR_RED     "\033[0;31m"
#define _N_COLOR_GREEN   "\033[0;32m"
#define _N_COLOR_YELLOW  "\033[0;33m"
#define _N_COLOR_BLUE    "\033[0;34m"
#define _N_COLOR_MAGENTA "\033[0;35m"
#define _N_COLOR_CYAN    "\033[0;36m"
#define _N_COLOR_WHITE   "\033[0;37m"
#define _N_COLOR_RESET   "\033[0m"
#endif
#define _N_COLOR_RESET   "\033[0m"

#define _N_LOG_COLOR_P _N_COLOR_WHITE
#define _N_LOG_COLOR_F _N_COLOR_MAGENTA
#define _N_LOG_COLOR_E _N_COLOR_RED
#define _N_LOG_COLOR_W _N_COLOR_YELLOW
#define _N_LOG_COLOR_I _N_COLOR_GREEN
#define _N_LOG_COLOR_D _N_COLOR_BLUE

#define _N_LOG_COLOR(level) "_N_LOG_COLOR" ## level
#define _N_LOG_COLOR_EVAL(level)  _N_LOG_COLOR(level)

#if defined(_MSC_VER)
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#ifdef NDEBUG
#define _N_LOG_PREFIX(level, filename, function, line) do { \
  char ts[26] = {0}; \
  printf("[%s %s] ", level, n_timestamp(ts, 26)); \
} while (false)
#else
#define _N_LOG_PREFIX(level, filename, function, line) do { \
  char ts[26] = {0}; \
  printf("[%s %s | %s - %s - %ld] ", level, n_timestamp(ts, 26), filename, function, line); \
} while (false)
#endif

inline void _n_log_internal(
    const char* filename, const char* function, const long line,
    const char* level, const char* file, const char* format, ...)
{
  if (_n_log_sync)
  {
    while (!_n_log_done)
    {
      n_cnd_wait(&_n_log_cnd, &_n_log_mtx);
    }

    _n_log_done = false;
    n_cnd_notify_all(&_n_log_cnd);
  }

  if (strcmp(level, "P") == 0)
  {
#if N_LOG_LEVEL >= 1
    printf(_N_LOG_COLOR_P);
#else
    return;
#endif
  }
  else if (strcmp(level, "F") == 0)
  {
#if N_LOG_LEVEL >= 2
    printf(_N_LOG_COLOR_F);
#else
    return;
#endif
  }
  else if (strcmp(level, "E") == 0)
  {
#if N_LOG_LEVEL >= 3
    printf(_N_LOG_COLOR_E);
#else
    return;
#endif
  }
  else if (strcmp(level, "W") == 0)
  {
#if N_LOG_LEVEL >= 4
    printf(_N_LOG_COLOR_W);
#else
    return;
#endif
  }
  else if (strcmp(level, "I") == 0)
  {
#if N_LOG_LEVEL >= 5
    printf(_N_LOG_COLOR_I);
#else
    return;
#endif
  }
  else if (strcmp(level, "D") == 0)
  {
#if N_LOG_LEVEL >= 6
    printf(_N_LOG_COLOR_D);
#else
    return;
#endif
  }

  if (!n_string_empty(file))
  {
    size_t sz = 256;
    int bytes = 0;

    char* prefix_buf = (char*)malloc(sz);
    char ts[26] = {0};

    bytes = snprintf(
        prefix_buf, sz, "[%s %s | %s - %s - %ld] ",
        level, n_timestamp(ts, 26), filename, function, line);
    if (bytes + 1 > sz)
    {
      free(prefix_buf);
      prefix_buf = (char*)malloc(bytes + 1);
      snprintf(
          prefix_buf, bytes + 1, "[%s %s | %s - %s - %ld] ",
          level, n_timestamp(ts, 26), filename, function, line);
    }


    va_list msg;
    va_start(msg, format);

    char* msg_buf = (char*)malloc(sz);
    bytes = vsnprintf(msg_buf, sz, format, msg);
    if (bytes + 1 > sz)
    {
      free(msg_buf);
      msg_buf = (char*)malloc(bytes + 1);
      vsnprintf(msg_buf, bytes + 1, format, msg);
    }

    va_end(msg);
    n_log_to(file, "%s%s\n", prefix_buf, msg_buf);

    printf("%s%s%s\n", prefix_buf, msg_buf, _N_COLOR_RESET);

    free(prefix_buf);
    free(msg_buf);
  }
  else
  {
    _N_LOG_PREFIX(level, filename, function, line);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("%s\n", _N_COLOR_RESET);
  }

  if (_n_log_sync)
  {
    _n_log_done = true;
    n_cnd_notify_all(&_n_log_cnd);
  }

  if (strcmp(level, "F") == 0)
  {
#if N_LOG_LEVEL >= 2
    exit(EXIT_FAILURE);
#endif
  }
}

#define n_log(level, file, format, ...) do { \
  _n_log_internal(__FILENAME__, __FUNCTION__, __LINE__, level, file, format, ##__VA_ARGS__); \
} while (false)
//n_log}}}

#define n_array_size(a) ((sizeof(a) / sizeof(*(a))) / (size_t)(!(sizeof(a) % sizeof(*(a)))))

#ifdef NDEBUG
#define n_assert(expr) do { \
  if (!(expr)) { \
    char ts[26] = {0}; \
    fprintf(stderr, "\n[ASSERTION FAILURE %s | %s - %s - %d] \n%s", \
        n_timestamp(ts, 26), nfull_path(__FILENAME__), __FUNCTION__, __LINE__, #expr); \
    exit(EXIT_FAILURE); } \
} while (false)
#else
#define n_assert(expr) do {assert(expr);} while (false)
#endif

#define n_bit(bit) (1 << bit)

#define n_pi(T) (T)(3.141592653589793238462643383279502884197169399375)

//***************************************************************** Function{{{
inline bool n_succ(const errno_t err)
{
  return (err == 0);
}

inline bool n_fail(const errno_t err)
{
  return (err != 0);
}

inline double n_duration(
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

inline const char* n_full_path(const char* src, char* dst)
{
  if (dst == NULL)
  {
    return NULL;
  }

#if defined(_MSC_VER)
  return _fullpath(dst, src, _MAX_PATH);
#else
  return realpath(src, dst);
#endif
}

inline void n_log_sync_destroy()
{
  n_cnd_destroy(&_n_log_cnd);
  n_mtx_destroy(&_n_log_mtx);
  _n_log_done = true;
  _n_log_sync = false;
}

inline int n_log_sync_init()
{
  int err = 0;
  err = n_cnd_init(&_n_log_cnd);
  if (err != 0)
  {
    return err;
  }

  err = n_mtx_init(&_n_log_mtx, 0);
  if (err != 0)
  {
    return err;
  }

  _n_log_done = true;
  _n_log_sync = true;

  return 0;
}

inline errno_t n_log_to(const char* file, const char* format, ...)
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

inline bool n_path_exists(const char* path)
{
  int result = -1;

#if defined(_MSC_VER)
  struct _stat buffer;
  result = _stat(path, &buffer);
#else
  struct stat buffer;
  result = stat(path, &buffer);
#endif

  return (result == 0);
}

#if !defined(_MSC_VER)
inline int _kbhit()
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

inline int n_pressed_key()
{
  int key = 0;

#if defined(_MSC_VER)
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

inline void n_sleep(const unsigned long ms)
{
#if defined(_MSC_VER)
  Sleep(ms);
#else
  struct timespec duration;
  duration.tv_nsec = (long)(ms * 1000000L);
  nanosleep(&duration, NULL);
#endif
}

inline bool n_string_empty(const char* string)
{
  return (string == NULL || string[0] == '\0');
}

inline const char* n_timestamp(char* buffer, const size_t size)
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
//Function}}}

//************************************************************** Concurrency{{{
#if defined(_MSC_VER)
enum
{
  thrd_success  = 0,
  thrd_busy     = 1,
  thrd_error    = 2,
  thrd_nomen    = 3,
  thrd_timedout = 4,
};

enum
{
  mtx_plain     = 0,
  mtx_recursive = 1,
  mtx_timed     = 2,
};

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
//Concurrency}}}

//******************************************************************** n_cnd{{{
inline void n_cnd_destroy(cnd_t* cnd)
{
  if (cnd == NULL)
  {
    return;
  }

#if defined(_MSC_VER)
#else
  return cnd_destroy(cnd);
#endif

  cnd = NULL;
}

inline int n_cnd_init(cnd_t* cnd)
{
  if (cnd == NULL)
  {
    return thrd_error;
  }

#if defined(_MSC_VER)
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

inline int n_cnd_notify_all(cnd_t* cnd)
{
  if (cnd == NULL)
  {
    return thrd_error;
  }

#if defined(_MSC_VER)
  WakeAllConditionVariable(cnd);
  return thrd_success;
#else
  return cnd_broadcast(cnd);
#endif
}

inline int n_cnd_notify_one(cnd_t* cnd)
{
  if (cnd == NULL)
  {
    return thrd_error;
  }

#if defined(_MSC_VER)
  WakeConditionVariable(cnd);
  return thrd_success;
#else
  return cnd_signal(cnd);
#endif
}

inline int n_cnd_wait(cnd_t* cnd, mtx_t* mutex)
{
  if (cnd == NULL || mutex == NULL)
  {
    return thrd_error;
  }

#if defined(_MSC_VER)
  SleepConditionVariableCS(cnd, mutex, INFINITE);
  return thrd_success;
#else
  return cnd_wait(cnd, mutex);
#endif
}
//n_cnd_x}}}

//******************************************************************** n_mtx{{{
inline void n_mtx_destroy(mtx_t* mutex)
{
  if (mutex == NULL)
  {
    return;
  }

#if defined(_MSC_VER)
  DeleteCriticalSection(mutex);
#else
  mtx_destroy(mutex);
#endif

  mutex = NULL;
}

inline int n_mtx_init(mtx_t* mutex, int type)
{
  if (mutex == NULL)
  {
    return thrd_error;
  }

#if defined(_MSC_VER)
  InitializeCriticalSection(mutex);
  if (mutex == NULL)
  {
    return thrd_error;
  }
  return thrd_success;
#else
  return mtx_init(mutex, type);
#endif
}

inline int n_mtx_lock(mtx_t* mutex)
{
  if (mutex == NULL)
  {
    return thrd_error;
  }

#if defined(_MSC_VER)
  EnterCriticalSection(mutex);
  return thrd_success;
#else
  return mtx_lock(mutex);
#endif
}

inline int n_mtx_trylock(mtx_t* mutex)
{
  if (mutex == NULL)
  {
    return thrd_error;
  }

#if defined(_MSC_VER)
  TryEnterCriticalSection(mutex);
  return thrd_success;
#else
  return mtx_trylock(mutex);
#endif
}

inline int n_mtx_unlock(mtx_t* mutex)
{
  if (mutex == NULL)
  {
    return thrd_error;
  }

#if defined(_MSC_VER)
  LeaveCriticalSection(mutex);
  return thrd_success;
#else
  return mtx_unlock(mutex);
#endif
}
//n_mtx}}}

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

inline struct timespec n_now()
{
  struct timespec ts;
  timespec_get(&ts, TIME_UTC);
  return ts;
}

inline errno_t n_timer_init(struct n_timer* timer)
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

inline errno_t n_tic(const bool echo, struct n_timer* timer)
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

inline errno_t n_toc(const bool echo, const char* unit, struct n_timer* timer)
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

inline errno_t n_toc_profile(
    const bool echo, const long long cycle, const char* unit, const char* title,
    struct n_timer* timer)
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

  errno_t err = n_toc(false, unit, timer);
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
//n_timer}}}

#ifdef __cplusplus
}
#endif
//C/C++}}}

//********************************************************************** C++{{{
#ifdef __cplusplus
#include <condition_variable>
#include <thread>

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

//******************************************************************* n_thrd{{{
struct n_thread
{
  enum n_thrd_ctrl_code cc;
  std::thread thd;
  std::condition_variable cnd;
  std::mutex mtx;
};

inline std::thread::id n_thrd_current()
{
  return std::this_thread::get_id();
}

inline int n_thrd_detach(struct n_thread& thread)
{
  thread.thd.detach();
  return thrd_success;
}

inline bool n_thrd_equal(const struct n_thread& lhs, const struct n_thread& rhs)
{
  return lhs.thd.get_id() == rhs.thd.get_id();
}

inline void n_thrd_exit()
{
}

template<class Fn, class ...Args>
int n_thrd_init(struct n_thread* thread, Fn&& fn, Args&& ...args)
{
  if (thread == nullptr)
  {
    return thrd_error;
  }

  thread->cc = THRD_NONE;
  thread->thd = std::thread(fn, args...);

  return thrd_success;
}

inline int n_thrd_join(struct n_thread& thread)
{
  if (!thread.thd.joinable())
  {
    return thrd_error;
  }

  thread.thd.join();
  return thrd_success;
}

inline void n_thrd_wait(struct n_thread& thread, bool& ready)
{
  std::unique_lock<std::mutex> lock(thread.mtx);

  while (!ready)
  {
    thread.cnd.wait(lock);
  }
}

inline int n_thrd_yield()
{
  std::this_thread::yield();
  return thrd_success;
}
//n_thrd}}}
//C++}}}
//************************************************************************ C{{{
#else
#define n_release(x) do { \
  if (x != NULL) { \
    free(x); \
    x = NULL; \
  } \
} while (false)

#define n_release_array(x) do { \
  if (x != NULL) { \
    free(x); \
    x = NULL; \
  } \
} while (false)
//******************************************************************* n_thrd{{{
struct n_thread
{
  enum n_thrd_ctrl_code cc;
  thrd_t thd;
  cnd_t cnd;
  mtx_t mtx;
};

inline thrd_t n_thrd_current()
{
#if defined(_MSC_VER)
  return GetCurrentThread();
#else
  return thrd_current();
#endif
}

inline int n_thrd_detach(struct n_thread thread)
{
#if defined(_MSC_VER)
  BOOL succeeded = CloseHandle(thread.thd);
  if (!succeeded)
  {
    return thrd_error;
  }
#else
  int result = thrd_detach(thread.thd);
  if (result != thrd_success)
  {
    return result;
  }
#endif

  n_cnd_destroy(&thread.cnd);
  n_mtx_destroy(&thread.mtx);

  return thrd_success;
}

inline bool n_thrd_equal(struct n_thread lhs, struct n_thread rhs)
{
  return lhs.thd == rhs.thd;
}

inline void n_thrd_exit(int exit_code)
{
#if defined(_MSC_VER)
  _endthreadex((unsigned int)exit_code);
#else
  thrd_exit(exit_code);
#endif
}

inline int n_thrd_init(struct n_thread* thread, thrd_start_t fn, void* data)
{
  if (thread == NULL)
  {
    return thrd_error;
  }

  int result = thrd_success;

#if defined(_MSC_VER)
  thread->thd = (thrd_t)_beginthreadex(NULL, 0, fn, data, 0, NULL);
  if (!thread->thd)
  {
    return thrd_error;
  }
#else
  result = thrd_create(&thread->thd, fn, data);
  if (result != thrd_success)
  {
    return result;
  }
#endif

  thread->cc = THRD_NONE;

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

inline int n_thrd_join(struct n_thread thread, int* exit_code)
{
#if defined(_MSC_VER)
  if (WaitForSingleObject(thread.thd, INFINITE) != WAIT_OBJECT_0)
  {
    return thrd_error;
  }

  DWORD result = 0L;
  if (GetExitCodeThread(thread.thd, &result) == 0)
  {
    return thrd_error;
  }

  if (exit_code != NULL)
  {
    *exit_code = (int)result;
  }

  return n_thrd_detach(thread);
#else
  return thrd_join(thread, exit_code);
#endif
}

inline void n_thrd_wait(struct n_thread thread, bool ready)
{
  n_mtx_lock(&thread.mtx);
  
  while (!ready)
  {
    n_cnd_wait(&thread.cnd, &thread.mtx);
  }

  n_mtx_unlock(&thread.mtx);
}

inline int n_thrd_yield()
{
#if defined(_MSC_VER)
  return SwitchToThread() ? thrd_success : thrd_error;
#else
  thrd_yield();
  return thrd_success;
#endif
}
//n_thrd}}}
//C}}}
#endif

#endif  // NEU_H
