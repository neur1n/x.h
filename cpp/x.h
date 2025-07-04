/******************************************************************************
Copyright (c) 2023 Jihang Li
x.h is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan
PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details.


Last update: 2025-07-04 20:32
Version: v0.8.6
******************************************************************************/
#ifndef X_H
#define X_H x_version(0, 8, 6)


/** @internal
 * Table of Contents
 *
 * Feature Configuration
 * Architecture Detection
 * Compiler Detection
 * Operating System Detection
 * Platform Detection
 *
 * Headers
 *
 * Symbol Visibility
 * Miscellaneous
 *
 * Communication
 * Console IO
 * Date and Time
 * Error Handling
 * File System
 * Hardware
 * Mathematics
 * Memory Management
 * Standard IO
 * String
 *
 * IMPL_Communication
 * IMPL_Console_IO
 * IMPL_Date_and_Time
 * IMPL_Error_Handling
 * IMPL_File_System
 * IMPL_Hardware
 * IMPL_Mathematics
 * IMPL_Memory_Management
 * IMPL_Standard_IO
 * IMPL_String
 * @endinternal
 */

/// @brief Generate a version number.
/// @param major The major version number, ranges in [0, 99].
/// @param minor The minor version number, ranges in [0, 99].
/// @param patch The patch version number, ranges in [0, 99999].
/// @return The version number.
/// @remark A big range of patch allow the result version number specifically
///         work with [\_MSC\_FULL\_VER](https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-170).
#define x_version(major, minor, patch) \
  (((major) % 100) * 10000000 + ((minor) % 100) * 100000 + ((patch) % 100000))

/******************************************************************************
 * @name Feature Configuration
 * @{
 *****************************************************************************/
/// @brief Toggle the availability of CUDA driver API related functions.
/// @remark The `_CU` suffix follows the naming convention of the CUDA driver
///         API's prefix `cu`.
#ifndef X_ENABLE_CU
#define X_ENABLE_CU (0)
#endif

/// @brief Toggle the availability of CUDA runtime API related functions.
/// @remark The `_CUDA` suffix follows the naming convention of the CUDA
///         runtime API's prefix `cuda`.
#ifndef X_ENABLE_CUDA
#define X_ENABLE_CUDA (0)
#endif

#ifndef X_ENABLE_SOCKET
#define X_ENABLE_SOCKET (0)
#endif
/** @} */  // Feature Configuration

/******************************************************************************
 * @name Architecture Detection
 * @{
 *****************************************************************************/
#if INTPTR_MAX == INT64_MAX
#define X_32BIT (0)
#define X_64BIT (1)
#elif INTPTR_MAX == INT32_MAX
#define X_32BIT (1)
#define X_64BIT (0)
#else
#error "Only 32-bit or 64-bit architecture is supported."
#endif

#if defined(__arm__) || defined(__thumb__) || defined(_M_ARM)
#define X_ARM (1)
#else
#define X_ARM (0)
#endif

#if defined(__aarch64__)
#define X_ARM64 (1)
#else
#define X_ARM64 (0)
#endif

#if defined(i386) || defined(__i386) || defined(__i386__) \
  || defined(__i486__) || defined(__i586__) || defined(__i686__) \
  || defined(_M_IX86_) || defined(_X86_)
#define X_X86 (1)
#else
#define X_X86 (0)
#endif

#if defined(__amd64) || defined(__amd64__) || defined(__x86_64) \
  || defined(__x86_64__) || defined(_M_AMD64) || defined(_M_X64)
#define X_X64 (1)
#else
#define X_X64 (0)
#endif
/** @} */  // Architecture detection

/******************************************************************************
 * @name Compiler Detection
 * @{
 *****************************************************************************/
#if defined(__clang__)
#define X_CLANG x_version(__clang_major__, __clang_minor__, __clang_patchlevel__)
#else
#define X_CLANG (0)
#endif

#if defined(__GNUC__)
#define X_GCC x_version(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#else
#define X_GCC (0)
#endif

#if defined(_MSC_VER)
#define X_MSVC _MSC_FULL_VER
#else
#define X_MSVC (0)
#endif

#if defined(__NVCC__)
#define X_NVCC x_version(__CUDACC_VER_MAJOR__, __CUDACC_VER_MINOR__, __CUDACC_VER_BUILD__)
#else
#define X_NVCC (0)
#endif
/** @} */  // Compiler detection

/******************************************************************************
 * @name Operating System Detection
 * @{
 *****************************************************************************/
#if defined(__CYGWIX__)
#define X_CYGWIN x_version(CYGWIN_VERSION_API_MAJOR, CYGWIN_VERSION_API_MINOR, 0)
#else
#define X_CYGWIN (0)
#endif

#if defined(__gnu_linux__) || defined(__linux__)
#define X_LINUX (1)
#else
#define X_LINUX (0)
#endif

#if defined(Macintosh) || defined(macintosh)
#define X_MACOS x_version(9, 0, 0)
#elif defined(__APPLE__) && defined(__MACH__)
#define X_MACOS x_version(10, 0, 0)
#else
#define X_MACOS (0)
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) \
  || defined(__TOS_WIX__) || defined(__WINDOWS__)
#define X_WINDOWS (1)
#else
#define X_WINDOWS (0)
#endif
/** @} */  // Operating system detection

/******************************************************************************
 * @name Platform Detection
 * @{
 *****************************************************************************/
#if defined(__ANDROID__)
#define X_ANDROID (1)
#else
#define X_ANDROID (0)
#endif

#if defined(__MINGW32__) || defined(__MINGW64__)
#include <_mingw.h>
#define X_MINGW (1)
#else
#define X_MINGW (0)
#endif

#if defined(__MINGW32__)
#define X_MINGW32 x_version(__MINGW32_VERSION_MAJOR, __MINGW32_VERSION_MINOR, 0)
#else
#define X_MINGW32 (0)
#endif

#if defined(__MINGW64__)
#define X_MINGW64 x_version(__MINGW64_VERSION_MAJOR, __MINGW64_VERSION_MINOR, 0)
#else
#define X_MINGW64 (0)
#endif
/** @} */   // Platform detection

//****************************************************************** Headers{{{
#if X_CLANG || X_GCC
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#endif

#include <cassert>
#include <cerrno>
#include <cfloat>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>

#if (__cplusplus >= 202002L && (X_CLANG >= x_version(17, 0, 0) || X_GCC >= x_version(13, 0, 0) || X_MSVC >= x_version(19, 29, 0)))
#include <format>
#endif
#include <stdexcept>
#include <string>

#include <fcntl.h>
#include <sys/stat.h>

#if X_ENABLE_CU
#include <cuda.h>
#endif

#if X_ENABLE_CUDA
#include <cuda_runtime.h>
#endif

#if X_WINDOWS && X_MSVC
#if X_ENABLE_SOCKET
#pragma comment(lib, "Ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <windows.h>
#include <conio.h>
#elif X_GCC || (!X_MINGW && X_CLANG)
#if X_ENABLE_SOCKET
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

#include <cctype>
#include <climits>
#include <cstdlib>

#include <sys/ioctl.h>
#include <sys/sysinfo.h>
#include <termios.h>
#include <unistd.h>
#else
#error "Unsupported build environment."
#endif
// Headers}}}

/******************************************************************************
 * @name Symbol Visibility
 * @see [Microsoft Docs](https://docs.microsoft.com/en-us/cpp/cpp/dllexport-dllimport?view=msvc-170)
 *      and [GCC Wiki](https://gcc.gnu.org/wiki/Visibility).
 * @{
 *****************************************************************************/
#if X_WINDOWS
#define X_EXP __declspec(dllexport)
#else
#define X_EXP __attribute__ ((visibility("default")))
#endif

#if X_WINDOWS
#define X_IMP __declspec(dllimport)
#else
#define X_IMP __attribute__ ((visibility("hidden")))
#endif
/** @} */  // Symbol Visibility

/******************************************************************************
 * @name Miscellaneous
 * @{
 *****************************************************************************/
/// @brief Just a semantic placeholder for empty arguments.
#define X_EMPTINESS

#if X_WINDOWS
/// @brief Get the base name from a full path. (Windows)
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

/// @brief Unify the function signature macro between Windows and Linux.
#define __PRETTY_FUNCTION__ __FUNCSIG__

/// @brief Unify the path length limit macro between Windows and Linux.
#define X_PATH_MAX _MAX_PATH
#else
/// @brief Get the base name from a full path. (Linux)
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

/// @brief Unify the path length limit macro between Windows and Linux.
#define X_PATH_MAX PATH_MAX

/// @brief This is set for the `stat` function on Linux.
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS (64)
#endif
#endif

#define X_INL inline

template<typename T>
X_INL constexpr bool x_false = false;
/** @} */  // Miscellaneous

class x_error;

/******************************************************************************
 * @name Communication
 * @brief A collection of communication utilities.
 * @{
 *****************************************************************************/
/// @brief Calculate the CRC32 checksum of a data chunk.
X_INL uint32_t x_checksum_crc32(
    const void* data, const size_t size, const uint32_t* prev = nullptr);

/// @brief Calculate the internet checksum of a data chunk.
/// @see [RFC1071](https://www.rfc-editor.org/info/rfc1071)
X_INL uint16_t x_checksum_rfc1071(
    const void* data, const size_t size, const uint16_t* prev = nullptr);

/// @brief Calculate the XOR checksum of a data chunk.
X_INL uint8_t x_checksum_xor(
    const void* data, const size_t size, const uint8_t* prev = nullptr);

/// @brief The default start of frame for a packet.
#ifndef X_PACKET_SOF
#define X_PACKET_SOF (0x55AA)
#endif

/// @brief A macro to represent an infinite number of packets.
#ifndef X_PACKET_INF
#define X_PACKET_INF UINT64_MAX
#endif

/// @struct x_header
/// @brief The header of a packet.
/// @var x_header::sof
///      The start of frame.
/// @var x_header::ctl
///      The control code.
/// @var x_header::opt
///      The option, just use it freely.
/// @var x_header::cnt
///      The total number of packets.
/// @var x_header::idx
///      The index of the current packet.
/// @var x_header::dsz
///      The size of the data chunk.
/// @var x_header::cks
///      The checksum of the packet, which is calculated based on the header
///      and the data chunk.
typedef struct _x_header_
{
  uint16_t sof{X_PACKET_SOF};
  uint16_t ctl{0};
  uint32_t opt{0};
  uint64_t cnt{X_PACKET_INF};
  uint64_t idx{0};
  uint64_t dsz{0};
  uint64_t cks{0};
} x_header;

