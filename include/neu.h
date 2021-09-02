/******************************************************************************
MIT License

Copyright (c) 2021 Jihang Li (Jihang DOT Li AT outlook DOT com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


Last update: 2021-09-02 16:09
******************************************************************************/
#ifndef NEU_H
#define NEU_H

#include <algorithm>
#include <cctype>
#include <cmath>
#include <condition_variable>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <map>
#include <mutex>
#include <queue>
#include <stdarg.h>
#include <system_error>
#include <thread>
#include <vector>
#include <sys/stat.h>

#if defined(_MSC_VER)
#include <conio.h>
#else
#include <linux/limits.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#endif


//****************************************************************** Special{{{
#if (defined(_MSVC_LANG) && _MSVC_LANG < 201402L) || (!defined(_MSVC_LANG) && __cplusplus < 201402L)
namespace std
{
template<class T, T... I>
struct integer_sequence
{
  typedef T value_type;
  static constexpr size_t size()
  {
    return sizeof...(I);
  };
};

template<size_t... I>
using index_sequence = integer_sequence<size_t, I...>;

template<typename T, size_t N, T... I>
struct make_integer_sequence :make_integer_sequence<T, N-1, N-1, I...> {};

template<typename T, T... I>
struct make_integer_sequence<T, 0, I...> :integer_sequence<T, I...> {};

template<std::size_t N>
using make_index_sequence = make_integer_sequence<std::size_t, N>;

template<typename... T>
using index_sequence_for = make_index_sequence<sizeof...(T)>;
}  // namespace std
#endif

template<int>
struct NVariadicPlaceholder {};

template<int N>
struct std::is_placeholder<NVariadicPlaceholder<N>> :std::integral_constant<int, N+1> {};
//****************************************************************** Special}}}

//******************************************************************** Macro{{{
#ifndef DLL_API
#if defined(_MSC_VER)
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __attribute__ ((visibility("default")))
#endif
#endif

#if defined(_MSC_VER)
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define NKEY_ESC   0x1B
#define NKEY_A     0x41
#define NKEY_B     0x42
#define NKEY_C     0x43
#define NKEY_D     0x44
#define NKEY_Q     0x51
#if defined(_MSC_VER)
#define NKEY_LEFT  0x4B
#define NKEY_UP    0x48
#define NKEY_RIGHT 0x4D
#define NKEY_DOWN  0x50
#else
#define NKEY_LEFT  (-1)
#define NKEY_UP    (-2)
#define NKEY_RIGHT (-3)
#define NKEY_DOWN  (-4)
#endif

#define NCOLOR_BLACK(bold)   "\033[" #bold ";30m"
#define NCOLOR_RED(bold)     "\033[" #bold ";31m"
#define NCOLOR_GREEN(bold)   "\033[" #bold ";32m"
#define NCOLOR_YELLOW(bold)  "\033[" #bold ";33m"
#define NCOLOR_BLUE(bold)    "\033[" #bold ";34m"
#define NCOLOR_MAGENTA(bold) "\033[" #bold ";35m"
#define NCOLOR_CYAN(bold)    "\033[" #bold ";36m"
#define NCOLOR_WHITE(bold)   "\033[" #bold ";37m"
#define NCOLOR_RESET         "\033[0m"

#define NLOG_COLOR_P NCOLOR_WHITE(1)
#define NLOG_COLOR_F NCOLOR_MAGENTA(1)
#define NLOG_COLOR_E NCOLOR_RED(1)
#define NLOG_COLOR_W NCOLOR_YELLOW(1)
#define NLOG_COLOR_I NCOLOR_BLUE(1)
#define NLOG_COLOR_D NCOLOR_GREEN(1)

#ifdef NDEBUG
#define NLOG_FORMAT(level, format) "[" level " %s %s] " format "\n" NCOLOR_RESET, __DATE__, __TIME__
#else
#define NLOG_FORMAT(level, format) "[" level " %s %s %s > %s > %d] " format "\n" NCOLOR_RESET, __DATE__, __TIME__, __FILENAME__, __FUNCTION__, __LINE__
#endif

