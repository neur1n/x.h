/******************************************************************************
MIT License

Copyright (c) 2021 Jihang Li (jihangli AT duck DOT com)

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


Last update: 2021-12-14 15:56
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

//The Must-Put-on-Top{{{
std::string NTimestamp(const char *format = "%Y-%m-%d %H:%M:%S");
//The Must-Put-on-Top}}}
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
#define NLOG_COLOR_I NCOLOR_GREEN(1)
#define NLOG_COLOR_D NCOLOR_BLUE(1)

#ifdef NDEBUG
#define NLOG_FORMAT(level, format) "[" level " %s] " format "\n" NCOLOR_RESET, NTimestamp().c_str()
#else
#define NLOG_FORMAT(level, format) "[" level " %s %s > %s > %d] " format "\n" NCOLOR_RESET, NTimestamp().c_str(), __FILENAME__, __FUNCTION__, __LINE__
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
#define NLogF(format, ...) printf(NLOG_COLOR_F NLOG_FORMAT("F", format), ##__VA_ARGS__); exit(EXIT_FAILURE)
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

#define NBit(bit) (1 << bit)

//{{{
typedef std::error_code NCode;
static const NCode NOK                         = std::error_code();
static const NCode NAF_NOT_SUPPORT             = std::make_error_code(std::errc::address_family_not_supported);
static const NCode NADDR_IN_USE                = std::make_error_code(std::errc::address_in_use);
static const NCode NADDR_NOT_AVAIL             = std::make_error_code(std::errc::address_not_available);
static const NCode NIS_CONN                    = std::make_error_code(std::errc::already_connected);
static const NCode NARG_LIST_TOO_LONG          = std::make_error_code(std::errc::argument_list_too_long);
static const NCode NARG_OUT_OF_DOM             = std::make_error_code(std::errc::argument_out_of_domain);
static const NCode NBAD_ADDR                   = std::make_error_code(std::errc::bad_address);
static const NCode NBAD_FD                     = std::make_error_code(std::errc::bad_file_descriptor);
static const NCode NBAD_MSG                    = std::make_error_code(std::errc::bad_message);
static const NCode NBAD_PIPE                   = std::make_error_code(std::errc::broken_pipe);
static const NCode NCONN_ABORTED               = std::make_error_code(std::errc::connection_aborted);
static const NCode NCONN_IN_PROGRESS           = std::make_error_code(std::errc::connection_already_in_progress);
static const NCode NCONN_REFUSED               = std::make_error_code(std::errc::connection_refused);
static const NCode NCONN_RESET                 = std::make_error_code(std::errc::connection_reset);
static const NCode NCROSS_DEV_LINK             = std::make_error_code(std::errc::cross_device_link);
static const NCode NDST_ADDR_REQ               = std::make_error_code(std::errc::destination_address_required);
static const NCode NBUSY                       = std::make_error_code(std::errc::device_or_resource_busy);
static const NCode NDIR_NOT_EMPTY              = std::make_error_code(std::errc::directory_not_empty);
static const NCode NEXE_FMT_ERR                = std::make_error_code(std::errc::executable_format_error);
static const NCode NFILE_EXISTS                = std::make_error_code(std::errc::file_exists);
static const NCode NFILE_TOO_BIG               = std::make_error_code(std::errc::file_too_large);
static const NCode NNAME_TOO_LONG              = std::make_error_code(std::errc::filename_too_long);
static const NCode NFUNC_NOT_SUPPORT           = std::make_error_code(std::errc::function_not_supported);
static const NCode NHOST_UNREACHABLE           = std::make_error_code(std::errc::host_unreachable);
static const NCode NID_REMOVED                 = std::make_error_code(std::errc::identifier_removed);
static const NCode NILLEGAL_BTYE_SEQ           = std::make_error_code(std::errc::illegal_byte_sequence);
static const NCode NIVALID_IO_CTRL             = std::make_error_code(std::errc::inappropriate_io_control_operation);
static const NCode NINTR                       = std::make_error_code(std::errc::interrupted);
static const NCode NINVALID_ARG                = std::make_error_code(std::errc::invalid_argument);
static const NCode NINVALID_SEEK               = std::make_error_code(std::errc::invalid_seek);
static const NCode NIO_ERR                     = std::make_error_code(std::errc::io_error);
static const NCode NIS_DIR                     = std::make_error_code(std::errc::is_a_directory);
static const NCode NMSG_TOO_LONG               = std::make_error_code(std::errc::message_size);
static const NCode NNET_DOWN                   = std::make_error_code(std::errc::network_down);
static const NCode NNET_RESET                  = std::make_error_code(std::errc::network_reset);
static const NCode NNET_UNREACHABLE            = std::make_error_code(std::errc::network_unreachable);
static const NCode NNO_BUF_SPACE               = std::make_error_code(std::errc::no_buffer_space);
static const NCode NNO_CHILD_PROCESS           = std::make_error_code(std::errc::no_child_process);
static const NCode NNO_LINK                    = std::make_error_code(std::errc::no_link);
static const NCode NNO_LOCK                    = std::make_error_code(std::errc::no_lock_available);
static const NCode NNO_DATA                    = std::make_error_code(std::errc::no_message_available);
static const NCode NNO_MSG                     = std::make_error_code(std::errc::no_message);
static const NCode NNO_PROTO_OPT               = std::make_error_code(std::errc::no_protocol_option);
static const NCode NNO_SPACE                   = std::make_error_code(std::errc::no_space_on_device);
static const NCode NNO_STREAM_RES              = std::make_error_code(std::errc::no_stream_resources);
static const NCode NNO_DEV                     = std::make_error_code(std::errc::no_such_device);
static const NCode NNO_IO                      = std::make_error_code(std::errc::no_such_device_or_address);
static const NCode NNO_ENTITY                  = std::make_error_code(std::errc::no_such_file_or_directory);
static const NCode NNO_PROCESS                 = std::make_error_code(std::errc::no_such_process);
static const NCode NNOT_DIR                    = std::make_error_code(std::errc::not_a_directory);
static const NCode NNOT_SOCKET                 = std::make_error_code(std::errc::not_a_socket);
static const NCode NNOT_STREAM                 = std::make_error_code(std::errc::not_a_stream);
static const NCode NNOT_CONN                   = std::make_error_code(std::errc::not_connected);
static const NCode NNO_MEM                     = std::make_error_code(std::errc::not_enough_memory);
static const NCode NNOT_SUPPORT                = std::make_error_code(std::errc::not_supported);
static const NCode NOP_CANCELED                = std::make_error_code(std::errc::operation_canceled);
static const NCode NOP_IN_PROGRESS             = std::make_error_code(std::errc::operation_in_progress);
static const NCode NOP_NOT_PERMIT              = std::make_error_code(std::errc::operation_not_permitted);
static const NCode NOP_NOT_SUPPORT             = std::make_error_code(std::errc::operation_not_supported);
static const NCode NOP_WOULD_BLOCK             = std::make_error_code(std::errc::operation_would_block);
static const NCode NOWNER_DEAD                 = std::make_error_code(std::errc::owner_dead);
static const NCode NPERMISSION_DENIED          = std::make_error_code(std::errc::permission_denied);
static const NCode NPROTO_ERR                  = std::make_error_code(std::errc::protocol_error);
static const NCode NPROTO_NOT_SUPPORT          = std::make_error_code(std::errc::protocol_not_supported);
static const NCode NRO_FS                      = std::make_error_code(std::errc::read_only_file_system);
static const NCode NDEAD_LOCK                  = std::make_error_code(std::errc::resource_deadlock_would_occur);
static const NCode NTRY_AGAIN                  = std::make_error_code(std::errc::resource_unavailable_try_again);
static const NCode NOUT_OF_RANGE               = std::make_error_code(std::errc::result_out_of_range);
static const NCode NNOT_RECOVERABLE            = std::make_error_code(std::errc::state_not_recoverable);
static const NCode NTIMER_EXPIRED              = std::make_error_code(std::errc::stream_timeout);
static const NCode NTXT_BUSY                   = std::make_error_code(std::errc::text_file_busy);
static const NCode NCONN_TIMED_OUT             = std::make_error_code(std::errc::timed_out);
static const NCode NTOO_MANY_FILES_OPEN_IN_SYS = std::make_error_code(std::errc::too_many_files_open_in_system);
static const NCode NTOO_MANY_FILES_OPEN        = std::make_error_code(std::errc::too_many_files_open);
static const NCode NTOO_MANY_LINKS             = std::make_error_code(std::errc::too_many_links);
static const NCode NTOO_MANY_SYMLINK_LEVELS    = std::make_error_code(std::errc::too_many_symbolic_link_levels);
static const NCode NOVERFLOW                   = std::make_error_code(std::errc::value_too_large);
static const NCode NWRONG_PROTO_TYPE           = std::make_error_code(std::errc::wrong_protocol_type);
//}}}

template<class T>
static constexpr T NPi = (T)3.141592653589793238462643383279502884197169399375;
//******************************************************************** Macro}}}

//************************************************************* Declarations{{{
//Function{{{
/* The followings are in Marco section.
  NLogP(const char *format, ...);
  NLogF(const char *format, ...);
  NLogE(const char *format, ...);
  NLogW(const char *format, ...);
  NLogI(const char *format, ...);
  NLogD(const char *format, ...);
  NBit(int bit);
*/