/// @struct x_packet
/// @brief A message packet.
/// @var x_packet::head
///      The header of the packet.
/// @var x_packet::body
///      The data chunk of the packet.
typedef struct _x_packet_
{
  x_header head;
  void* body{nullptr};
} x_packet;

/// @struct x_iovec
/// @brief An I/O vector.
/// @var x_iovec::buf
///      The buffer.
/// @var x_iovec::len
///      The length of the buffer.
typedef struct _x_iovec_
{
  void* buf{nullptr};
  size_t len{0};
} x_iovec;

#if X_ENABLE_SOCKET
/// @brief A class wrapping the socket operations.
class x_socket
{
public:
  /// @brief Constructor, a wrapper of `socket`.
  /// @note This constructor enables the `SO_KEEPALIVE` and
  ///       `SO_EXCLUSIVEADDRUSE` options by default but does not check the
  ///       return value of `setsockopt`.
  X_INL x_socket(const int domain, const int type, const int protocol);

  /// @brief Destructor.
  X_INL ~x_socket();

  /// @brief Accept a connection from a client.
  /// @param client The client to be accepted.
  /// @return An instance of @ref x_error.
  X_INL x_error accept(x_socket* client);

  /// @brief Query the IP address and port of the socket.
  /// @param ip The buffer to store the IP address.
  /// @param port The buffer to store the port.
  /// @return An instance of @ref x_error.
  X_INL x_error addr(char* ip, uint16_t* port);

  /// @brief Close the socket.
  /// @return An instance of @ref x_error.
  X_INL x_error close();

  /// @brief Connect to a server with specified IP address and port.
  /// @param ip The IP address of the server.
  /// @param port The port of the server.
  /// @return An instance of @ref x_error.
  X_INL x_error connect(const char* ip, const uint16_t port);

  /// @brief Wrapper of `getsockopt` with error handling.
  /// @see getsockopt
  /// @return An instance of @ref x_error.
  X_INL x_error getopt(
      const int level, const int option, void* value, socklen_t* length);

  /// @brief Listen on a specified IP address and port.
  /// @param ip The IP address to listen on.
  /// @param port The port to listen on.
  /// @return An instance of @ref x_error.
  X_INL x_error listen(const char* ip, const uint16_t port);

  /// @brief Wrapper of `recv` with error handling.
  /// @return An instance of @ref x_error.
  /// @see recv
  /// @remark Different from the standard `recv`, this function trys to receive
  ///         the specified size of data before returning.
  X_INL x_error recv(void* buffer, const size_t size, const int flags);

  /// @brief Vectored version of `recv`.
  /// @return An instance of @ref x_error.
  /// @see @ref x_socket::recv
  X_INL x_error recvv(x_iovec* iovec, const size_t count, const int flags);

  /// @brief Wrapper of `send` with error handling.
  /// @return An instance of @ref x_error.
  /// @see send
  /// @remark Different from the standard `send`, this function trys to send
  ///         the specified size of data before returning.
  X_INL x_error send(const void* buffer, const size_t size, const int flags);

  /// @brief Vectored version of `send`.
  /// @return An instance of @ref x_error.
  /// @see @ref x_socket::send
  X_INL x_error sendv(const x_iovec* iovec, const size_t count, const int flags);

  /// @brief Wrapper of `setsockopt` with error handling.
  /// @see setsockopt
  /// @return An instance of @ref x_error.
  X_INL x_error setopt(
      const int level, const int option, const void* value, const socklen_t length);

private:
#if X_WINDOWS
  SOCKET m_hndl{INVALID_SOCKET};
#else
  int m_hndl{-1};
#endif
  struct sockaddr m_addr{0};
  int m_domain{AF_UNSPEC};
};
#endif  // X_ENABLE_SOCKET
/** @} */  // Communication

/******************************************************************************
 * @name Console IO
 * @brief A collection of console IO utilities.
 * @see [Virtual-Key Codes](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes)
 * @remark X_KEY_LEFT, X_KEY_UP, X_KEY_RIGHT, and X_KEY_DOWN are defined as
 *         negative values since they are handled differently in Windows and
 *         Linux.
 * @{
 *****************************************************************************/
#define X_KEY_ESC   (0x1B)
#define X_KEY_A     (0x41)
#define X_KEY_B     (0x42)
#define X_KEY_C     (0x43)
#define X_KEY_D     (0x44)
#define X_KEY_Q     (0x51)
#if X_WINDOWS
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

#if !X_WINDOWS
/// @brief Checks the console for keyboard input. (Linux)
/// @return Returns a non-zero value if a key is pressed, 0 otherwise.
/// @see [_kbhit](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/kbhit?view=msvc-170)
X_INL int _kbhit();
#endif

/// @brief A wrapper for Win32's `getch`, and an implementation for Linux.
/// @return Returns the character read from the console, or 0 if no character
///         is available.
X_INL int x_getch();
/** @} */  // Console IO

/******************************************************************************
 * @name Date and Time
 * @brief A collection of date and time utilities.
 * @{
 *****************************************************************************/
/// @brief Sleep for a specified amount of time.
/// @param ms The amount of time to sleep in milliseconds.
X_INL void x_sleep(const unsigned long ms);

/// @brief Get the current timestamp.
/// @param buffer The buffer to store the timestamp.
/// @param size The size of the buffer.
/// @return The current timestamp, same as `buf`.
/// @remark This function calls `ctime_s` on Windows and `ctime_r` on Linux
///         internally and uses their return values to fill the buffer.
///         Therefore, the format is not customizable. A buffer with at least
///         26 bytes is guaranteed to store the timestamp.
X_INL const char* x_timestamp(char* buffer, const size_t size);

/// @struct x_event_stats
/// @brief A structure to store the statistics of a stopwatch.
/// @var x_event_stats::ready
///      Whether the stopwatch is ready.
/// @var x_event_stats::cyc
///      The number of cycles.
/// @var x_event_stats::sum
///      The total elapsed time.
/// @var x_event_stats::avg
///      The average elapsed time.
/// @var x_event_stats::max
///      The frame that captures the maximum elapsed time.
/// @var x_event_stats::min
///      The frame that captures the minimum elapsed time.
typedef struct _x_event_stats_
{
  /// @brief Echo the statistics in a predefined format.
  /// @param unit The unit of the statistics, "h", "m", "s", "ms", "us", or "ns".
  /// @param title The title for the statistics. Default is "STATS". If the
  ///              title is not a valid string, "STATS" will be used.
  /// @param stream The output stream. Default is `stdout`.
  /// @remark This helper function provides a handy way to print the
  ///         statistics. Users may use their own format to print the
  ///         statistics with the data members of this structure.
  X_INL void echo(
      const char* unit, const char* title = "STATS", FILE* const stream = stdout);

  /// @brief Reset the statistics.
  X_INL void reset();

  bool ready{false};
  size_t cyc{0};
  double sum{0.0};
  double avg{0.0};
  struct
  {
    size_t idx;
    double val;
  } max{0, DBL_MIN}, min{0, DBL_MAX};
} x_event_stats;

class x_event
{
public:
  X_INL x_event(const char* type, const unsigned int flags = 0);

  X_INL ~x_event();

  /// @brief Calculate the duration between two events.
  /// @param unit The unit of the duration, "h", "m", "s", "ms", "us", or "ns".
  /// @return The elapsed time.
  /// @attention Before calling this function, @ref x_event::tic and
  ///            @ref x_event::toc must be called.
  X_INL double duration(const char* unit);

  /// @brief Start the recording.
  /// @param stream The optional stream to perform the recording. It is only
  ///               used when the CUDA driver or runtime API is enabled.
  /// @param flags The optional flags to perform the recording. It is only used
  ///              when the CUDA driver or runtime API is enabled.
  X_INL void tic(void* stream = nullptr, const unsigned int flags = 0);

  /// @brief Stop the recording.
  /// @param stream The optional stream to perform the recording. It is only
  ///               used when the CUDA driver or runtime API is enabled.
  /// @param flags The optional flags to perform the recording. It is only used
  ///              when the CUDA driver or runtime API is enabled.
  X_INL void toc(void* stream = nullptr, const unsigned int flags = 0);

  /// @brief Stop the event recording and return the statistics.
  /// @param stats The statistics of the event.
  /// @param cycle The number of cycles.
  X_INL void toc(
      x_event_stats& stats, const size_t cycle,
      void* stream = nullptr, const unsigned int flags = 0);

private:
  union
  {
    struct timespec cpu;
#if X_ENABLE_CU
    CUevent cu;
#endif
#if X_ENABLE_CUDA
    cudaEvent_t cuda;
#endif
  } m_start{0}, m_stop{0};

  int32_t m_type{0};  // For faster branching.
};
/** @} */  // Date and Time

/******************************************************************************
 * @name Error Handling
 * @brief A collection of error handling utilities.
 * @{
 *****************************************************************************/
/// @brief Assertion with optional message.
/// @param expr The expression to assert.
/// @param ... The optional message to print.
/// @attention The optional message must be a C-style string.
#define x_assert(expr, ...) do { \
  if (!(expr)) { \
    fprintf(stderr, "Assertion failed: %s\n", #expr); \
    _x_assert_msg(__VA_ARGS__); \
    fprintf(stderr, "Position: %s:%lld: %s\n", \
        __FILENAME__, static_cast<long long>(__LINE__), __PRETTY_FUNCTION__); \
    abort(); \
  } \
} while (false)