#ifndef NLOG_LEVEL
#define NLOG_LEVEL 5
#endif

#if NLOG_LEVEL >= 0
#define NLogP(format, ...) printf(NLOG_COLOR_P NLOG_FORMAT("P", format), ##__VA_ARGS__)
#else
#define NLogP(format, ...)
#endif

#if NLOG_LEVEL >= 1
#define NLogF(format, ...) printf(NLOG_COLOR_F NLOG_FORMAT("F", format), ##__VA_ARGS__)
#else
#define NLogF(format, ...)
#endif

#if NLOG_LEVEL >= 2
#define NLogE(format, ...) printf(NLOG_COLOR_E NLOG_FORMAT("E", format), ##__VA_ARGS__)
#else
#define NLogE(format, ...)
#endif

#if NLOG_LEVEL >= 3
#define NLogW(format, ...) printf(NLOG_COLOR_W NLOG_FORMAT("W", format), ##__VA_ARGS__)
#else
#define NLogW(format, ...)
#endif

#if NLOG_LEVEL >= 4
#define NLogI(format, ...) printf(NLOG_COLOR_I NLOG_FORMAT("I", format), ##__VA_ARGS__)
#else
#define NLogI(format, ...)
#endif

#if NLOG_LEVEL >= 5
#define NLogD(format, ...) printf(NLOG_COLOR_D NLOG_FORMAT("D", format), ##__VA_ARGS__)
#else
#define NLogD(format, ...)
#endif
//******************************************************************** Macro}}}

//************************************************************* Declarations{{{
//Function{{{
/* The followings are in Marco section.
  NLogF(const char *format, ...);
  NLogE(const char *format, ...);
  NLogW(const char *format, ...);
  NLogI(const char *format, ...);
  NLogD(const char *format, ...);
*/

#if defined(_MSC_VER)
#else
int _kbhit();
#endif

bool NSucc(const std::error_code &error);

bool NFail(const std::error_code &error);

bool NPathExists(const char *path);

bool NStringEmpty(const char *string);

int NPressedKey();

long long NDuration(
    const std::chrono::system_clock::time_point &start,
    const std::chrono::system_clock::time_point &end,
    const std::string &unit = "ms");

void NSleep(const unsigned long long &ms);

std::string NFullPath(const char *path);

std::string NTimestamp(const char *format = "%Y-%m-%d %H:%M:%S");

template<class T, size_t N>
size_t NArraySize(const T (&array)[N]);

template<class T>
const T &NClamp(const T &x, const T &low, const T &high);

template<class T>
void NRelease(T &&pointer);

template<class T>
void NReleaseArray(T &&pointer);
//Function}}}

//Class{{{
class NCallable
{
public:
  NCallable() = delete;
  ~NCallable() = delete;

  template<class R, class ...Args>
  static auto Create(R (*function)(Args...));

  template<class T, class R, class ...Args>
  static auto Create(R (T::*function)(Args...), T *obj);

private:
  template<class R, class T, class... Args1, class... Args2>
  static auto Bind(R (T::*function)(Args1...), Args2&&... arguments);

  template<class R, class T, class... Args1, class... Args2, size_t... I>
  static auto InternalBind(
      std::index_sequence<I...>, R (T::*function)(Args1...), Args2&&... arguments);
};  // class NCallable

template<class T>
class NQueue
{
public:
  NQueue() = default;
  NQueue(const NQueue<T> &) = delete;
  NQueue& operator=(const NQueue<T> &) = delete;

  NQueue(NQueue<T> &&other);

  ~NQueue();

  const T &Back(const bool &wait = true);

  void Clear();

  bool Empty();

  const T &Front();

  void Pop();

  void Push(const T &item);

  size_t Size();

private:
  std::condition_variable m_cv;
  std::deque<T> m_queue;
  std::mutex m_mutex;
};  // class NQueue

class NThread
{
public:
  template<class Fn, class ...Args>
  NThread(Fn&& function, Args&&... arguments);

  ~NThread();