#if defined(_MSC_VER)
#else
int _kbhit();
#endif

bool NSucc(const NCode &code);

bool NFail(const NCode &code);

bool NPathExists(const char *path);

bool NStringEmpty(const char *string);

const char *NCodeMessage(const int &code);

const char *NCodeMessage(const NCode &code);

int NPressedKey();

long long NDuration(
    const std::chrono::steady_clock::time_point &start,
    const std::chrono::steady_clock::time_point &end,
    const std::string &unit = "ms");

void NSleep(const unsigned long long &ms);

std::string NFullPath(const char *path);

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
template<class R, class ...Args>
class NCallable
{
public:
  NCallable() = delete;
  ~NCallable() = default;

  NCallable(R (*function)(Args...));

  template<class T>
  NCallable(R (T::*function)(Args...), T *obj);

  R operator()(Args ...arguments);

private:
  template<class T, class... Args1, class... Args2>
  void Bind(R (T::*function)(Args1...), Args2&&... arguments);

  template<class T, class... Args1, class... Args2, size_t... I>
  void InternalBind(
      std::index_sequence<I...>, R (T::*function)(Args1...), Args2&&... arguments);

  std::function<R(Args...)> m_function;
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

  NCode Start();

  void Notify();

  void WaitUntil(bool ready);

private:
  std::condition_variable m_cv;
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
  std::chrono::steady_clock::time_point m_tic;
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

inline bool NSucc(const NCode &code)
{
  return !code ? true : false;
}

inline bool NFail(const NCode &code)
{
  return code ? true : false;
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

inline const char *NCodeMessage(const int &code)
{
  static std::string msg = std::system_category().message(code);
  return msg.c_str();
}

inline const char *NCodeMessage(const NCode &code)
{
  static std::string msg = code.message();
  return msg.c_str();
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
    const std::chrono::steady_clock::time_point &start,
    const std::chrono::steady_clock::time_point &end,
    const std::string &unit)
{
  if (unit.compare("h") == 0)
  {
    return std::chrono::duration_cast<std::chrono::hours>(end - start).count();
  }
  else if (unit.compare("m") == 0)
  {
    return std::chrono::duration_cast<std::chrono::minutes>(end - start).count();
  }
  else if (unit.compare("s") == 0)
  {
    return std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
  }
  else if (unit.compare("ms") == 0)
  {
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  }
  else if (unit.compare("us") == 0)
  {
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  }
  else  // if (unit.compare("ns") == 0)
  {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
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
NCallable<R, Args...>::NCallable(R (*function)(Args...))
{
  this->m_function = std::function<R(Args...)>(function);
}

template<class R, class ...Args>
template<class T>
NCallable<R, Args...>::NCallable(R (T::*function)(Args...), T *obj)
{
  this->Bind(function, obj);
}

template<class R, class ...Args>
R NCallable<R, Args...>::operator()(Args ...arguments)
{
  return this->m_function(arguments...);
}

template<class R, class ...Args>
template<class T, class... Args1, class... Args2>
void NCallable<R, Args...>::Bind(R (T::*function)(Args1...), Args2&&... arguments)
{
  this->InternalBind(
      std::make_index_sequence<sizeof...(Args1) - sizeof...(Args2) + 1>{},
      function, std::forward<Args2>(arguments)...);
}

template<class R, class ...Args>
template<class T, class... Args1, class... Args2, size_t... I>
void NCallable<R, Args...>::InternalBind(
    std::index_sequence<I...>, R (T::*function)(Args1...), Args2&&... arguments)
{
  this->m_function = std::bind(
      function, std::forward<Args2>(arguments)..., NVariadicPlaceholder<I>{}...);
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
  this->m_thread = std::thread(function, arguments...);
}

inline NThread::~NThread()
{
  if (this->m_thread.joinable())
  {
    this->m_thread.join();
  }
}

inline NCode NThread::Start()
{
  if (this->m_thread.joinable())
  {
    this->m_thread.join();
  }
  else
  {
    NLogE("Failed to start thread, thread is not joinable.");
    return NOP_CANCELED;
  }

  return NOK;
}

inline void NThread::Notify()
{
  std::unique_lock<std::mutex> lock(this->m_mutex);
  this->m_cv.notify_one();
}

inline void NThread::WaitUntil(bool ready)
{
  std::unique_lock<std::mutex> lock(this->m_mutex);
  while (!ready)
  {
    this->m_cv.wait(lock);
    return;
  }
}
//NThread}}}

//NTimer{{{
inline NTimer::NTimer()
  :m_tic(std::chrono::steady_clock::now()), m_elapsed(0)
{
}

inline NTimer::~NTimer()
{
}

inline void NTimer::Tic(const bool &echo)
{
  this->m_tic = std::chrono::steady_clock::now();

  if (echo)
  {
    NLogP("Timing started.");
  }
}

inline long long NTimer::Toc(const char *unit, const bool &echo)
{
  this->m_elapsed = NDuration(
      this->m_tic, std::chrono::steady_clock::now(), unit);

  if (echo)
  {
    NLogP("Time elapsed: %lld (%s)", this->m_elapsed, unit);
  }

  return this->m_elapsed;
}
//NTimer}}}
//******************************************************************** Class}}}
#endif  // NEU_H