/// @brief Wrapping the error handling of a function call.
/// @param category The error category, should be supported by @ref x_error.
/// @param error The error object, should be an instance of @ref x_error.
/// @param function The function to call.
/// @param ... The arguments of the function.
#define x_check(category, error, function, ...) do {\
  error = _x_check_impl(category, function, ##__VA_ARGS__); \
  if (error) { \
    _x_log_impl<'e'>(__FILENAME__, #function, static_cast<long long>(__LINE__), stderr, "%s", error.msg()); \
  } \
} while (false)

/// @brief Check if an instance of @ref x_error indicates a failure.
/// @param error The instance of @ref x_error.
/// @return `true` if the instance is indicating a faiure, `false` otherwise.
/// @see @ref x_succ
/// @remark Using this function is not necessary since there is a boolean
///         operator defined for @ref x_error. This is provided to align with
///         the C version of this library.
X_INL bool x_fail(const x_error& error);

/// @brief The counterpart of x_fail.
/// @see @ref x_fail
X_INL bool x_succ(const x_error& error);

/// @brief An error class that encapsulates the error category, error value,
///        and corresponding error message.
/// @remark The error handling class `std::error_code` is extensible but not
///         user-friendly in my opinion. This class is designed to work with
///         different error categories with less effort.
class x_error
{
public:
  /// @brief Default constructor, which sets the error category to "posix" and
  ///        the error value to 0.
  X_INL explicit x_error();

  /// @brief Constructor with a error category. Corresponding error code and
  ///        message will be queried based on the error category. For example,
  ///        if the error category is "posix", the error value will be set to
  ///        `errno` and the error message will be set to `strerror(errno)`.
  /// @attention Using "custom" here is not supported since the sources of
  ///            error value and message are unknown.
  X_INL explicit x_error(const char* category);

  /// @brief Constructor with an error category, an error value, and an
  ///        optional failure predicate. The failure predicate is used to
  ///        determine if the error value is a failure or not. The error
  ///        message will be queried based on the error category and the error
  ///        value. For example, if the error category is "posix" and the error
  ///        value is `EINVAL`, the error message will be set to
  ///        `strerror(EINVAL)`.
  /// @note If the error category is "custom", the error message will be set to
  ///       "Custom error X" where X is the error value `val`.
  /// @attention If the error category is "custom", the failure predicate is
  ///            mandatory.
  X_INL explicit x_error(
      const char* category, const int32_t value,
      bool (*fail)(const int32_t) = nullptr);

  /// @brief Constructor with an error category, an error value, and a custom
  ///        predicate.
  X_INL explicit x_error(
      const char* category, const int32_t value, const char* message,
      bool (*fail)(const int32_t) = nullptr);

  /// @brief Destructor.
  X_INL ~x_error();

  X_INL x_error& set(const char* category);

  X_INL x_error& set(
      const char* category, const int32_t value,
      bool (*fail)(const int32_t) = nullptr);

  X_INL x_error& set(
      const char* category, const int32_t value, const char* message,
      bool (*fail)(const int32_t) = nullptr);

  /// @brief Get the error category.
  X_INL const char* cat();

  /// @brief Get the error message.
  X_INL const char* msg();

  /// @brief Get the error value.
  X_INL int32_t val() const;

  /// @brief A boolean operator to check if the error value is a failure. It
  ///        calls the failure predicate if it is set.
  X_INL operator bool() const;

private:
  const char* m_cat{"posix"};
  std::string m_msg;
  int32_t m_val{0};
  bool (*m_fail)(const int32_t){nullptr};
};
/** @} */  // Error Handling

/******************************************************************************
 * @name File System
 * @brief A collection of file system utilities.
 * @{
 *****************************************************************************/
/// @brief Query if a file or directory exists.
/// @param file The file or directory to query.
/// @return `true` if the file or directory exists, `false` otherwise.
X_INL bool x_fexist(const char* file);

/// @brief Open a file stream with error handling.
/// @param stream The file stream to open.
/// @param file The file to open.
/// @param mode The mode to open the file.
/// @return An instance of @ref x_error.
/// @remark A wrapper of `fopen` with error handling, as well as for `fopen_s`
///         on Windows to avoid the warning C4996.
X_INL x_error x_fopen(FILE** stream, const char* file, const char* mode);

/// @brief Get the full path of a file or directory.
/// @param dst The destination buffer to store the full path.
/// @param src The source file or directory.
/// @return The full path of the file or directory. Same as `dst`.
X_INL const char* x_fpath(char* dst, const char* src);

/// @brief Get the size of a file.
/// @param file The file to query.
/// @return The size of the file. If an error occurs, the return value is -1.
X_INL int64_t x_fsize(const char* file);

/// @brief Split a path into root, directory, file, and extension.
/// @param path The path to split.
/// @param root The buffer to store the root.
/// @param rsz The size of the root buffer.
/// @param dir The buffer to store the directory.
/// @param dsz The size of the directory buffer.
/// @param file The buffer to store the base name.
/// @param fsz The size of the file buffer.
/// @param ext The buffer to store the extension.
/// @param esz The size of the extension buffer.
X_INL x_error x_split_path(
    const char* path,
    char* root, const size_t rsz, char* dir, const size_t dsz,
    char* file, const size_t fsz, char* ext, const size_t esz);
/** @} */  // File System

/******************************************************************************
 * @name Hardware
 * @brief A collection of memory hardware utilities.
 * @{
 *****************************************************************************/
/// @brief Get the number of CPU cores.
X_INL size_t x_ncpu();

/// @brief Get the number of GPU devices.
/// @param api The API to query, must be one of "cu" or "cuda". If the API is
///            not supported, the return value is 0.
X_INL size_t x_ngpu(const char* api);
/** @} */  // Hardware

/******************************************************************************
 * @name Mathematics
 * @brief A collection of mathematical utilities.
 * @{
 *****************************************************************************/
/// @brief Constant PI of user-defined type.
/// @var x_Pi
/// @param T The user-defined type.
/// @return The constant PI.
template<typename T>
static constexpr T x_Pi = static_cast<T>(3.141592653589793238462643383279502884197169399375);

/// @brief Kibibyte constant generator, i.e., 1 KiB = 1024 bytes.
/// @param n The scale factor.
template<typename T>
X_INL constexpr T x_KiB(const T n);

/// @brief Mebibyte constant generator, i.e., 1 MiB = 1048576 bytes.
/// @param n The scale factor.
template<typename T>
X_INL constexpr T x_MiB(const T n);

/// @brief Gibibyte constant generator, i.e., 1 GiB = 1073741824 bytes.
/// @param n The scale factor.
template<typename T>
X_INL constexpr T x_GiB(const T n);

/// @brief Tebibyte constant generator, i.e., 1 TiB = 1099511627776 bytes.
/// @param n The scale factor.
template<typename T>
X_INL constexpr T x_TiB(const T n);

/// @brief Pebibyte constant generator, i.e., 1 PiB = 1125899906842620 bytes.
/// @param n The scale factor.
template<typename T>
X_INL constexpr T x_PiB(const T n);

/// @brief A macro used to generate an integer with only the n-th bit set to 1.
///        This is useful when one needs enumerations like `0b0001`, `0b0010`,
///        `0b0100` to perform the `&`, `|`, `~` operations.
/// @param n The n-th bit.
/// @see C++'s `std::bitset` for a more versatile solution.
template<typename T>
X_INL constexpr T x_bit(const T n);

/// @brief Calculate the greatest common divisor of two integers.
/// @param m The first integer.
/// @param n The second integer.
/// @return The greatest common divisor.
/// @attention This function is only available for integral types.
template<typename T>
X_INL constexpr T x_gcd(const T m, const T n);

/// @brief Calculate the least common multiple of two integers.
/// @param m The first integer.
/// @param n The second integer.
/// @return The least common multiple.
/// @attention This function is only available for integral types.
template<typename T>
X_INL constexpr T x_lcm(const T m, const T n);

/// @brief Calculate the next exponent of a base.
/// @param base The base.
/// @param src The source number.
/// @return The next exponent of the base.
/// @attention This function is only available for integral types.
template<typename T>
X_INL constexpr T x_next_exp(const T base, const T src);

/// @brief Calculate the next multiple of a base.
/// @param base The base.
/// @param src The source number.
/// @return The next multiple of the base.
/// @attention This function is only available for integral types.
template<typename T>
X_INL constexpr T x_next_mul(const T base, const T src);

/// @brief Calculate the previous exponent of a base.
/// @param base The base.
/// @param src The source number.
/// @return The previous exponent of the base.
/// @attention This function is only available for integral types.
template<typename T>
X_INL constexpr T x_prev_exp(const T base, const T src);

/// @brief Calculate the previous multiple of a base.
/// @param base The base.
/// @param src The source number.
/// @return The previous multiple of the base.
/// @attention This function is only available for integral types.
template<typename T>
X_INL constexpr T x_prev_mul(const T base, const T src);
/** @} */  // Mathematics

/******************************************************************************
 * @name Memory Management
 * @brief A collection of memory management utilities.
 * @{
 *****************************************************************************/
/// @brief Get number of items in an array.
/// @param array The array.
/// @attention This function only works with static arrays.
template<typename T, size_t N>
X_INL constexpr size_t x_count(const T (&array)[N]);

/// @brief Delete a pointer, allocated by `new` or `new[]`, and set it to
///        `nullptr`.
/// @tparam array Whether the pointer is allocated by `new[]`.
/// @param ptr The pointer to delete.
template<bool array, typename T>
X_INL void x_delete(T*& ptr);

/// @brief Free a memory block allocated on the heap and set it to `nullptr`.
/// @param ptr The memory block to free.
/// @remark If the pointer is `nullptr`, this function does nothing.
template<typename T>
X_INL void x_free(T*& ptr);

template<typename T>
X_INL x_error x_malloc(T** ptr, const size_t size);

X_INL x_error x_memcpy(void* dst, const void* src, const size_t size);

X_INL x_error x_meminfo(const char* type, size_t* avail, size_t* total);

/// @note Current supported types are:
///            - "cu"
///            - "cuda"
template<typename T>
X_INL const char* x_memtype(const char* type, const T ptr);
/** @} */  // Memory Management

/******************************************************************************
 * @name Standard IO
 * @brief A collection of standard IO utilities.
 * @{
 *****************************************************************************/
#define X_LOG_NONE   (-1)
#define X_LOG_PLAIN   (0)
#define X_LOG_FATAL   (1)
#define X_LOG_ERROR   (2)
#define X_LOG_WARNING (3)
#define X_LOG_INFO    (4)
#define X_LOG_DEBUG   (5)

/// @brief The highest log level to print.
#ifndef X_LOG_LEVEL
#ifdef NDEBUG
#define X_LOG_LEVEL X_LOG_INFO
#else
#define X_LOG_LEVEL X_LOG_DEBUG
#endif
#endif

/// @brief The maximum length of the log prefix.
#ifndef X_LOG_PREFIX_LIMIT
#ifdef NDEBUG
#define X_LOG_PREFIX_LIMIT (64)
#else
#define X_LOG_PREFIX_LIMIT (256)
#endif
#endif

/// @brief The maximum length of the log message.
#ifndef X_LOG_MSG_LIMIT
#define X_LOG_MSG_LIMIT (256)
#endif

/// @brief A function providing similar functionality to `std::vformat`.
/// @param buffer The buffer to store the formatted string.
/// @param size The size of the buffer.
/// @param format The format string.
/// @param args The optional arguments.
/// @return The formatted string, same as `buffer`. If `buffer` is `nullptr` or
///         `size` is 0, `format` is returned.
/// @note This function calls `std::vformat` if C++20 is available.
template<typename... Args>
X_INL const char* x_fmt(
    char* buffer, const size_t size, const char* format, Args... args);

/// @brief Log a message with a specified log level.
/// @param level The log level, one of 'p', 'f', 'e', 'w', 'i', 'd'.
/// @param stream The optional file stream to save the log.
/// @param format The format string as in `printf`.
/// @param ... The optional arguments as in `printf`.
/// @attention The log level is case insensitive, and must be known at compile
///            time.
#define x_log(level, stream, format, ...) do { \
  _x_log_impl<level>(__FILENAME__, __FUNCTION__, __LINE__, stream, format, ##__VA_ARGS__); \
} while (false)
/** @} */  // Standard IO

/******************************************************************************
 * @name String
 * @brief A collection of string utilities.
 * @{
 *****************************************************************************/
/// @brief Copy a string with error handling.
X_INL x_error x_strcpy(char* dst, const size_t size, const char* src);

/// @brief Check if a string is empty.
X_INL bool x_strmty(const char* string);
/** @} */  // String

//******************************************************* IMPL_Communication{{{
X_INL uint32_t x_checksum_crc32(
    const void* data, const size_t size, const uint32_t* prev)
{
  const uint8_t* d{static_cast<const uint8_t*>(data)};
  size_t cnt{size / sizeof(uint8_t)};
  int i{0};

  uint32_t cks{prev ? ~(*prev) : 0xFFFFFFFF};

  while (cnt--) {
    cks ^= *d++;

    for (i = 0; i < 8; ++i) {
      cks = (cks >> 1) ^ (-static_cast<int32_t>(cks & 1) & 0xEDB88320);
    }
  }

  return ~cks;
}

X_INL uint16_t x_checksum_rfc1071(
    const void* data, const size_t size, const uint16_t* prev)
{
  const uint16_t* d{static_cast<const uint16_t*>(data)};
  size_t cnt{size / sizeof(uint8_t)};
  uint32_t cks{prev ? static_cast<uint32_t>(~(*prev)) : 0};

  while (cnt > 1) {
    cks += *d++;
    cnt -= 2;
  }

  if (cnt > 0) {
    cks += *d;
  }

  while (cks >> 16) {
    cks = (cks & 0xFFFF) + (cks >> 16);
  }

  return static_cast<uint16_t>(~cks);
}

X_INL uint8_t x_checksum_xor(
    const void* data, const size_t size, const uint8_t* prev)
{
  const uint8_t* d8{static_cast<const uint8_t*>(data)};
  const uint64_t* d64{(const uint64_t*)data};
  const size_t dsz{sizeof(uint64_t)};
  const size_t cnt{size / dsz};

  union {
    uint8_t u8[8];
    uint64_t u64;
  } cks{{0}};

  if (prev) {
    cks.u8[0] = *prev;
  }

  size_t i{0};
  for (i = 0; i < (cnt & (~0x07)); i += 8) {
    cks.u64 ^= d64[i] ^ d64[i + 1] ^ d64[i + 2] ^ d64[i + 3]
      ^ d64[i + 4] ^ d64[i + 5] ^ d64[i + 6] ^ d64[i + 7];
  }

  for (size_t j = i * dsz; j < size; ++j) {
    cks.u8[0] ^= d8[j];
  }

  cks.u8[0] ^= cks.u8[1] ^ cks.u8[2] ^ cks.u8[3]
    ^ cks.u8[4] ^ cks.u8[5] ^ cks.u8[6] ^ cks.u8[7];

  return cks.u8[0];
}

#if X_ENABLE_SOCKET
// class x_socket{{{
X_INL x_socket::x_socket(const int domain, const int type, const int protocol)
  :m_domain{domain}
{
#if X_WINDOWS
  WSADATA data{0};
  if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
    throw std::runtime_error(
        std::string("WSAStartup: ") + x_error("socket").msg());
  }
#endif

  this->m_hndl = socket(domain, type, protocol);
#if X_WINDOWS
  if (this->m_hndl  == INVALID_SOCKET) {
    throw std::runtime_error(std::string("socket: ") + x_error("socket").msg());
  }
#else
  if (this->m_hndl  == -1) {
    throw std::runtime_error(std::string("posix: ") + x_error("socket").msg());
  }
#endif

  int val{1};
  socklen_t len{static_cast<socklen_t>(sizeof(val))};
  setsockopt(this->m_hndl, SOL_SOCKET, SO_KEEPALIVE, (char*)&val, len);
#if X_WINDOWS
  setsockopt(this->m_hndl, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char*)&val, len);
