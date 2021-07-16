/******************************************************************************
Author: Jihang Li (Jihang DOT Li AT outlook DOT com)
Last update: 2021-07-16 19:03
******************************************************************************/
#ifndef NEU_H
#define NEU_H

#include <stdio.h>
#include <stdlib.h>
#include <condition_variable>
#include <functional>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

#if defined(__GNUC__) || defined(__GNUG__)
#include <linux/limits.h>
#endif

//******************************************************************** Handy{{{
std::string NTimestamp();

#ifndef DLL_API
#if defined(_MSC_VER)
#define DLL_API __declspec(dllexport)
#elif defined(__GNUC__) || defined(__GNUG__)
#define DLL_API __attribute__ ((visibility("default")))
#endif
#endif

#if defined(_MSC_VER)
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

/* For debug build */
#if NEU_LOG_LEVEL < 0
#define NOneLine(fmt, ...) printf("\r[%s] " fmt, __FILENAME__, ##__VA_ARGS__); fflush(stdout); // Print on one line.
#define NErr(fmt, ...) printf("\033[0;31m[%s > %s > %d] " fmt "\n\033[0m", __FILENAME__, __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__)
#define NWarn(fmt, ...) printf("\033[0;33m[%s > %s] " fmt "\n\033[0m", __FILENAME__, __FUNCTION__, ##__VA_ARGS__)
#define NHint(fmt, ...) printf("\033[0;34m[%s] " fmt "\n\033[0m", __FILENAME__, ##__VA_ARGS__)
#define NInfo(fmt, ...) printf("\033[0;32m[%s] " fmt "\n\033[0m", NTimestamp().c_str(), ##__VA_ARGS__)
/* Undefine */
#elif NEU_LOG_LEVEL >= 0
#define NOneLine(fmt, ...)
#define NErr(fmt, ...)
#define NWarn(fmt, ...)
#define NHint(fmt, ...)
#define NInfo(fmt, ...)
#endif
/* For release build */
#if NEU_LOG_LEVEL >= 1
#undef NOneLine
#undef NErr
#define NOneLine(fmt, ...) printf("\r[%s] " fmt, NTimestamp().c_str(), ##__VA_ARGS__); fflush(stdout); // Print on one line.
#define NErr(fmt, ...) printf("\033[0;31m[ERROR %s] " fmt "\n\033[0m", NTimestamp().c_str(), ##__VA_ARGS__)
#endif
#if NEU_LOG_LEVEL >= 2
#undef NWarn
#define NWarn(fmt, ...) printf("\033[0;33m[WARN %s] " fmt "\n\033[0m", NTimestamp().c_str(), ##__VA_ARGS__)
#endif
#if NEU_LOG_LEVEL >= 3
#undef NHint
#define NHint(fmt, ...) printf("\033[0;34m[HINT %s] " fmt "\n\033[0m", NTimestamp().c_str(), ##__VA_ARGS__)
#endif
#if NEU_LOG_LEVEL >= 4
#undef NInfo
#define NInfo(fmt, ...) printf("\033[0;32m[INFO %s] " fmt "\n\033[0m", NTimestamp().c_str(), ##__VA_ARGS__)
#endif

#ifndef NArraySize
#define NArraySize(a) ((sizeof(a) / sizeof(*(a))) / static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))
#endif

#ifndef NBit
#define NBit(b) (1 << b)
#endif

#ifndef NClamp
#if defined(_MSC_VER)
#define NClamp(x, l, u) \
  [&](){ \
    decltype(x) _x = (x); \
    decltype(l) _l = (l); \
    decltype(u) _u = (u); \
    return _x <= _l ? _l : (_x <= _u ? _x : _u); \
  }()
#elif defined(__GNUC__) || defined(__GNUG__)
#define NClamp(x, l, u) \
  ({ \
    __typeof__(x) _x = (x); \
    __typeof__(l) _l = (l); \
    __typeof__(u) _u = (u); \
    _x <= _l ? _l : (_x <= _u ? _x : _u); \
  })
#endif
#endif

#ifndef NMax
#if defined(_MSC_VER)
#define NMax(x, y) \
  [&](){ \
    decltype(x) _x = (x); \
    decltype(y) _y = (y); \
    return _x >= _y ? _x : _y; \
  }()
#elif defined(__GNUC__) || defined(__GNUG__)
#define NMax(x, y) \
  ({ \
    __typeof__(x) _x = (x); \
    __typeof__(y) _y = (y); \
    _x >= _y ? _x : _y; \
  })
#endif
#endif

#ifndef NMin
#if defined(_MSC_VER)
#define NMin(x, y) \
  [&](){ \
    decltype(x) _x = (x); \
    decltype(y) _y = (y); \
    return _x <= _y ? _x : _y; \
  }()
#elif defined(__GNUC__)  || defined(__GNUG__)
#define NMin(x, y) \
  ({ \
    __typeof__(x) _x = (x); \
    __typeof__(y) _y = (y); \
    _x <= _y ? _x : _y; \
  })
#endif
#endif

#define NRelease(o) \
  if ((o) != nullptr) \
  { \
    delete (o); \
    (o) = nullptr; \
  }

inline std::string NAbsolutePath(const char *file)
{
#if defined(_MSC_VER)
  char abs_path[_MAX_PATH];
  if (!_fullpath(abs_path, file, _MAX_PATH))
  {
    return nullptr;
  }
#elif defined(__GNUC__)  || defined(__GNUG__)
  char abs_path[PATH_MAX];
  if (!realpath(file, abs_path))
  {
    return nullptr;
  }
#endif

  return std::string(abs_path);
}