  std::error_code Start();

  void Notify();

  void Stop();

  void Wait(bool should_wait);

private:
  std::condition_variable m_cv;
  std::unique_lock<std::mutex> m_lock;
  std::mutex m_mutex;
  std::thread m_thread;
};  // class NThread

class NTimer
{
public:
  NTimer();

  ~NTimer();

  void Tic(const bool &echo = false);

  long long Toc(const char *unit = "ms", const bool &echo = false);

private:
  std::chrono::system_clock::time_point m_tic;
  long long m_elapsed;
};  // class NTimer
//Class}}}
//************************************************************* Declarations}}}

//***************************************************************** Function{{{
#if defined(_MSC_VER)
#else
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

inline bool NSucc(const std::error_code &error)
{
  return !error ? true : false;
}

inline bool NFail(const std::error_code &error)
{
  return error ? true : false;
}

inline bool NPathExists(const char *path)
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

inline bool NStringEmpty(const char *string)
{
  return (string == nullptr || string[0] == '\0');
}

inline int NPressedKey()
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
  } buffer{0};
  int err = 0;
  ssize_t bytes_read = 0;

  err = tcgetattr(0, &old_settings);
  if (err) { return 0; }

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

inline long long NDuration(
    const std::chrono::system_clock::time_point &start,
    const std::chrono::system_clock::time_point &end,
    const std::string &unit)
{
  if (unit.compare("s") == 0)
  {
    return std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
  }
  else if (unit.compare("us") == 0)
  {
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  }
  else //  if (unit.compare("ms") == 0)
  {
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  }
}

inline void NSleep(const unsigned long long &ms)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

inline std::string NFullPath(const char *file)
{
#if defined(_MSC_VER)
  char abs_path[_MAX_PATH];
  if (!_fullpath(abs_path, file, _MAX_PATH))
  {
    return nullptr;
  }
#else
  char abs_path[PATH_MAX];
  if (!realpath(file, abs_path))
  {
    return nullptr;
  }
#endif

  return std::string(abs_path);
}

inline std::string NTimestamp(const char *format)
{
  time_t rawtime(0);
  struct tm timeinfo;

  time(&rawtime);
#if defined(_MSC_VER)
  localtime_s(&timeinfo, &rawtime);
#else
  localtime_r(&rawtime, &timeinfo);
#endif

  char buffer[50];

  if (NStringEmpty(format))
  {
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
  }
  else
  {
    strftime(buffer, sizeof(buffer), format, &timeinfo);
  }
  return std::string(buffer);
}

template<class T, size_t N>
size_t NArraySize(const T (&array)[N])
{
  return N;
}

template<class T>
const T &NClamp(const T &x, const T &low, const T &high)
{
#if (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || (__cplusplus >= 201703L)
  return std::clamp(x, low, high);
#else
  return x < low ? low : (x > high ? high : x);
#endif
}

template<class T>
void NRelease(T &&pointer)
{
  if (pointer != nullptr)
  {
    delete pointer;
    pointer = nullptr;
  }
}

template<class T>
void NReleaseArray(T &&pointer)
{
  if (pointer != nullptr)
  {
    delete[] pointer;
    pointer = nullptr;
  }
}
//***************************************************************** Function}}}

//******************************************************************** Class{{{
//NCallable{{{
template<class R, class ...Args>
auto NCallable::Create(R (*function)(Args...))
{
  return std::function<R(Args...)>(function);
}

template<class T, class R, class ...Args>
auto NCallable::Create(R (T::*function)(Args...), T *obj)
{
  return NCallable::Bind(function, obj);
}

template<class R, class T, class... Args1, class... Args2, size_t... I>
auto NCallable::InternalBind(
    std::index_sequence<I...>, R (T::*function)(Args1...), Args2&&... arguments)
{
  return std::bind(
      function, std::forward<Args2>(arguments)..., NVariadicPlaceholder<I>{}...);
}