#else
  val = 0;
  setsockopt(this->m_hndl, SOL_SOCKET, SO_REUSEADDR, (char*)&val, len);
#endif
}

X_INL x_socket::~x_socket()
{
  this->close();
}

X_INL x_error x_socket::accept(x_socket* client)
{
  if (client == nullptr) {
    return x_error("posix", EINVAL);
  }

  struct sockaddr addr{0};
  socklen_t len{0};

#if X_WINDOWS
  SOCKET hndl = ::accept(this->m_hndl, &addr, &len);
  if (hndl == INVALID_SOCKET) {
    return x_error("socket");
  }
#else
  int hndl = ::accept(this->m_hndl, &addr, &len);
  if (hndl == -1) {
    return x_error("socket");
  }
#endif

  client->m_addr = std::move(addr);
  client->m_hndl = std::move(hndl);

  return x_error();
}

X_INL x_error x_socket::addr(char* ip, uint16_t* port)
{
  if (ip == nullptr || port == nullptr) {
    return x_error("posix", EINVAL);
  }

  struct sockaddr_in* sin{reinterpret_cast<struct sockaddr_in*>(&this->m_addr)};

  if (inet_ntop(this->m_domain, &sin->sin_addr, ip, 16) == nullptr) {
    return x_error("socket");
  }

  *port = sin->sin_port;

  return x_error();
}

X_INL x_error x_socket::close()
{
#if X_WINDOWS
  if (closesocket(this->m_hndl) != 0) {
    return x_error("socket");
  }

  return WSACleanup() == 0 ? x_error() : x_error("socket");
#else
  return ::close(this->m_hndl) == 0 ? x_error() : x_error("socket");
#endif
}

X_INL x_error x_socket::connect(const char* ip, const uint16_t port)
{
  struct sockaddr_in sin{0};
  sin.sin_family = this->m_domain;
  sin.sin_port = htons(port);
  int ierr{inet_pton(this->m_domain, ip, &sin.sin_addr)};
  if (ierr == 0) {
    return x_error("posix", EFAULT);
  } else if (ierr == -1) {
    return x_error("socket");
  }

  memcpy(&this->m_addr, &sin, sizeof(struct sockaddr));

  return ::connect(this->m_hndl, &this->m_addr, sizeof(struct sockaddr)) == 0
    ? x_error() : x_error("socket");
}

X_INL x_error x_socket::getopt(
    const int level, const int option, void* value, socklen_t* length)
{
  if (value == nullptr || length == nullptr) {
    return x_error("posix", EINVAL);
  }

  return getsockopt(this->m_hndl, level, option, (char*)value, length) == 0
    ? x_error() : x_error("socket");
}

X_INL x_error x_socket::listen(const char* ip, const uint16_t port)
{
  struct sockaddr_in sin{0};
  sin.sin_family = this->m_domain;
  sin.sin_port = htons(port);

  int ierr{inet_pton(this->m_domain, ip, &sin.sin_addr)};
  if (ierr == 0) {
    return x_error("posix", EFAULT);
  } else if (ierr == -1) {
    return x_error("socket");
  }

  memcpy(&this->m_addr, &sin, sizeof(struct sockaddr));

  ierr = bind(this->m_hndl, &this->m_addr, sizeof(struct sockaddr_in));
  if (ierr == 0) {
    ierr = ::listen(this->m_hndl, SOMAXCONN);
  }

  return ierr == 0 ? x_error() : x_error("socket");
}

X_INL x_error x_socket::recv(void* buffer, const size_t size, const int flags)
{
  if (buffer == nullptr || size == 0) {
    return x_error("posix", EINVAL);
  }

#if X_WINDOWS
  int remain{static_cast<int>(size)};
  int bytes{0};
#else
  size_t remain{size};
  ssize_t bytes{0};
#endif
  size_t offset{0};

  while (remain > 0) {
    bytes = ::recv(this->m_hndl, (char*)(buffer) + offset, remain, flags);
    if (bytes <= 0) {
      return x_error("socket");
    }

    offset += bytes;
    remain -= bytes;
  }

  return x_error();
}

X_INL x_error x_socket::recvv(
    x_iovec* iovec, const size_t count, const int flags)
{
  if (iovec == nullptr || count == 0) {
    return x_error("posix", EINVAL);
  }

  size_t total{0};
  for (size_t i = 0; i < count; ++i) {
    if (iovec[i].buf == nullptr || iovec[i].len == 0) {
      return x_error("posix", EINVAL);
    }

    if (iovec[i].len > (SIZE_MAX - total)) {
      return x_error("posix", EOVERFLOW);
    }

    total += iovec[i].len;
  }

  // NOTE: _alloca/alloca may be used if all data received are rather small.
  void* buf = malloc(total);
  if (buf == nullptr) {
    return x_error("posix");
  }

  x_error err = this->recv(buf, total, flags);
  if (!err) {
    size_t offset{0};
    for (size_t i = 0; i < count; ++i) {
      memcpy(iovec[i].buf, (char*)buf + offset, iovec[i].len);
      offset += iovec[i].len;
    }
  }

  free(buf);

  return err;
}

X_INL x_error x_socket::send(
    const void* buffer, const size_t size, const int flags)
{
#if X_WINDOWS
  int remain{static_cast<int>(size)};
#else
  size_t remain{size};
#endif
  size_t offset{0};
  int bytes{0};

  while (remain > 0) {
    bytes = ::send(this->m_hndl, (char*)(buffer) + offset, remain, flags);
    if (bytes <= 0) {
      return x_error("socket");
    }

    offset += bytes;
    remain -= bytes;
  }

  return x_error();
}