inline long long NDuration(
    const std::chrono::system_clock::time_point &start,
    const std::chrono::system_clock::time_point &end,
    const std::string &unit = "ms")
{
  if (unit.compare("s") == 0)
  {
    return std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
  }
  else if (unit.compare("us") == 0)
  {
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  }
  else //  if (unit.compare("ms") == 0)  // Milliseconds by default
  {
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  }
}

inline bool NIsStringEmpty(const char *str)
{
  return (str == nullptr || str[0] == '\0');
}

inline void NSleep(const int &ms)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

inline std::string NTimestamp()
{
  time_t rawtime(0);
  struct tm timeinfo;

  time(&rawtime);
#if defined(_MSC_VER)
  localtime_s(&timeinfo, &rawtime);
#elif defined(__GNUC__) || defined(__GNUG__)
  localtime_r(&rawtime, &timeinfo);
#endif

  char buffer[50];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return std::string(buffer);
}

template<class T>
T NEnumAnd(const T &x, const T &y)
{
  return static_cast<T>(static_cast<int>(x) & static_cast<int>(y));
}

template<class T>
T NEnumNot(T x)
{
  return static_cast<T>(~static_cast<int>(x));
}

template<class T>
T NEnumOr(const T &x, const T &y)
{
  return static_cast<T>(static_cast<int>(x) | static_cast<int>(y));
}

template<class T>
T NEnumSubtract(const T &x, const T &y)
{
  return static_cast<T>(static_cast<int>(x) - static_cast<int>(y));
}
//******************************************************************** Handy}}}

//*************************************************************** Error Code{{{
typedef enum
{
  N_OK                = 0,
  N_FAIL              = -1,
  N_DATA_INVALID      = -2,
  N_INPUT_INVALID     = -3,
  N_OPERATION_ABORTED = -4,
} NRESULT;

static const std::map<NRESULT, const char*> k_nr_msgs =
{
  {N_OK,                "Succeeded"},
  {N_FAIL,              "Failed"},
  {N_DATA_INVALID,      "Data are invalid"},
  {N_INPUT_INVALID,     "Input is invalid"},
  {N_OPERATION_ABORTED, "Operation aborted"},
};

inline const char *NRMsg(NRESULT nr)
{
  std::map<const NRESULT, const char*>::const_iterator it = k_nr_msgs.find(nr);

  if (it != k_nr_msgs.end())
  {
    return it->second;
  }

  return "Unknown error";
}

#define NSucc(nr) (((NRESULT)(nr)) >= 0)
#define NFail(nr) (((NRESULT)(nr)) < 0)
//*************************************************************** Error Code}}}

//*********************************************************** Class & Struct{{{
template<class Ret, class ...Args>
class NCallback
{
public:
  template<class Obj, class Fn>
  NCallback(Fn &&function, Obj &&obj)
  {
    this->m_function = std::bind(function, obj, std::placeholders::_1);
  }

  template<class Fn>
  NCallback(Fn &&function)
  {
    this->m_function = std::function<Ret(Args...)>(function);
  }

  ~NCallback() {}

  Ret operator()(Args ...arguments)
  {
    return this->m_function(arguments...);
  }

private:
  std::function<Ret(Args...)> m_function;
};  // NCallback

class NMutexThread
{
public:
  template<class Fn, class ...Args>
  NMutexThread(Fn&& function, Args&&... arguments)
  {
    this->m_lock = std::unique_lock<std::mutex>(this->m_mutex);
    this->m_thread = std::thread(function, arguments...);
  }

  ~NMutexThread()
  {
    if (this->m_thread.joinable())
    {
      this->m_thread.join();
    }
  }

  NRESULT Start()
  {
    if (this->m_thread.joinable())
    {
      this->m_thread.join();
    }
    else
    {
      NErr("Failed to start thread, thread is not joinable.");
      return N_OPERATION_ABORTED;
    }

    return N_OK;
  }

  void Notify()
  {
    this->m_controller.notify_one();
  }

  void Wait(bool should_wait)
  {
    while (should_wait)
    {
      this->m_controller.wait(this->m_lock);
      break;
    }
  }

private:
  std::condition_variable_any m_controller;
  std::unique_lock<std::mutex> m_lock;
  std::mutex m_mutex;
  std::thread m_thread;
};  // class NMutexThread

/* Numeric vector, only works with numeric data. */
template<class T>
class NNVector
{
public:
  NNVector()
  {
    this->m_sum = (T)0;
  }

  ~NNVector() {}

  void Erase(const int &index)
  {
    this->m_sum -= this->m_data[index];

    if (index >= 0)
    {
      this->m_data.erase(this->m_data.begin() + index);
    }
    else
    {
      this->m_data.erase(this->m_data.end() + index);
    }
  }

  T Mean()
  {
    if (this->m_data.size() <= 0)
    {
      return (T)0;
    }

    return this->m_sum / this->m_data.size();
  }

  void Push(const T &data)
  {
    this->m_data.push_back(data);
    this->m_sum += data;
  }

  size_t Size()
  {
    return this->m_data.size();
  }

private:
  std::vector<T> m_data;
  T m_sum;
};  // class NNVector
//*********************************************************** Class & Struct}}}

//********************************************************************* Keys{{{
#define NK_ESC   0x1B
#define NK_LEFT  0x25
#define NK_UP    0x26
#define NK_RIGHT 0x27
#define NK_DOWN  0x28
#define NK_Q     0x51
//********************************************************************* Keys}}}

#endif  // NEU_H