template<class R, class T, class... Args1, class... Args2>
auto NCallable::Bind(R (T::*function)(Args1...), Args2&&... arguments)
{
  return NCallable::InternalBind(
      std::make_index_sequence<sizeof...(Args1) - sizeof...(Args2) + 1>{},
      function, std::forward<Args2>(arguments)...);
}
//NCallable}}}

//NQueue{{{
template<class T>
NQueue<T>::NQueue(NQueue<T> &&other)
{
  this->m_queue.clear();
  this->m_queue = std::move(other.m_queue);
  other.m_queue.clear();
}

template<class T>
NQueue<T>::~NQueue()
{
  this->m_queue.clear();
}

template<class T>
const T &NQueue<T>::Back(const bool &wait)
{
  std::unique_lock<std::mutex> lock(this->m_mutex);

  if (wait)
  {
    while (this->m_queue.empty())
    {
      this->m_cv.wait(lock);
    }
  }
  else
  {
    if (this->m_queue.empty())
    {
      throw std::out_of_range("[ERROR] Queue is empty.");
    }
  }

  return this->m_queue.back();
}

template<class T>
void NQueue<T>::Clear()
{
  std::lock_guard<std::mutex> lock(this->m_mutex);
  if (!this->m_queue.empty())
  {
    this->m_queue.clear();
  }
}

template<class T>
bool NQueue<T>::Empty()
{
  std::lock_guard<std::mutex> lock(this->m_mutex);
  return this->m_queue.empty();
}

template<class T>
const T &NQueue<T>::Front()
{
  std::unique_lock<std::mutex> lock(this->m_mutex);

  while (this->m_queue.empty())
  {
    this->m_cv.wait(lock);
  }

  return this->m_queue.front();
}

template<class T>
void NQueue<T>::Pop()
{
  std::unique_lock<std::mutex> lock(this->m_mutex);

  while (this->m_queue.empty())
  {
    this->m_cv.wait(lock);
  }

  this->m_queue.pop_front();
}

template<class T>
void NQueue<T>::Push(const T &item)
{
  std::lock_guard<std::mutex> lock(this->m_mutex);
  this->m_queue.push_back(item);
  this->m_cv.notify_all();
}

template<class T>
size_t NQueue<T>::Size()
{
  std::lock_guard<std::mutex> lock(this->m_mutex);
  return this->m_queue.size();
}
//NQueue}}}

//NThread{{{
template<class Fn, class ...Args>
NThread::NThread(Fn&& function, Args&&... arguments)
{
  this->m_lock = std::unique_lock<std::mutex>(this->m_mutex);
  this->m_thread = std::thread(function, arguments...);
}

inline NThread::~NThread()
{
  this->Stop();
  if (this->m_thread.joinable())
  {
    this->m_thread.join();
  }
}

inline std::error_code NThread::Start()
{
  if (this->m_thread.joinable())
  {
    this->m_thread.join();
  }
  else
  {
    NLogE("Failed to start thread, thread is not joinable.");
    return std::make_error_code(std::errc::operation_canceled);
  }

  return std::error_code();
}

inline void NThread::Notify()
{
  this->m_cv.notify_one();
}

inline void NThread::Stop()
{
  this->m_lock.unlock();
}

inline void NThread::Wait(bool should_wait)
{
  while (should_wait)
  {
    this->m_cv.wait(this->m_lock);
    break;
  }
}
//NThread}}}

//NTimer{{{
inline NTimer::NTimer()
  :m_tic(std::chrono::system_clock::now()), m_elapsed(0)
{
}

inline NTimer::~NTimer()
{
}

inline void NTimer::Tic(const bool &echo)
{
  this->m_tic = std::chrono::system_clock::now();

  if (echo)
  {
    NLogI("Timing started.");
  }
}

inline long long NTimer::Toc(const char *unit, const bool &echo)
{
  this->m_elapsed = NDuration(
      this->m_tic, std::chrono::system_clock::now(), unit);

  if (echo)
  {
    NLogI("Time elapsed: %lld (%s)", this->m_elapsed, unit);
  }

  return this->m_elapsed;
}
//NTimer}}}
//******************************************************************** Class}}}
#endif  // NEU_H