X_INL x_error x_socket::sendv(
    const x_iovec* iovec, const size_t count, const int flags)
{
  if (iovec == nullptr || count == 0) {
    return x_error("posix", EINVAL);
  }

  size_t total{0};
  for (size_t i = 0; i < count; ++i) {
    if (iovec[i].buf == nullptr || iovec[i].len == 0) {
      return x_error("posix", EINVAL);
    }

    if (iovec[i].len > (SIZE_MAX - total)) {
      return x_error("posix", EOVERFLOW);
    }

    total += iovec[i].len;
  }

  // NOTE: _alloca/alloca may be used if all data sent are rather small.
  void* buf = malloc(total);
  if (buf == nullptr) {
    return x_error("posix");
  }

  size_t offset{0};
  for (size_t i = 0; i < count; ++i) {
    memcpy(static_cast<char*>(buf) + offset, iovec[i].buf, iovec[i].len);
    offset += iovec[i].len;
  }

  x_error err = this->send(buf, total, flags);

  free(buf);

  return err;
}

X_INL x_error x_socket::setopt(
    const int level, const int option, const void* value, const socklen_t length)
{
  if (value == nullptr) {
    return x_error("posix", EINVAL);
  }

  return setsockopt(this->m_hndl, level, option, (char*)value, length) == 0
    ? x_error() : x_error("socket");
}
// class x_socket}}}
#endif  // X_ENABLE_SOCKET
// IMPL_Communication}}}

//********************************************************** IMPL_Console_IO{{{
#if !X_WINDOWS
X_INL int _kbhit()
{
  static bool initialized{false};
  if (!initialized) {
    struct termios settings{0};
    tcgetattr(STDIN_FILENO, &settings);
    settings.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &settings);
    setbuf(stdin, nullptr);
    initialized = true;
  }

  int byte{0};
  ioctl(STDIN_FILENO, FIONREAD, &byte);

  return byte;
}
#endif

X_INL int x_getch()
{
#if X_WINDOWS
  return _kbhit() ? toupper(_getch()) : 0;
#else
  int key{0};

  int bytes_waiting{_kbhit()};
  if (bytes_waiting <= 0) {
    return 0;
  }

  struct termios old_settings{0};
  struct termios new_settings{0};
  union {
    int in;
    char ch[4];
  } buf{0};
  int ierr{0};
  ssize_t bytes_read{0};

  ierr = tcgetattr(0, &old_settings);
  if (ierr != 0) {
    return 0;
  }

  new_settings = old_settings;
  new_settings.c_lflag &= ~ICANON;
  new_settings.c_lflag &= ~ECHO;

  ierr = tcsetattr(0, TCSANOW, &new_settings);
  if (ierr != 0) {
    tcsetattr(0, TCSANOW, &old_settings);
    return 0;
  }

  bytes_read = read(STDIN_FILENO, &buf.in, bytes_waiting);
  if (bytes_read <= 0) {
    tcsetattr(0, TCSANOW, &old_settings);
    return 0;
  } else if (bytes_read >= 2) {
    if (buf.ch[0] == 0x1B && buf.ch[1] == 0x5B) {
      if (bytes_read == 2) {
        key = X_KEY_ESC;
      } else {
        switch (buf.ch[2]) {
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
    } else {
      key = buf.ch[0];
    }
  } else {
    key = buf.ch[0];
  }

  tcsetattr(0, TCSADRAIN, &old_settings);

  return isalpha(key) ? toupper(key) : key;
#endif
}
// IMPL_Console_IO}}}

//******************************************************* IMPL_Date_and_Time{{{
X_INL void x_sleep(const unsigned long ms)
{
#if X_WINDOWS
  Sleep(ms);
#else
  struct timespec req{0};
  struct timespec rem{0};

  req.tv_sec = ms / 1000;
  req.tv_nsec = static_cast<long>((ms % 1000) * 1000000);

  if (nanosleep(&req, &rem) == EINTR) {
    nanosleep(&rem, nullptr);
  }
#endif
}

X_INL const char* x_timestamp(char* buffer, const size_t size)
{
  if (buffer == nullptr) {
    return "";
  }

  time_t now{time(nullptr)};

#if X_WINDOWS
  if (ctime_s(buffer, size, &now) != 0) {
    return "";
  }
#else
  ctime_r(&now, buffer);
#endif

  buffer[strlen(buffer) - 1] = '\0';

  return buffer;
}

// struct _x_event_stats_{{{
X_INL void x_event_stats::echo(
    const char* unit, const char* title, FILE* const stream)
{
  const char* t = x_strmty(title) ? "STATS" : title;
  std::string msg(128, '\0');

  double scale{1.0};

  if (strcmp(unit, "us") == 0) {
    scale = 1000.0;
  } else if (strcmp(unit, "ms") == 0) {
    scale = 1000000.0;
  } else if (strcmp(unit, "s") == 0) {
    scale = 1000000000.0;
  } else if (strcmp(unit, "m") == 0) {
    scale = 60000000000.0;
  } else if (strcmp(unit, "h") == 0) {
    scale = 3600000000000.0;
  }

  msg = std::string("[") + t + std::string("] ")
    + std::to_string(this->sum / scale) + unit + " in "
    + std::to_string(this->cyc) + " cycles - avg: "
    + std::to_string(this->avg / scale) + unit + ", min("
    + std::to_string(this->min.idx) + "): "
    + std::to_string(this->min.val / scale) + unit + ", max("
    + std::to_string(this->max.idx) + "): "
    + std::to_string(this->max.val / scale) + unit;

  fprintf(stream ? stream : stdout, "%s\n", msg.c_str());
}

X_INL void x_event_stats::reset()
{
  this->ready = false;
  this->cyc = 0;
  this->sum = 0.0;
  this->avg = 0.0;
  this->max.idx = 0;
  this->max.val = DBL_MIN;
  this->min.idx = 0;
  this->min.val = DBL_MAX;
}
// struct _x_event_stats_}}}

// class x_event{{{
X_INL double _x_duration_cpu(
    const struct timespec& start, const struct timespec& stop)
{
  int64_t s{static_cast<int64_t>(stop.tv_sec - start.tv_sec)};
  int64_t ns{static_cast<int64_t>(stop.tv_nsec - start.tv_nsec)};

  return static_cast<double>(s * 1e9 + ns);
}

X_INL struct timespec _x_now_cpu()
{
  struct timespec ts{0};

#if X_WINDOWS || __STDC_VERSION__ >= 201112L
  timespec_get(&ts, TIME_UTC);
#else
  clock_gettime(CLOCK_MONOTONIC, &ts);
#endif

  return ts;
}

X_INL void _x_tic_cpu(struct timespec& ts)
{
#if X_WINDOWS || __STDC_VERSION__ >= 201112L
  timespec_get(&ts, TIME_UTC);
#else
  clock_gettime(CLOCK_MONOTONIC, &ts);
#endif
}

#define _x_toc_cpu _x_tic_cpu

#if X_ENABLE_CU
X_INL double _x_duration_cu(const CUevent start, const CUevent stop)
{
  const char* msg{nullptr};

  CUresult cres = cuEventSynchronize(stop);
  if (cres != CUDA_SUCCESS) {
    cres = cuGetErrorString(cres, &msg);
    if (cres == CUDA_SUCCESS) {
      fprintf(stderr, "cuEventSynchronize: %s\n", msg);
    } else {
      fprintf(stderr, "cuEventSynchronize: unknown error %d\n", cres);
    }
    return -1.0;
  }

  float ms{0.0f};
  cres = cuEventElapsedTime(&ms, start, stop);
  if (cres != CUDA_SUCCESS) {
    cres = cuGetErrorString(cres, &msg);
    if (cres == CUDA_SUCCESS) {
      fprintf(stderr, "cuEventElapsedTime: %s\n", msg);
    } else {
      fprintf(stderr, "cuEventElapsedTime: unknown error %d\n", cres);
    }
    return -1.0;
  }

  return static_cast<double>(ms) * 1000000.0;
}

X_INL CUevent _x_now_cu(const unsigned int flags)
{
  CUevent evt{0};
  const char* msg{nullptr};

  CUresult cres = cuEventCreate(&evt, flags);
  if (cres != CUDA_SUCCESS) {
    cres = cuGetErrorString(cres, &msg);
    if (cres == CUDA_SUCCESS) {
      throw std::runtime_error(std::string("cuEventCreate: ") + msg);
    } else {
      throw std::runtime_error(
          std::string("cuEventCreate: unknown error ") + std::to_string(cres));
    }
  }

  return evt;
}

X_INL void _x_tic_cu(CUevent event, CUstream stream, const unsigned int flags)
{
  const char* msg{nullptr};

  CUresult cres = cuEventRecordWithFlags(event, stream, flags);
  if (cres != CUDA_SUCCESS) {
    cres = cuGetErrorString(cres, &msg);
    if (cres == CUDA_SUCCESS) {
      throw std::runtime_error(std::string("cuEventRecordWithFlags: ") + msg);
    } else {
      throw std::runtime_error(
          std::string("cuEventRecordWithFlags: unknown error ") + std::to_string(cres));
    }
  }
}

#define _x_toc_cu _x_tic_cu
#endif  // X_ENABLE_CU

#if X_ENABLE_CUDA
X_INL double _x_duration_cuda(const cudaEvent_t start, const cudaEvent_t stop)
{
  cudaError_t cerr = cudaEventSynchronize(stop);
  if (cerr != cudaSuccess) {
    fprintf(stderr, "cudaEventSynchronize: %s\n", cudaGetErrorString(cerr));
    return -1.0;
  }

  float ms{0.0f};
  cerr = cudaEventElapsedTime(&ms, start, stop);
  if (cerr != cudaSuccess) {
    fprintf(stderr, "cudaEventElapsedTime: %s\n", cudaGetErrorString(cerr));
    return -1.0;
  }

  return static_cast<double>(ms) * 1000000.0;
}

X_INL cudaEvent_t _x_now_cuda(const unsigned int flags)
{
  CUevent evt{0};

  cudaError_t cerr = cudaEventCreateWithFlags(&evt, flags);
  if (cerr != cudaSuccess) {
    throw std::runtime_error(
        std::string("cudaEventCreateWithFlags: ") + cudaGetErrorString(cerr));
  }

  return evt;
}

X_INL void _x_tic_cuda(
    cudaEvent_t event, cudaStream_t stream, const unsigned int flags)
{
  cudaError_t cerr = cudaEventRecordWithFlags(event, stream, flags);
  if (cerr != cudaSuccess) {
    throw std::runtime_error(
        std::string("cudaEventRecordWithFlags: ") + cudaGetErrorString(cerr));
  }
}

#define _x_toc_cuda _x_tic_cuda
#endif  // X_ENABLE_CUDA

X_INL x_event::x_event(const char* type, const unsigned int flags)
{
  if (strcmp(type, "cpu") == 0) {
    this->m_type = 0;
#if X_ENABLE_CU
  } else if (strcmp(type, "cu") == 0) {
    this->m_type = 1;
    this->m_start.cu = _x_now_cu(flags);
    this->m_stop.cu = _x_now_cu(flags);
#endif
#if X_ENABLE_CUDA
    this->m_type = 2;
  } else if (strcmp(type, "cuda") == 0) {
    this->m_start.cuda = _x_now_cuda(flags);
    this->m_stop.cuda = _x_now_cuda(flags);
#endif
  } else {
    this->m_type = -1;
    throw std::invalid_argument(
        std::string("x_event: unsupported type ") + type);
  }
}

X_INL x_event::~x_event()
{
  switch (this->m_type) {
    case 0:
      break;
#if X_ENABLE_CU
    case 1:
      cuEventDestroy(this->m_start.cu);
      cuEventDestroy(this->m_stop.cu);
      break;
#endif
#if X_ENABLE_CUDA
    case 2:
      cudaEventDestroy(this->m_start.cuda);
      cudaEventDestroy(this->m_stop.cuda);
      break;
#endif
    default:
      break;
  }
}

X_INL double x_event::duration(const char* unit)
{
  double ns{-1.0};

  switch (this->m_type) {
    case 0:
      ns = _x_duration_cpu(this->m_start.cpu, this->m_stop.cpu);
      break;
#if X_ENABLE_CU
    case 1:
      ns = _x_duration_cu(this->m_start.cu, this->m_stop.cu);
      break;
#endif
#if X_ENABLE_CUDA
    case 2:
      ns = _x_duration_cuda(this->m_start.cuda, this->m_stop.cuda);
      break;
#endif
    default:
      throw std::runtime_error("x_event::duration: unsupported call");
  }

  if (strcmp(unit, "ns") == 0) {
    return ns;
  } else if (strcmp(unit, "us") == 0) {
    return ns / 1000.0;
  } else if (strcmp(unit, "ms") == 0) {
    return ns / 1000000.0;
  } else if (strcmp(unit, "s") == 0) {
    return ns / 1000000000.0;
  } else if (strcmp(unit, "m") == 0) {
    return ns / 60000000000.0;
  } else {  // (strcmp(unit, "h") == 0)
    return ns / 3600000000000.0;
  }
}

X_INL void x_event::tic(void* stream, const unsigned int flags)
{
  switch (this->m_type) {
    case 0:
      _x_tic_cpu(this->m_start.cpu);
      break;
#if X_ENABLE_CU
    case 1:
      _x_tic_cu(this->m_start.cu, static_cast<CUstream>(stream), flags);
      break;
#endif
#if X_ENABLE_CUDA
    case 2:
      _x_tic_cuda(this->m_start.cuda, static_cast<cudaStream_t>(stream), flags);
      break;
#endif
    default:
      throw std::runtime_error("x_event::tic: unsupported call");
  }
}

X_INL void x_event::toc(void* stream, const unsigned int flags)
{
  switch (this->m_type) {
    case 0:
      _x_toc_cpu(this->m_stop.cpu);
      break;
#if X_ENABLE_CU
    case 1:
      _x_toc_cu(this->m_stop.cu, static_cast<CUstream>(stream), flags);
      break;
#endif
#if X_ENABLE_CUDA
    case 2:
      _x_toc_cuda(this->m_stop.cuda, static_cast<cudaStream_t>(stream), flags);
      break;
#endif
    default:
      throw std::runtime_error("x_event::toc: unsupported call");
  }
}

X_INL void x_event::toc(
    x_event_stats& stats, const size_t cycle, void* stream, const unsigned int flags)
{
  if (cycle == 0) {
    stats.reset();
    return;
  }

  // NOTE: If the statistics are ready, do not update them.
  if (stats.ready) {
    return;
  }

  // NOTE: Reset the stats before the first cycle.
  if (stats.cyc == 0) {
    stats.reset();
  }

  this->toc(stream, flags);

  double ns = this->duration("ns");

  if (ns > stats.max.val) {
    stats.max.idx = stats.cyc;
    stats.max.val = ns;
  }
  if (ns < stats.min.val) {
    stats.min.idx = stats.cyc;
    stats.min.val = ns;
  }

  stats.sum += ns;
  stats.cyc += 1;
  stats.avg = stats.sum / stats.cyc;

  if (stats.cyc % cycle == 0) {
    stats.ready = true;
  }
}
// class x_event}}}
// IMPL_Date_and_Time}}}

//****************************************************** IMPL_Error_Handling{{{
template<typename... Args>
X_INL void _x_assert_msg(Args&&... args)
{
  if constexpr (sizeof...(args) > 0) {
    fprintf(stderr, "Message: %s\n", std::forward<Args>(args)...);
  }
}

template<typename Func, typename... Args>
X_INL x_error _x_check_impl(const char* category, Func&& func, Args&&... args)
{
  static_assert(
      std::is_same_v<std::invoke_result_t<Func, Args...>, x_error>
      || std::is_convertible_v<std::invoke_result_t<Func, Args...>, int32_t>,
      "Return type of 'func' must be x_error or convertible to int32_t.");

  if constexpr (std::is_same_v<std::invoke_result_t<Func, Args...>, x_error>) {
    return func(std::forward<Args>(args)...);
  } else {
    return x_error(category, static_cast<int32_t>(func(std::forward<Args>(args)...)));
  }
}

X_INL bool x_fail(const x_error& error)
{
  return error;
}

X_INL bool x_succ(const x_error& error)
{
  return !error;
}

// class x_error{{{
X_INL x_error::x_error()
{
}

X_INL x_error::x_error(const char* category)
{
  this->set(category);
}

X_INL x_error::x_error(
    const char* category, const int32_t value, bool (*fail)(const int32_t))
{
  this->set(category, value, fail);
}

X_INL x_error::x_error(
    const char* category, const int32_t value, const char* message,
    bool (*fail)(const int32_t))
{
  this->set(category, value, message, fail);
}

X_INL x_error::~x_error()
{
}

X_INL x_error& x_error::set(const char* category)
{
  this->m_cat = category;

  if (strcmp(category, "posix") == 0) {
    this->m_val = static_cast<int32_t>(errno);
#if X_WINDOWS
  } else if (strcmp(category, "win32") == 0) {
    this->m_val = static_cast<int32_t>(GetLastError());
  } else if (strcmp(category, "socket") == 0) {
    this->m_val = static_cast<int32_t>(WSAGetLastError());
#endif
#if X_ENABLE_CUDA
  } else if (strcmp(category, "cuda") == 0) {
    this->m_val = static_cast<int32_t>(cudaGetLastError());
#endif
  } else {
    throw std::invalid_argument(
        std::string("x_error: unsupported error category ") + category);
  }

  return *this;
}

X_INL x_error& x_error::set(
    const char* category, const int32_t value, bool (*fail)(const int32_t))
{
  if (strcmp(category, "custom") == 0 && this->m_fail == nullptr && fail == nullptr) {
    throw std::invalid_argument(
        "x_error: a failure predicate is required for a custom error");
  }

  this->m_cat = category;
  this->m_val = value;
  this->m_msg.clear();
  this->m_fail = fail;

  return *this;
}

X_INL x_error& x_error::set(
    const char* category, const int32_t value, const char* message,
    bool (*fail)(const int32_t))
{
  if (strcmp(category, "custom") == 0 && this->m_fail == nullptr && fail == nullptr) {
    throw std::invalid_argument(
        "x_error: a failure predicate is required for a custom error");
  }

  this->m_cat = category;
  this->m_val = value;
  this->m_msg = message;
  this->m_fail = fail;

  return *this;
}

X_INL const char* x_error::cat()
{
  return this->m_cat;
}

X_INL const char* x_error::msg()
{
#if X_WINDOWS
  if (strcmp(this->m_cat, "posix") == 0) {
    if (this->m_msg.empty()) {
      this->m_msg.resize(64);
    }
    strerror_s(
        this->m_msg.data(), this->m_msg.size(), static_cast<int>(this->m_val));
  } else if (strcmp(this->m_cat, "win32") == 0 || strcmp(this->m_cat, "socket") == 0) {
    if (this->m_msg.empty()) {
      this->m_msg.resize(128);
    }
    FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM
        | FORMAT_MESSAGE_IGNORE_INSERTS
        | FORMAT_MESSAGE_MAX_WIDTH_MASK,
        nullptr, static_cast<DWORD>(this->m_val),
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        this->m_msg.data(), static_cast<DWORD>(this->m_msg.size()), nullptr);
  }
#else
  if (strcmp(this->m_cat, "posix") == 0 || strcmp(this->m_cat, "socket") == 0) {
    this->m_msg = strerror(static_cast<int>(this->m_val));
  }
#endif
#if X_ENABLE_CU
  else if (strcmp(this->m_cat, "cu") == 0) {
    const char* msg{nullptr};
    CUresult cres = cuGetErrorString(static_cast<CUresult>(this->m_val), &msg);
    if (cres == CUDA_SUCCESS) {
      this->m_msg = msg;
    } else {
      this->m_msg = std::string("Unknown CUDA driver error ") + std::to_string(cres);
    }
  }
#endif
#if X_ENABLE_CUDA
  else if (strcmp(this->m_cat, "cuda") == 0) {
    this->m_msg = cudaGetErrorString(static_cast<cudaError_t>(this->m_val));
  }
#endif
  else {
    if (this->m_msg.empty()) {
      this->m_msg = std::string("Custom error ") + std::to_string(this->m_val);
    }
  }

  return this->m_msg.c_str();
}

X_INL int32_t x_error::val() const
{
  return this->m_val;
}

X_INL x_error::operator bool() const
{
  if (this->m_fail) {
    // NOTE: Covers the "custom" and other customized cases.
    return this->m_fail(this->m_val);
  } else {
    if (strcmp(this->m_cat, "socket") == 0) {
      return this->m_val != 0;
    }
#if X_WINDOWS
    else if (strcmp(this->m_cat, "win32") == 0) {
      return this->m_val != 0;
    }
#endif
#if X_ENABLE_CU
    else if (strcmp(this->m_cat, "cu") == 0) {
      return static_cast<CUresult>(this->m_val) != CUDA_SUCCESS;
    }
#endif
#if X_ENABLE_CUDA
    else if (strcmp(this->m_cat, "cuda") == 0) {
      return static_cast<cudaError_t>(this->m_val) != cudaSuccess;
    }
#endif
    else {
      // NOTE: Covers the "posix" case.
      return this->m_val != 0;
    }
  }
}
// class x_error}}}
// IMPL_Error_Handling}}}

//********************************************************* IMPL_File_System{{{
X_INL bool x_fexist(const char* file)
{
  int ierr{0};

#if X_WINDOWS
  struct _stat64 s{0};
  ierr = _stat64(file, &s);
#else
  struct stat s{0};
  ierr = stat(file, &s);
#endif

  return ierr == 0;
}

X_INL x_error x_fopen(FILE** stream, const char* file, const char* mode)
{
#if X_WINDOWS
  errno_t ierr = fopen_s(stream, file, mode);
  if (ierr != 0) {
    return x_error("posix", ierr);
  }
#else
  *stream = fopen(file, mode);
  if (*stream == nullptr) {
    return x_error("posix");
  }
#endif

  return x_error();
}

X_INL const char* x_fpath(char* dst, const char* src)
{
#if X_WINDOWS
  return dst != nullptr ? _fullpath(dst, src, X_PATH_MAX) : nullptr;
#else
  return dst != nullptr ? realpath(src, dst) : nullptr;
#endif
}

X_INL int64_t x_fsize(const char* file)
{
  int ierr{0};

#if X_WINDOWS
  struct _stat64 s{0};
  ierr = _stat64(file, &s);
#else
  struct stat s{0};
  ierr = stat(file, &s);
#endif

  return ierr == 0 ? static_cast<int64_t>(s.st_size) : -1;
}

X_INL x_error x_split_path(
    const char *path,
    char *root, const size_t rsz, char *dir, const size_t dsz,
    char *file, const size_t fsz, char *ext, const size_t esz)
{
  char full[X_PATH_MAX]{0};
  x_fpath(full, path);

  if (!x_fexist(full)) {
    return x_error("posix", ENOENT);
  }

#if X_WINDOWS
  return x_error(
      "posix", _splitpath_s(full, root, rsz, dir, dsz, file, fsz, ext, esz));
#else
  if (root == nullptr || rsz == 0 || dir == nullptr || dsz == 0
      || file == nullptr || fsz == 0 || ext == nullptr || esz == 0) {
    return x_error("posix", EINVAL);
  }

  if (root != nullptr) { root[0] = '\0'; }
  if (dir != nullptr) { dir[0] = '\0'; }
  if (file != nullptr) { file[0] = '\0'; }
  if (ext != nullptr) { ext[0] = '\0'; }

  size_t psz{strlen(full)};
  size_t sz{0};
  char* begin{nullptr};
  char* end{nullptr};

  // root
  begin = strchr((char*)path, '/');
  if (begin == nullptr) {
    return x_error("posix", ENOENT);
  }

  end = strchr(begin + 1, '/');
  if (end == nullptr) {
    end = full + psz;
  }

  if (root != nullptr) {
    sz = end - begin;
    if (sz >= rsz) {
      return x_error("posix", ENOBUFS);
    }

    memcpy(root, begin, sz);
    root[sz] = '\0';

    if (end == nullptr) {
      return x_error();
    }
  }

  // dir
  begin = strchr(end, '/');
  if (begin == nullptr) {
    return x_error();
  }

  end = strrchr((char*)path, '/');
  if (end <= begin) {
    return x_error();
  }
  if (end == nullptr) {
    end = full + psz;
  }

  if (dir != nullptr) {
    sz = end - begin;
    if (sz >= dsz) {
      return x_error("posix", ENOBUFS);
    }

    memcpy(dir, begin, sz);
    dir[sz] = '\0';

    if (end == nullptr) {
      return x_error();
    }
  }

  // file
  begin = end + 1;
  if ((begin - full) >= 0) {
    return x_error();
  }

  end = strrchr((char*)path, '.');
  if (end <= begin) {
    return x_error();
  }
  if (end == nullptr) {
    end = full + psz;
  }

  if (file != nullptr) {
    sz = end - begin;
    if (sz >= fsz) {
      return x_error("posix", ENOBUFS);
    }

    memcpy(file, begin, sz);
    file[sz] = '\0';
  }

  // ext
  if (ext != nullptr) {
    begin = end;
    end = full + psz;
    if (end <= begin) {
      return x_error();
    }

    sz = end - begin;
    memcpy(ext, begin, sz);
    ext[sz] = '\0';
  }

  return x_error();
#endif
}
// IMPL_File_System}}}

//************************************************************ IMPL_Hardware{{{
X_INL size_t x_ncpu()
{
#if X_WINDOWS
  SYSTEM_INFO info{{0}};
  GetSystemInfo(&info);
  return static_cast<size_t>(info.dwNumberOfProcessors);
#else
  return static_cast<size_t>(sysconf(_SC_NPROCESSORS_ONLN));
#endif
}

X_INL size_t x_ngpu(const char* api)
{
  if (x_strmty(api)) {
    return 0;
  }

  if (strcmp(api, "cu") == 0) {
#if X_ENABLE_CU
    int count{0};
    CUresult cres = cuDeviceGetCount(&count);
    if (cres != CUDA_SUCCESS) {
      const char* msg{nullptr};
      cres = cuGetErrorString(cres, &msg);
      if (cres == CUDA_SUCCESS) {
        fprintf(stderr, "cuDeviceGetCount: %s\n", msg);
      } else {
        fprintf(stderr, "cuDeviceGetCount: unknown error %d\n", cres);
      }
      return 0;
    }

    return static_cast<size_t>(count);
#else
    return 0;
#endif
  } else if (strcmp(api, "cuda") == 0) {
#if X_ENABLE_CUDA
    int count{0};
    cudaError_t cerr = cudaGetDeviceCount(&count);
    if (cerr != cudaSuccess) {
      fprintf(stderr, "cudaGetDeviceCount: %s\n", cudaGetErrorString(cerr));
      return 0;
    }

    return static_cast<size_t>(count);
#else
    return 0;
#endif
  }

  return 0;
}
// IMPL_Hardware}}}

//********************************************************* IMPL_Mathematics{{{
template<typename T>
X_INL constexpr T x_KiB(const T n)
{
  return n * static_cast<T>(1024);
}

template<typename T>
X_INL constexpr T x_MiB(const T n)
{
  return n * static_cast<T>(1048576);
}

template<typename T>
X_INL constexpr T x_GiB(const T n)
{
  return n * static_cast<T>(1073741824);
}

template<typename T>
X_INL constexpr T x_TiB(const T n)
{
  return n * static_cast<T>(1099511627776);
}

template<typename T>
X_INL constexpr T x_PiB(const T n)
{
  return n * static_cast<T>(1125899906842620);
}

template<typename T>
X_INL constexpr T x_bit(const T n)
{
  static_assert(std::is_integral_v<T>, "x_bit: an integral type is required");

  return static_cast<T>(1) << n;
}

template<typename T>
X_INL constexpr T x_gcd(const T m, const T n)
{
  static_assert(std::is_integral_v<T>, "x_gcd: an integral type is required");

  return n == 0 ? m : x_gcd(n, m % n);
}

template<typename T>
X_INL constexpr T x_lcm(const T m, const T n)
{
  static_assert(std::is_integral_v<T>, "x_lcm: an integral type is required");

  if (m == 0 || n == 0) {
    return 0;
  }

  return m / x_gcd(m, n) * n;
}

template<typename T>
X_INL constexpr T x_next_exp(const T base, const T src)
{
  static_assert(std::is_integral_v<T>, "x_next_exp: an integral type is required");

  if (src == 0) {
    return 1;
  }

  if (base == 2) {
    if ((src & (src - 1)) == 0) {
      return src;
    }

    T s{src};
    T count{0};
    while (s != 0) {
      s >>= 1;
      count += 1;
    }

    return 1 << count;
  } else {
    double exp =
      std::log(static_cast<double>(src)) / std::log(static_cast<double>(base));

    if (exp == std::round(exp)) {
      return src;
    }

    return std::pow(base, static_cast<int>(std::ceil(exp)));
  }
}

template<typename T>
X_INL constexpr T x_next_mul(const T base, const T src)
{
  static_assert(std::is_integral_v<T>, "x_next_mul: an integral type is required");

  return (src / base + 1) * base;
}

template<typename T>
X_INL constexpr T x_prev_exp(const T base, const T src)
{
  static_assert(std::is_integral_v<T>, "x_prev_exp: an integral type is required");

  if (src == 0) {
    return 0;
  }

  if (base == 2) {
    if ((src & (src - 1)) == 0) {
      return src;
    }

    T s{src};
    T count{0};
    while (s != 0) {
      s >>= 1;
      count += 1;
    }

    return 1 << (count - 1);
  } else {
    double exp =
      std::log(static_cast<double>(src)) / std::log(static_cast<double>(base));

    if (exp == std::round(exp)) {
      return src;
    }

    return std::pow(base, static_cast<int>(std::floor(exp)));
  }
}

template<typename T>
X_INL constexpr T x_prev_mul(const T base, const T src)
{
  static_assert(std::is_integral_v<T>, "x_prev_mul: an integral type is required");

  return (src / base) * base;
}
// IMPL_Mathematics}}}

//*************************************************** IMPL_Memory_Management{{{
template<typename T, size_t N>
X_INL constexpr size_t x_count(const T (&array)[N])
{
  return N;
}

template<bool array, typename T>
X_INL void x_delete(T*& ptr)
{
  if (ptr != nullptr) {
    if constexpr (array) {
      delete[] ptr;
    } else {
      delete ptr;
    }
    ptr = nullptr;
  }
}

template<typename T>
X_INL void x_free(T*& ptr)
{
  if (ptr != nullptr) {
    free(ptr);
    ptr = nullptr;
  }
}

template<typename T>
X_INL x_error x_malloc(T** ptr, const size_t size)
{
  if (*ptr != nullptr) {
    return x_error("posix", EINVAL);
  }

  *ptr = static_cast<T*>(malloc(size));
  if (*ptr == nullptr) {
    return x_error("posix");
  }

  return x_error();
}

X_INL x_error x_memcpy(void* dst, const void* src, const size_t size)
{
  if (dst == nullptr || src == nullptr) {
    return x_error("posix", EINVAL);
  }

  if (size != 0) {
    memcpy(dst, src, size);
  }

  return x_error();
}

X_INL x_error _x_meminfo_cpu(size_t* avail, size_t* total)
{
  if (avail == nullptr && total == nullptr) {
    return x_error("posix", EINVAL);
  }

#if X_WINDOWS
  MEMORYSTATUSEX status{0};
  status.dwLength = sizeof(status);

  if (!GlobalMemoryStatusEx(&status)) {
    return x_error("win32");
  }

  if (avail != nullptr) {
    *avail = static_cast<size_t>(status.ullAvailPhys);
  }
  if (total != nullptr) {
    *total = static_cast<size_t>(status.ullTotalPhys);
  }
#else
  struct sysinfo info{0};
  if (sysinfo(&info) != 0) {
    return x_error("posix");
  }

  if (avail != nullptr) {
    *avail = static_cast<size_t>(info.freeram);
  }
  if (total != nullptr) {
    *total = static_cast<size_t>(info.totalram);
  }
#endif

  return x_error();
}

#if X_ENABLE_CU
X_INL x_error _x_meminfo_cu(size_t* avail, size_t* total)
{
  if (avail == nullptr && total == nullptr) {
    return x_error("posix", EINVAL);
  }

  CUresult cres = cuMemGetInfo(avail, total);
  if (cres != CUDA_SUCCESS) {
    return x_error("cu", cres);
  }

  return x_error();
}
#endif

#if X_ENABLE_CUDA
X_INL x_error _x_meminfo_cuda(size_t* avail, size_t* total)
{
  if (avail == nullptr && total == nullptr) {
    return x_error("posix", EINVAL);
  }

  cudaError_t cerr = cudaMemGetInfo(avail, total);
  if (cerr != cudaSuccess) {
    return x_error("cuda", cerr);
  }

  return x_error();
}
#endif

X_INL x_error x_meminfo(const char* type, size_t* avail, size_t* total)
{
  if (strcmp(type, "cpu") == 0) {
    return _x_meminfo_cpu(avail, total);
#if X_ENABLE_CU
  } else if (strcmp(type, "cu") == 0) {
    return _x_meminfo_cu(avail, total);
#endif
#if X_ENABLE_CUDA
  } else if (strcmp(type, "cuda") == 0) {
    return _x_meminfo_cuda(avail, total);
#endif
  } else {
    return x_error("posix", ENOTSUP, "unspported type");
  }
}

#if X_ENABLE_CU
X_INL const char* _x_memtype_cu(const CUdeviceptr ptr)
{
  static const char* type[] = {"Unknown", "Host", "Device", "Array", "Unified"};

  CUmemorytype attr;
  CUresult cres = cuPointerGetAttribute(&attr, CU_POINTER_ATTRIBUTE_MEMORY_TYPE, ptr);
  if (cres != CUDA_SUCCESS) {
    const char* msg{nullptr};
    cres = cuGetErrorString(cres, &msg);
    if (cres == CUDA_SUCCESS) {
      fprintf(stderr, "cuPointerGetAttribute: %s\n", msg);
    } else {
      fprintf(stderr, "cuPointerGetAttribute: unknown error %d\n", cres);
    }
    return "Unknown";
  }

  if (attr >= CU_MEMORYTYPE_HOST && attr <= CU_MEMORYTYPE_UNIFIED) {
    return type[attr];
  }

  return "Unknown";
}
#endif  // X_ENABLE_CU

#if X_ENABLE_CUDA
X_INL const char* _x_memtype_cuda(const void* ptr)
{
  static const char* type[] = {"Unregistered", "Host", "Device", "Managed"};

  cudaPointerAttributes attr;
  cudaError_t cerr = cudaPointerGetAttributes(&attr, ptr);
  if (cerr != cudaSuccess) {
    fprintf(stderr, "cudaPointerGetAttributes: %s\n", cudaGetErrorString(cerr));
    return "Unknown";
  }

  if (attr.type >= cudaMemoryTypeUnregistered && attr.type <= cudaMemoryTypeManaged) {
    return type[attr.type];
  }

  return "Unknown";
}
#endif  // X_ENABLE_CUDA

template<typename T>
X_INL const char* x_memtype(const char* type, const T ptr)
{
#if X_ENABLE_CU
  if constexpr (std::is_same_v<T, CUdeviceptr>) {
    if (strcmp(type, "cu") == 0) {
      return _x_memtype_cu(ptr);
    }
  }
#endif

#if X_ENABLE_CUDA
  if constexpr (std::is_pointer_v<T>) {
    if (strcmp(type, "cuda") == 0) {
      return _x_memtype_cuda(ptr);
    }
  }
#endif

  return "N/A";
}

// IMPL_Memory_Management}}}

//********************************************************* IMPL_Standard_IO{{{
// x_log{{{
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

template<typename T>
X_INL std::string _x_log_to_string(T src)
{
  if constexpr (std::is_same_v<T, std::string>) {
    return src;
  } else if constexpr (std::is_same_v<T, const char*> || std::is_same_v<T, char*>) {
    return std::string(src);
  } else if constexpr (std::is_pointer_v<T>) {
    char buf[17]{0};
    snprintf(buf, sizeof(buf), "%p", src);
    return std::string(buf);
  } else {
    return std::to_string(src);
  }
}

template<typename... Args>
X_INL const char* x_fmt(
    char* buffer, const size_t size, const char* format, Args... args)
{
  if (buffer == nullptr || size == 0) {
    return format;
  }

  if constexpr (sizeof...(args) == 0) {
    memcpy(buffer, format, size < strlen(format) ? size : strlen(format));
  } else {
#if (__cplusplus >= 202002L && (X_CLANG >= x_version(17, 0, 0) || X_GCC >= x_version(13, 0, 0) || X_MSVC >= x_version(19, 29, 0)))
    std::string fmsg = std::vformat(format, std::make_format_args(args...));
    memcpy(buffer, fmsg.c_str(), size < fmsg.size() ? size : fmsg.size());
#else
    std::string arg[]{_x_log_to_string(std::forward<Args>(args))...};
    std::string fmt{format};
    size_t index{0};
    size_t offset{0};
    size_t begin{0};
    size_t end{0};

    while ((end = fmt.find("{}", begin)) != std::string::npos && offset < size) {
      memcpy(buffer + offset, format + begin, end - begin);
      offset += end - begin;

      memcpy(buffer + offset, arg[index].c_str(), arg[index].size());
      offset += arg[index].size();

      begin = end + 2;
      index += 1;
    }

    memcpy(buffer + offset, format + begin, fmt.size() - begin);
#endif
  }

  return buffer;
}

template<char level>
X_INL void _x_log_prefix(
    char* buf, const size_t bsz,
    const char* filename, const char* function, const long long line)
{
  char timestamp[26]{0};

#ifdef NDEBUG
  snprintf(buf, bsz, "[%c %s] ", toupper(level), x_timestamp(timestamp, 26));
#else
  snprintf(
      buf, bsz, "[%c %s | %s - %s - %lld] ",
      toupper(level), x_timestamp(timestamp, 26), filename, function, line);
#endif
}

template<char level, typename... Args>
X_INL void _x_log_impl(
    const char* filename, const char* function, const long long line,
    FILE* stream, const char* format, Args&&... args)
{
  char color_level[8]{0};
  char color_reset[8]{0};

  if constexpr (level == 'p' || level == 'P') {
#if X_LOG_LEVEL >= X_LOG_PLAIN
    snprintf(color_level, 8, _X_LOG_COLOR_P);
#else
    return;
#endif
  } else if constexpr (level == 'f' || level == 'F') {
#if X_LOG_LEVEL >= X_LOG_FATAL
    snprintf(color_level, 8, _X_LOG_COLOR_F);
#else
    return;
#endif
  } else if constexpr (level == 'e' || level == 'E') {
#if X_LOG_LEVEL >= X_LOG_ERROR
    snprintf(color_level, 8, _X_LOG_COLOR_E);
#else
    return;
#endif
  } else if constexpr (level == 'w' || level == 'W') {
#if X_LOG_LEVEL >= X_LOG_WARNING
    snprintf(color_level, 8, _X_LOG_COLOR_W);
#else
    return;
#endif
  } else if constexpr (level == 'i' || level == 'I') {
#if X_LOG_LEVEL >= X_LOG_INFO
    snprintf(color_level, 8, _X_LOG_COLOR_I);
#else
    return;
#endif
  } else if constexpr (level == 'd' || level == 'D') {
#if X_LOG_LEVEL >= X_LOG_DEBUG
    snprintf(color_level, 8, _X_LOG_COLOR_D);
#else
    return;
#endif
  } else {
    return;
  }

  snprintf(color_reset, 8, _X_COLOR_RESET);

  char prefix[X_LOG_PREFIX_LIMIT]{0};
  _x_log_prefix<level>(prefix, X_LOG_PREFIX_LIMIT, filename, function, line);

  const char* msg{format};

  if constexpr (sizeof...(args) > 0) {
    char buf[X_LOG_MSG_LIMIT]{0};
    snprintf(buf, X_LOG_MSG_LIMIT, format, std::forward<Args>(args)...);
    msg = buf;
  }

  if (stream == nullptr || stream == stdout || stream == stderr) {
    fprintf(
        stream == nullptr ? stdout : stream,
        "%s%s%s%s\n", color_level, prefix, msg, color_reset);
  } else {
    fprintf(stream, "%s%s\n", prefix, msg);
  }
}
// x_log}}}
// IMPL_Standard_IO}}}

//************************************************************** IMPL_String{{{
X_INL x_error x_strcpy(char* dst, const size_t size, const char* src)
{
  if (dst == nullptr || size == 0) {
    return x_error("posix", EINVAL);
  }

  size_t cpy_sz{size - 1};
  size_t src_sz{strlen(src)};

  if (src_sz > 0) {
    cpy_sz = cpy_sz < src_sz ? cpy_sz : src_sz;

    x_error err = x_memcpy(dst, src, cpy_sz);
    if (err) {
      return err;
    }
  }

  dst[cpy_sz] = '\0';

  return x_error();
}

X_INL bool x_strmty(const char* string)
{
  return string == nullptr || string[0] == '\0';
}
// IMPL_String}}}


#endif  // X_H
