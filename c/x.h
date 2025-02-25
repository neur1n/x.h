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


Last update: 2025-02-25 20:36
Version: v0.8.3
******************************************************************************/
#ifndef X_H
#define X_H x_version(0, 8, 3)


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

#include <assert.h>
#include <errno.h>
#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

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

#include <ctype.h>
#include <limits.h>
#include <stdlib.h>

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

#ifdef __cplusplus
#define X_INL inline
#else
#define X_INL static inline
#endif

#define _x_arg_n( \
      _1,   _2,   _3,   _4,   _5,   _6,   _7,   _8, \
      _9,  _10,  _11,  _12,  _13,  _14,  _15,  _16, \
     _17,  _18,  _19,  _20,  _21,  _22,  _23,  _24, \
     _25,  _26,  _27,  _28,  _29,  _30,  _31,  _32, \
     _33,  _34,  _35,  _36,  _37,  _38,  _39,  _40, \
     _41,  _42,  _43,  _44,  _45,  _46,  _47,  _48, \
     _49,  _50,  _51,  _52,  _53,  _54,  _55,  _56, \
     _57,  _58,  _59,  _60,  _61,  _62,  _63,  _64, \
     _65,  _66,  _67,  _68,  _69,  _70,  _71,  _72, \
     _73,  _74,  _75,  _76,  _77,  _78,  _79,  _80, \
     _81,  _82,  _83,  _84,  _85,  _86,  _87,  _88, \
     _89,  _90,  _91,  _92,  _93,  _94,  _95,  _96, \
     _97,  _98,  _99, _100, _101, _102, _103, _104, \
    _105, _106, _107, _108, _109, _110, _111, _112, \
    _113, _114, _115, _116, _117, _118, _119, _120, \
    _121, _122, _123, _124, _125, _126, _127,    N, ...) N

#define _x_seq_n() \
  127, 126, 125, 124, 123, 122, 121, 120, \
  119, 118, 117, 116, 115, 114, 113, 112, \
  111, 110, 109, 108, 107, 106, 105, 104, \
  103, 102, 101, 100,  99,  98,  97,  96, \
   95,  94,  93,  92,  91,  90,  89,  88, \
   87,  86,  85,  84,  83,  82,  81,  80, \
   79,  78,  77,  76,  75,  74,  73,  72, \
   71,  70,  69,  68,  67,  66,  65,  64, \
   63,  62,  61,  60,  59,  58,  57,  56, \
   55,  54,  53,  52,  51,  50,  49,  48, \
   47,  46,  45,  44,  43,  42,  41,  40, \
   39,  38,  37,  36,  35,  34,  33,  32, \
   31,  30,  29,  28,  27,  26,  25,  24, \
   23,  22,  21,  20,  19,  18,  17,  16, \
   15,  14,  13,  12,  11,  10,   9,   8, \
    7,   6,   5,   4,   3,   2,   1,   0

#define _x_narg(...) _x_arg_n(__VA_ARGS__)

#define x_narg(...) _x_narg(__VA_ARGS__, _x_seq_n())
/** @} */  // Miscellaneous

#ifdef __cplusplus
extern "C" {
#endif
// Forward Declarations{{{
typedef struct _x_error_
{
  const char* cat;
  int32_t val;
  bool (*fail)(const int32_t);
} x_error;

X_INL x_error _x_error_cat(const char* cat);
X_INL x_error _x_error_cat_val(const char* cat, ... /*const int32_t val*/);

#define x_error_set(cat,  ...) \
  x_narg(cat, ##__VA_ARGS__) == 1 ? _x_error_cat(cat) \
  : _x_error_cat_val(cat, ##__VA_ARGS__)
// Forward Declarations}}}

/******************************************************************************
 * @name Communication
 * @brief A collection of communication utilities.
 * @{
 *****************************************************************************/
/// @brief Calculate the CRC32 checksum of a data chunk.
X_INL uint32_t x_checksum_crc32(
    const void* data, const size_t size, const uint32_t* prev);

/// @brief Calculate the internet checksum of a data chunk.
/// @see [RFC1071](https://www.rfc-editor.org/info/rfc1071)
X_INL uint16_t x_checksum_rfc1071(
    const void* data, const size_t size, const uint16_t* prev);

/// @brief Calculate the XOR checksum of a data chunk.
X_INL uint8_t x_checksum_xor(
    const void* data, const size_t size, const uint8_t* prev);

/// @brief The default start of frame for a packet.
#ifndef X_PACKET_SOF
#define X_PACKET_SOF (0x55AA)
#endif

/// @brief A macro to represent an infinite number of packets.
#ifndef X_PACKET_INF
#define X_PACKET_INF UINT64_MAX
#endif

/// @struct x_heaher
/// @brief The header of a packet.
/// @var x_heaher::sof
///      The start of frame.
/// @var x_heaher::ctl
///      The control code.
/// @var x_heaher::opt
///      The option, just use it freely.
/// @var x_heaher::cnt
///      The total number of packets.
/// @var x_heaher::idx
///      The index of the current packet.
/// @var x_heaher::dsz
///      The size of the data chunk.
/// @var x_heaher::cks
///      The checksum of the packet, which is calculated based on the header
///      and the data chunk.
typedef struct _x_heaher_
{
  uint16_t sof;
  uint16_t ctl;
  uint32_t opt;
  uint64_t cnt;
  uint64_t idx;
  uint64_t dsz;
  uint64_t cks;
} x_heaher;

/// @brief The default initializer for @ref x_heaher.
#define X_HDR_INIT {X_PACKET_SOF, 0, 0, X_PACKET_INF, 0, 0, 0}

/// @struct x_packet
/// @brief A message packet.
/// @var x_packet::head
///      The header of the packet.
/// @var x_packet::body
///      The data chunk of the packet.
typedef struct _x_packet_
{
  x_heaher head;
  void* body;
} x_packet;

/// @brief The default initializer for @ref x_packet.
#define X_PACKET_INIT {X_HDR_INIT, NULL}

/// @struct x_iovec
/// @brief An I/O vector.
/// @var x_iovec::buf
///      The buffer.
/// @var x_iovec::len
///      The length of the buffer.
typedef struct _x_iovec_
{
  void* buf;
  size_t len;
} x_iovec;

/// @brief The default initializer for @ref x_iovec.
#define X_IOVEC_INIT {NULL, 0}

#if X_ENABLE_SOCKET
/// @brief A class wrapping the socket operations.
typedef struct _x_socket_ x_socket;
struct _x_socket_
{
  /// @brief Accept a connection from a client.
  /// @param self The self instance.
  /// @param client The client to be accepted.
  /// @return An instance of @ref x_error.
  x_error (*accept)(x_socket* self, x_socket* client);

  /// @brief Query the IP address and port of the socket.
  /// @param self The self instance.
  /// @param ip The buffer to store the IP address.
  /// @param port The buffer to store the port.
  /// @return An instance of @ref x_error.
  x_error (*addr)(x_socket* self, char* ip, uint16_t* port);

  /// @brief Close the socket.
  /// @param self The self instance.
  /// @return An instance of @ref x_error.
  x_error (*close)(x_socket* self);

  /// @brief Connect to a server with specified IP address and port.
  /// @param self The self instance.
  /// @param ip The IP address of the server.
  /// @param port The port of the server.
  /// @return An instance of @ref x_error.
  x_error (*connect)(x_socket* self, const char* ip, const uint16_t port);

  /// @brief Wrapper of `getsockopt` with error handling.
  /// @param self The self instance.
  /// @see getsockopt
  /// @return An instance of @ref x_error.
  x_error (*getopt)(
      x_socket* self, const int lvl, const int opt, void* val, socklen_t* len);

  /// @brief Listen on a specified IP address and port.
  /// @param self The self instance.
  /// @param ip The IP address to listen on.
  /// @param port The port to listen on.
  /// @return An instance of @ref x_error.
  x_error (*listen)(x_socket* self, const char* ip, const uint16_t port);

  /// @brief Wrapper of `recv` with error handling.
  /// @param self The self instance.
  /// @return An instance of @ref x_error.
  /// @see recv
  /// @remark Different from the standard `recv`, this function trys to receive
  ///         the specified size of data before returning.
  x_error (*recv)(
      x_socket* self, void* buf, const size_t size, const int flags);

  /// @brief Vectored version of `recv`.
  /// @param self The self instance.
  /// @return An instance of @ref x_error.
  /// @see @ref x_socket::recv
  x_error (*recvv)(
      x_socket* self, x_iovec* iov, const size_t count, const int flags);

  /// @brief Wrapper of `send` with error handling.
  /// @param self The self instance.
  /// @return An instance of @ref x_error.
  /// @see send
  /// @remark Different from the standard `send`, this function trys to send
  ///         the specified size of data before returning.
  x_error (*send)(
      x_socket* self, const void* buf, const size_t size, const int flags);

  /// @brief Vectored version of `send`.
  /// @param self The self instance.
  /// @return An instance of @ref x_error.
  /// @see @ref x_socket::send
  x_error (*sendv)(
      x_socket* self, const x_iovec* iov, const size_t count, const int flags);

  /// @brief Wrapper of `setsockopt` with error handling.
  /// @param self The self instance.
  /// @see setsockopt
  /// @return An instance of @ref x_error.
  x_error (*setopt)(
      x_socket* self, const int lvl, const int opt, const void* val, const socklen_t len);

#if X_WINDOWS
  SOCKET m_hndl;
#else
  int m_hndl;
#endif
  struct sockaddr m_addr;
  int m_domain;
};

/// @brief Initialize a @ref x_socket instance. A wrapper for `socket`.
/// @param self The instance to be initialized.
/// @return An instance of @ref x_error.
X_INL x_error x_socket_init(
    x_socket* self, const int domain, const int type, const int protocol);

/// @brief Terminate a @ref x_socket instance.
/// @param self The instance to be terminated.
/// @return An instance of @ref x_error.
X_INL x_error x_socket_term(x_socket* self);
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
/// @param buf The buffer to store the timestamp.
/// @param bsz The size of the buffer.
/// @return The current timestamp, same as `buf`.
/// @remark This function calls `ctime_s` on Windows and `ctime_r` on Linux
///         internally and uses their return values to fill the buffer.
///         Therefore, the format is not customizable. A buffer with at least
///         26 bytes is guaranteed to store the timestamp.
X_INL const char* x_timestamp(char* buf, const size_t bsz);

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
typedef struct _x_event_stats_ x_event_stats;
struct _x_event_stats_
{
  /// @brief Echo the statistics in a predefined format.
  /// @param unit The unit of the statistics, "h", "m", "s", "ms", "us", or "ns".
  /// @param title The title for the statistics. Default is "STATS". If the
  ///              title is not a valid string, "STATS" will be used.
  /// @param stream The output stream. Default is `stdout`.
  /// @remark This helper function provides a handy way to print the
  ///         statistics. Users may use their own format to print the
  ///         statistics with the data members of this structure.
  void (*echo)(
      x_event_stats* self, const char* unit, const char* title, FILE* const stream);

  /// @brief Reset the statistics.
  void (*reset)(x_event_stats* self);

  bool ready;
  size_t cyc;
  double sum;
  double avg;
  struct
  {
    size_t idx;
    double val;
  } max, min;
};

X_INL void x_event_stats_init(x_event_stats* self);

typedef struct _x_event_ x_event;
struct _x_event_
{
  /// @brief Calculate the duration between two events.
  /// @param unit The unit of the duration, "h", "m", "s", "ms", "us", or "ns".
  /// @return The elapsed time.
  /// @attention Before calling this function, @ref x_event::tic and
  ///            @ref x_event::toc must be called.
  double (*duration)(x_event* self, const char* unit);

  /// @brief Start the recording.
  /// @param stream The optional stream to perform the recording. It is only
  ///               used when the CUDA driver or runtime API is enabled.
  /// @param flags The optional flags to perform the recording. It is only used
  ///              when the CUDA driver or runtime API is enabled.
  void (*tic)(x_event* self, void* stream, const unsigned int flags);

  /// @brief Stop the recording.
  /// @param stream The optional stream to perform the recording. It is only
  ///               used when the CUDA driver or runtime API is enabled.
  /// @param flags The optional flags to perform the recording. It is only used
  ///              when the CUDA driver or runtime API is enabled.
  void (*toc)(x_event* self, void* stream, const unsigned int flags);

  /// @brief Stop the event recording and return the statistics.
  /// @param stats The statistics of the event.
  /// @param cycle The number of cycles.
  void (*toc_stats)(
      x_event* self, x_event_stats* stats, const size_t cycle,
      void* stream, const unsigned int flags);

  union
  {
    struct timespec cpu;
#if X_ENABLE_CU
    CUevent cu;
#endif
#if X_ENABLE_CUDA
    cudaEvent_t cuda;
#endif
  } m_start, m_stop;

  int32_t m_type;  // For faster branching.
};

X_INL x_error x_event_init(x_event* self, const char* type, const unsigned int flags);

X_INL void x_event_term(x_event* self);
/** @} */  // Date and Time

/******************************************************************************
 * @name Error Handling
 * @brief A collection of error handling utilities.
 * @{
 *****************************************************************************/
/// @brief Assertion with optional message.
/// @param expr The expression to assert.
/// @param msg The optional message to print.
/// @attention The message cannot be optional for now, but it will not be
///            printed if it is an empty string.
#define x_assert(expr, msg) do { \
  if (!(expr)) { \
    fprintf(stderr, "Assertion failed: %s\n", #expr); \
    if (!x_strmty(msg)) { \
      fprintf(stderr, "Message: %s\n", msg); \
    } \
    fprintf(stderr, "Position: %s:%lld: %s\n", \
        __FILENAME__, (long long)__LINE__, __PRETTY_FUNCTION__); \
    abort(); \
  } \
} while (false)

/// @brief Wrapping the error handling of a function call.
/// @param cat The error category, should be supported by @ref x_error.
/// @param func The function to call.
/// @param ... The arguments of the function.
/// @return An instance of @ref x_error.
/// @attention Unlike the C++ version, this macro only works with functions
///            that return an integer value.
#define x_check(cat, func, ...) do { \
  x_error err = x_error_set(cat, (int32_t)(func(__VA_ARGS__))); \
  if (x_fail(err)) { \
    char msg[64] = {0}; \
    x_error_msg(msg, sizeof(msg), err); \
    _x_log_impl(__FILENAME__, #func, (long long)__LINE__, 'e', stderr, "%s", msg); \
  } \
} while (false)

/// @brief Check if an instance of @ref x_error indicates a failure.
/// @param err The instance of @ref x_error.
/// @return `true` if the instance is indicating a failure, `false` otherwise.
/// @see @ref x_succ
X_INL bool x_fail(const x_error err);

/// @brief The counterpart of x_fail.
/// @see @ref x_fail
X_INL bool x_succ(const x_error err);

// NOTE: Forward declared above.
// typedef struct _x_error_
// {
//   const char* cat;
//   int32_t val;
//   bool (*fail)(const int32_t);
// } x_error;

X_INL x_error x_ok();

X_INL const char* x_error_msg(char* msg, const size_t msz, const x_error err);

// NOTE: Forward declared above.
/// @brief Set the error category and  value (optionally).
/// @note Since it is difficult to provide the same API as that in C++, this
///       API hopefully suits the most common use cases. Error messages and
///       the custom predicate should be handled separately.
// #define x_error_set(cat,  ...) \
//   x_narg(cat, ##__VA_ARGS__) == 1 ? _x_error_cat(cat) \
//   : _x_error_cat_val(cat, ##__VA_ARGS__)
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
#define x_Pi(T) ((T)3.141592653589793238462643383279502884197169399375)

/// @brief Kibibyte constant generator, i.e., 1 KiB = 1024 bytes.
/// @param n The scale factor.
#define x_KiB(T, n) ((T)n * (T)1024)

/// @brief Mebibyte constant generator, i.e., 1 MiB = 1048576 bytes.
/// @param n The scale factor.
#define x_MiB(T, n) ((T)n * (T)1048576)

/// @brief Gibibyte constant generator, i.e., 1 GiB = 1073741824 bytes.
/// @param n The scale factor.
#define x_GiB(T, n) ((T)n * (T)1073741824)

/// @brief Tebibyte constant generator, i.e., 1 TiB = 1099511627776 bytes.
/// @param n The scale factor.
#define x_TiB(T, n) ((T)n * (T)1099511627776)

/// @brief Pebibyte constant generator, i.e., 1 PiB = 1125899906842620 bytes.
/// @param n The scale factor.
#define x_PiB(T, n) ((T)n * (T)1125899906842620)

/// @brief A macro used to generate an integer with only the n-th bit set to 1.
///        This is useful when one needs enumerations like `0b0001`, `0b0010`,
///        `0b0100` to perform the `&`, `|`, `~` operations.
/// @param n The n-th bit.
/// @see C++'s `std::bitset` for a more versatile solution.
#define x_bit(bit) (1 << (bit))

/// @brief Calculate the greatest common divisor of two integers.
/// @param m The first integer.
/// @param n The second integer.
/// @return The greatest common divisor.
/// @attention This function is only available for integral types.
X_INL uint64_t x_gcd(const uint64_t m, const uint64_t n);

/// @brief Calculate the least common multiple of two integers.
/// @param m The first integer.
/// @param n The second integer.
/// @return The least common multiple.
/// @attention This function is only available for integral types.
X_INL uint64_t x_lcm(const uint64_t m, const uint64_t n);

/// @brief Calculate the next exponent of a base.
/// @param base The base.
/// @param src The source number.
/// @return The next exponent of the base.
/// @attention This function is only available for integral types.
X_INL uint64_t x_next_exp(const uint64_t base, const uint64_t src);

/// @brief Calculate the next multiple of a base.
/// @param base The base.
/// @param src The source number.
/// @return The next multiple of the base.
/// @attention This function is only available for integral types.
X_INL uint64_t x_next_mul(const uint64_t base, const uint64_t src);

/// @brief Calculate the previous exponent of a base.
/// @param base The base.
/// @param src The source number.
/// @return The previous exponent of the base.
/// @attention This function is only available for integral types.
X_INL uint64_t x_prev_exp(const uint64_t base, const uint64_t src);

/// @brief Calculate the previous multiple of a base.
/// @param base The base.
/// @param src The source number.
/// @return The previous multiple of the base.
/// @attention This function is only available for integral types.
X_INL uint64_t x_prev_mul(const uint64_t base, const uint64_t src);
/** @} */  // Mathematics

/******************************************************************************
 * @name Memory Management
 * @brief A collection of memory management utilities.
 * @{
 *****************************************************************************/
/// @brief Get number of items in an array.
/// @param array The array.
/// @attention This function only works with static arrays.
#if (!X_ENABLE_CU || !X_ENABLE_CUDA) && defined(__cplusplus)
template<typename T, size_t N>
X_INL constexpr size_t x_count(const T (&array)[N]);
#else
#define x_count(a) ((sizeof(a) / sizeof(*(a))) / (size_t)(!(sizeof(a) % sizeof(*(a)))))
#endif

#if (!X_ENABLE_CU || !X_ENABLE_CUDA) && defined(__cplusplus)
/// @brief Delete a pointer, allocated by `new` or `new[]`, and set it to
///        `nullptr`.
/// @tparam array Whether the pointer is allocated by `new[]`.
/// @param ptr The pointer to delete.
template<bool array, typename T>
X_INL void x_delete(T*& ptr);
#endif

/// @brief Free a memory block allocated on the heap and set it to `NULL`.
/// @param ptr The memory block to free.
/// @remark If the pointer is `NULL`, this function does nothing.
#define x_free(ptr) do { \
  if ((ptr) != NULL) { \
    free((void*)ptr); \
    ptr = NULL; \
  } \
} while (false)

X_INL x_error x_malloc(void** ptr, const size_t size);

X_INL x_error x_memcpy(void* dst, const void* src, const size_t size);

X_INL x_error x_meminfo(const char* type, size_t* avail, size_t* total);

/// @note Current supported types are:
///            - "cu"
///            - "cuda"
/// @attention The second argument is always required and its type must be:
///            - "cu": `CUdeviceptr`
///            - "cuda": a pointer
X_INL const char* x_memtype(const char* type, ... /*const T* ptr*/);
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

/// @brief Log a message with a specified log level.
/// @param level The log level, one of 'p', 'f', 'e', 'w', 'i', 'd'.
/// @param stream The optional file stream to save the log.
/// @param format The format string as in `printf`.
/// @param ... The optional arguments as in `printf`.
/// @attention The log level is case insensitive, and must be known at compile
///            time.
#define x_log(level, stream, format, ...) do { \
  _x_log_impl(__FILENAME__, __FUNCTION__, __LINE__, level, stream, format, ##__VA_ARGS__); \
} while (false)
/** @} */  // Standard IO

/******************************************************************************
 * @name String
 * @brief A collection of string utilities.
 * @{
 *****************************************************************************/
/// @brief Copy a string with error handling.
X_INL x_error x_strcpy(char* dst, const size_t dsz, const char* src);

/// @brief Check if a string is empty.
X_INL bool x_strmty(const char* string);
/** @} */  // String
#ifdef __cplusplus
}  // extern "C"
#endif

//******************************************************* IMPL_Communication{{{
X_INL uint32_t x_checksum_crc32(
    const void* data, const size_t size, const uint32_t* prev)
{
  const uint8_t* d = (uint8_t*)data;
  size_t cnt = size / sizeof(uint8_t);
  int i = 0;

  uint32_t cks = prev ? ~(*prev) : 0xFFFFFFFF;

  while (cnt--) {
    cks ^= *d++;

    for (i = 0; i < 8; ++i) {
      cks = (cks >> 1) ^ (-(int32_t)(cks & 1) & 0xEDB88320);
    }
  }

  return ~cks;
}

X_INL uint16_t x_checksum_rfc1071(
    const void* data, const size_t size, const uint16_t* prev)
{
  const uint16_t* d = (uint16_t*)data;
  size_t cnt = size / sizeof(uint8_t);
  uint32_t cks = prev ? (uint32_t)(~(*prev)) : 0;

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

  return (uint16_t)(~cks);
}

X_INL uint8_t x_checksum_xor(
    const void* data, const size_t size, const uint8_t* prev)
{
  const uint8_t* d8 = (const uint8_t*)data;
  const uint64_t* d64 = (const uint64_t*)data;
  const size_t dsz = sizeof(uint64_t);
  const size_t cnt = size / dsz;

  union {
    uint8_t u8[8];
    uint64_t u64;
  } cks = {0};

  if (prev) {
    cks.u8[0] = *prev;
  }

  size_t i = 0;
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
// struct x_socket{{{
X_INL x_error _x_socket_accept(x_socket* self, x_socket* client)
{
  if (client == NULL) {
    return x_error_set("posix", EINVAL);
  }

  struct sockaddr addr = {0};
  socklen_t len = 0;

#if X_WINDOWS
  SOCKET hndl = accept(self->m_hndl, &addr, &len);
  if (hndl == INVALID_SOCKET) {
    return x_error_set("socket");
  }
#else
  int hndl = accept(self->m_hndl, &addr, &len);
  if (hndl == -1) {
    return x_error_set("socket");
  }
#endif

  memset(&client->m_addr, 0, sizeof(struct sockaddr));
  memcpy(&client->m_addr, &addr, len);

  client->m_hndl = hndl;

  return x_ok();
}

X_INL x_error _x_socket_addr(x_socket* self, char* ip, uint16_t* port)
{
  if (ip == NULL || port == NULL) {
    return x_error_set("posix", EINVAL);
  }

  struct sockaddr_in* sin = (struct sockaddr_in*)&self->m_addr;

  if (inet_ntop(self->m_domain, &sin->sin_addr, ip, 16) == NULL) {
    return x_error_set("socket");
  }

  *port = sin->sin_port;

  return x_ok();
}

X_INL x_error _x_socket_close(x_socket* self)
{
#if X_WINDOWS
  if (closesocket(self->m_hndl) != 0) {
    return x_error_set("socket");
  }

  return WSACleanup() == 0 ? x_ok() : x_error_set("socket");
#else
  return close(self->m_hndl) == 0 ? x_ok() : x_error_set("socket");
#endif
}

X_INL x_error _x_socket_connect(
    x_socket* self, const char* ip, const uint16_t port)
{
  struct sockaddr_in sin = {0};
  sin.sin_family = self->m_domain;
  sin.sin_port = htons(port);
  int ierr = inet_pton(self->m_domain, ip, &sin.sin_addr);
  if (ierr == 0) {
    return x_error_set("posix", EFAULT);
  } else if (ierr == -1) {
    return x_error_set("socket");
  }

  memcpy(&self->m_addr, &sin, sizeof(struct sockaddr));

  return connect(self->m_hndl, &self->m_addr, sizeof(struct sockaddr_in)) == 0
    ? x_ok() : x_error_set("socket");
}

X_INL x_error _x_socket_getopt(
    x_socket* self, const int lvl, const int opt, void* val, socklen_t* len)
{
  if (val == NULL || len == NULL) {
    return x_error_set("posix", EINVAL);
  }

  return getsockopt(self->m_hndl, lvl, opt, (char*)val, len) == 0
    ? x_ok() : x_error_set("socket");
}

X_INL x_error _x_socket_listen(
    x_socket* self, const char* ip, const uint16_t port)
{
  struct sockaddr_in sin = {0};
  sin.sin_family = self->m_domain;
  sin.sin_port = htons(port);

  int ierr = inet_pton(self->m_domain, ip, &sin.sin_addr);
  if (ierr == 0) {
    return x_error_set("posix", EFAULT);
  } else if (ierr == -1) {
    return x_error_set("socket");
  }

  memcpy(&self->m_addr, &sin, sizeof(struct sockaddr));

  ierr = bind(self->m_hndl, &self->m_addr, sizeof(struct sockaddr_in));
  if (ierr == 0) {
    ierr = listen(self->m_hndl, SOMAXCONN);
  }

  return ierr == 0 ? x_ok() : x_error_set("socket");
}

X_INL x_error _x_socket_recv(
    x_socket* self, void* buf, const size_t size, const int flags)
{
  if (buf == NULL || size == 0) {
    return x_error_set("posix", EINVAL);
  }

#if X_WINDOWS
  int remain = (int)size;
  int bytes = 0;
#else
  size_t remain = size;
  ssize_t bytes = 0;
#endif
  size_t offset = 0;

  while (remain > 0) {
    bytes = recv(self->m_hndl, (char*)buf + offset, remain, flags);
    if (bytes <= 0) {
      return x_error_set("socket");
    }

    offset += bytes;
    remain -= bytes;
  }

  return x_ok();
}

X_INL x_error _x_socket_recvv(
    x_socket* self, x_iovec* iov, const size_t count, const int flags)
{
  if (iov == NULL || count == 0) {
    return x_error_set("posix", EINVAL);
  }

  size_t total = 0;
  for (size_t i = 0; i < count; ++i) {
    if (iov[i].buf == NULL || iov[i].len == 0) {
      return x_error_set("posix", EINVAL);
    }

    if (iov[i].len > (SIZE_MAX - total)) {
      return x_error_set("posix", EOVERFLOW);
    }

    total += iov[i].len;
  }

  // NOTE: _alloca/alloca may be used if all data received are rather small.
  void* buf = malloc(total);
  if (buf == NULL) {
    return x_error_set("posix");
  }

  x_error err = _x_socket_recv(self, buf, total, flags);
  if (x_succ(err)) {
    size_t offset = 0;
    for (size_t i = 0; i < count; ++i) {
      memcpy(iov[i].buf, (char*)buf + offset, iov[i].len);
      offset += iov[i].len;
    }
  }

  free(buf);

  return err;
}

X_INL x_error _x_socket_send(
    x_socket* self, const void* buf, const size_t size, const int flags)
{
#if X_WINDOWS
  int remain = (int)size;
#else
  size_t remain = size;
#endif
  size_t offset = 0;
  int bytes = 0;

  while (remain > 0) {
    bytes = send(self->m_hndl, (char*)buf + offset, remain, flags);
    if (bytes <= 0) {
      return x_error_set("socket");
    }

    offset += bytes;
    remain -= bytes;
  }

  return x_ok();
}

X_INL x_error _x_socket_sendv(
    x_socket* self, const x_iovec* iov, const size_t count, const int flags)
{
  if (iov == NULL || count == 0) {
    return x_error_set("posix", EINVAL);
  }

  size_t total = 0;
  for (size_t i = 0; i < count; ++i) {
    if (iov[i].buf == NULL || iov[i].len == 0) {
      return x_error_set("posix", EINVAL);
    }

    if (iov[i].len > (SIZE_MAX - total)) {
      return x_error_set("posix", EOVERFLOW);
    }

    total += iov[i].len;
  }

  // NOTE: _alloca/alloca may be used if all data sent are rather small.
  void* buf = malloc(total);
  if (buf == NULL) {
    return x_error_set("posix");
  }

  size_t offset = 0;
  for (size_t i = 0; i < count; ++i) {
    memcpy((char*)buf + offset, iov[i].buf, iov[i].len);
    offset += iov[i].len;
  }

  x_error err = _x_socket_send(self, buf, total, flags);

  free(buf);

  return err;
}

X_INL x_error _x_socket_setopt(
    x_socket* self, const int lvl, const int opt, const void* val, const socklen_t len)
{
  if (val == NULL) {
    return x_error_set("posix", EINVAL);
  }

  return setsockopt(self->m_hndl, lvl, opt, (char*)val, len) == 0
    ? x_ok() : x_error_set("socket");
}

X_INL x_error x_socket_init(
    x_socket* self, const int domain, const int type, const int protocol)
{
#if X_WINDOWS
  WSADATA data = {0};
  if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
    return x_error_set("socket");
  }
#endif

  self->m_hndl = socket(domain, type, protocol);
#if X_WINDOWS
  if (self->m_hndl  == INVALID_SOCKET) {
    return x_error_set("socket");
  }
#else
  if (self->m_hndl  == -1) {
    return x_error_set("posix");
  }
#endif

  int val = 1;
  socklen_t len = (socklen_t)sizeof(val);
  setsockopt(self->m_hndl, SOL_SOCKET, SO_KEEPALIVE, (char*)&val, len);
#if X_WINDOWS
  setsockopt(self->m_hndl, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char*)&val, len);
#else
  val = 0;
  setsockopt(self->m_hndl, SOL_SOCKET, SO_REUSEADDR, (char*)&val, len);
#endif

  self->m_domain = domain;
  self->accept = _x_socket_accept;
  self->addr = _x_socket_addr;
  self->close = _x_socket_close;
  self->connect = _x_socket_connect;
  self->getopt = _x_socket_getopt;
  self->listen = _x_socket_listen;
  self->recv = _x_socket_recv;
  self->recvv = _x_socket_recvv;
  self->send = _x_socket_send;
  self->sendv = _x_socket_sendv;
  self->setopt = _x_socket_setopt;

  return x_ok();
}

X_INL x_error x_socket_term(x_socket* self)
{
  return _x_socket_close(self);
}
// class x_socket}}}
#endif  // X_ENABLE_SOCKET
// IMPL_Communication}}}

//********************************************************** IMPL_Console_IO{{{
#if !X_WINDOWS
X_INL int _kbhit()
{
  static bool initialized = false;
  if (!initialized) {
    struct termios settings = {0};
    tcgetattr(STDIN_FILENO, &settings);
    settings.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &settings);
    setbuf(stdin, NULL);
    initialized = true;
  }

  int byte = 0;
  ioctl(STDIN_FILENO, FIONREAD, &byte);

  return byte;
}
#endif

X_INL int x_getch()
{
#if X_WINDOWS
  return _kbhit() ? toupper(_getch()) : 0;
#else
  int key = 0;

  int bytes_waiting = _kbhit();
  if (bytes_waiting <= 0) {
    return 0;
  }

  struct termios old_settings = {0};
  struct termios new_settings = {0};
  union {
    int in;
    char ch[4];
  } buf = 0;
  int ierr = 0;
  ssize_t bytes_read = 0;

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
  struct timespec req = {0};
  struct timespec rem = {0};

  req.tv_sec = ms / 1000;
  req.tv_nsec = (long)((ms % 1000) * 1000000);

  if (nanosleep(&req, &rem) == EINTR) {
    nanosleep(&rem, NULL);
  }
#endif
}

X_INL const char* x_timestamp(char* buf, const size_t bsz)
{
  if (buf == NULL) {
    return "";
  }

  time_t now = time(NULL);

#if X_WINDOWS
  if (ctime_s(buf, bsz, &now) != 0) {
    return "";
  }
#else
  ctime_r(&now, buf);
#endif

  buf[strlen(buf) - 1] = '\0';

  return buf;
}

// struct _x_event_stats_{{{
X_INL void _x_event_stats_echo(
    x_event_stats* self, const char* unit, const char* title, FILE* const stream)
{
  const char* t = x_strmty(title) ? "STATS" : title;
  char msg[128] = {0};

  double scale = 1.0;

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

  snprintf(msg, 128, "[%s] %f%s in %zu cycles - avg: %f%s, min(%zu): %f%s, max(%zu): %f%s",
      t, self->sum / scale, unit, self->cyc, self->avg / scale, unit,
      self->min.idx, self->min.val / scale, unit,
      self->max.idx, self->max.val / scale, unit);

  fprintf(stream ? stream : stdout, "%s\n", msg);
}

X_INL void _x_event_stats_reset(x_event_stats* self)
{
  self->ready = false;
  self->cyc = 0;
  self->sum = 0.0;
  self->avg = 0.0;
  self->max.idx = 0;
  self->max.val = DBL_MIN;
  self->min.idx = 0;
  self->min.val = DBL_MAX;
}

X_INL void x_event_stats_init(x_event_stats* self)
{
  _x_event_stats_reset(self);
  self->echo = _x_event_stats_echo;
  self->reset = _x_event_stats_reset;
}
// struct _x_event_stats_}}}

// struct _x_event_{{{
X_INL double _x_duration_cpu(
    const struct timespec start, const struct timespec stop)
{
  int64_t s = (int64_t)(stop.tv_sec - start.tv_sec);
  int64_t ns = (int64_t)(stop.tv_nsec - start.tv_nsec);

  return (double)(s * 1e9 + ns);
}

X_INL struct timespec _x_now_cpu()
{
  struct timespec ts = {0};

#if X_WINDOWS || __STDC_VERSION__ >= 201112L
  timespec_get(&ts, TIME_UTC);
#else
  clock_gettime(CLOCK_MONOTONIC, &ts);
#endif

  return ts;
}

X_INL void _x_tic_cpu(struct timespec* ts)
{
#if X_WINDOWS || __STDC_VERSION__ >= 201112L
  timespec_get(ts, TIME_UTC);
#else
  clock_gettime(CLOCK_MONOTONIC, ts);
#endif
}

#define _x_toc_cpu _x_tic_cpu

#if X_ENABLE_CU
X_INL double _x_duration_cu(const CUevent start, const CUevent stop)
{
  const char* msg = NULL;

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

  float ms = 0.0f;
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

  return (double)ms * 1000000.0;
}

X_INL CUevent _x_now_cu(const unsigned int flags)
{
  CUevent evt = {0};
  const char* msg = NULL;

  CUresult cres = cuEventCreate(&evt, flags);
  if (cres != CUDA_SUCCESS) {
    cres = cuGetErrorString(cres, &msg);
    if (cres == CUDA_SUCCESS) {
      fprintf(stderr, "cuEventCreate: %s\n", msg);
    } else {
      fprintf(stderr, "cuEventCreate: unknown error %d\n", cres);
    }
    exit(EXIT_FAILURE);
  }

  return evt;
}

X_INL void _x_tic_cu(CUevent event, CUstream stream, const unsigned int flags)
{
  const char* msg = NULL;

  CUresult cres = cuEventRecordWithFlags(event, stream, flags);
  if (cres != CUDA_SUCCESS) {
    cres = cuGetErrorString(cres, &msg);
    if (cres == CUDA_SUCCESS) {
      fprintf(stderr, "cuEventRecordWithFlags: %s\n", msg);
    } else {
      fprintf(stderr, "cuEventRecordWithFlags: unknown error %d\n", cres);
    }
    exit(EXIT_FAILURE);
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
    fprintf(stderr, "cudaEventCreateWithFlags: %s\n", cudaGetErrorString(cerr));
    exit(EXIT_FAILURE);
  }

  return evt;
}

X_INL void _x_tic_cuda(
    cudaEvent_t event, cudaStream_t stream, const unsigned int flags)
{
  cudaError_t cerr = cudaEventRecordWithFlags(event, stream, flags);
  if (cerr != cudaSuccess) {
    fprintf(stderr, "cudaEventRecordWithFlags: %s\n", cudaGetErrorString(cerr));
    exit(EXIT_FAILURE);
  }
}

#define _x_toc_cuda _x_tic_cuda
#endif  // X_ENABLE_CUDA

X_INL double _x_event_duration(x_event* self, const char* unit)
{
  double ns = -1.0;

  switch (self->m_type) {
    case 0:
      ns = _x_duration_cpu(self->m_start.cpu, self->m_stop.cpu);
      break;
#if X_ENABLE_CU
    case 1:
      ns = _x_duration_cu(self->m_start.cu, self->m_stop.cu);
      break;
#endif
#if X_ENABLE_CUDA
    case 2:
      ns = _x_duration_cuda(self->m_start.cuda, self->m_stop.cuda);
      break;
#endif
    default:
      fprintf(stderr, "x_event::duration: unsupported call\n");
      exit(EXIT_FAILURE);
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

X_INL void _x_event_tic(x_event* self, void* stream, const unsigned int flags)
{
  switch (self->m_type) {
    case 0:
      _x_tic_cpu(&self->m_start.cpu);
      break;
#if X_ENABLE_CU
    case 1:
      _x_tic_cu(self->m_start.cu, static_cast<CUstream>(stream), flags);
      break;
#endif
#if X_ENABLE_CUDA
    case 2:
      _x_tic_cuda(self->m_start.cuda, static_cast<cudaStream_t>(stream), flags);
      break;
#endif
    default:
      fprintf(stderr, "x_event::tic: unsupported call\n");
  }
}

X_INL void _x_event_toc(x_event* self, void* stream, const unsigned int flags)
{
  switch (self->m_type) {
    case 0:
      _x_toc_cpu(&self->m_stop.cpu);
      break;
#if X_ENABLE_CU
    case 1:
      _x_toc_cu(self->m_stop.cu, static_cast<CUstream>(stream), flags);
      break;
#endif
#if X_ENABLE_CUDA
    case 2:
      _x_toc_cuda(self->m_stop.cuda, static_cast<cudaStream_t>(stream), flags);
      break;
#endif
    default:
      fprintf(stderr, "x_event::toc: unsupported call\n");
  }
}

X_INL void _x_event_toc_stats(
    x_event* self, x_event_stats* stats, const size_t cycle, void* stream,
    const unsigned int flags)
{
  if (cycle == 0) {
    // NOTE: To ensure member functions are available.
    x_event_stats_init(stats);
    return;
  }

  // NOTE: If the statistics are ready, do not update them.
  if (stats->ready) {
    return;
  }

  // NOTE: Reset the stats before the first cycle.
  if (stats->cyc == 0) {
    x_event_stats_init(stats);
  }

  _x_event_toc(self, stream, flags);

  double ns = _x_event_duration(self, "ns");

  if (ns > stats->max.val) {
    stats->max.idx = stats->cyc;
    stats->max.val = ns;
  }
  if (ns < stats->min.val) {
    stats->min.idx = stats->cyc;
    stats->min.val = ns;
  }

  stats->sum += ns;
  stats->cyc += 1;
  stats->avg = stats->sum / stats->cyc;

  if (stats->cyc % cycle == 0) {
    stats->ready = true;
  }
}

X_INL x_error x_event_init(
    x_event* self, const char* type, const unsigned int flags)
{
  if (strcmp(type, "cpu") == 0) {
    self->m_type = 0;
#if X_ENABLE_CU
  } else if (strcmp(type, "cu") == 0) {
    self->m_type = 1;
    self->m_start.cu = _x_now_cu(flags);
    self->m_stop.cu = _x_now_cu(flags);
#endif
#if X_ENABLE_CUDA
  } else if (strcmp(type, "cuda") == 0) {
    self->m_type = 2;
    self->m_start.cuda = _x_now_cuda(flags);
    self->m_stop.cuda = _x_now_cuda(flags);
#endif
  } else {
    self->m_type = -1;
    fprintf(stderr, "x_event: unsupported type %s\n", type);
    return x_error_set("posix", ENOTSUP);
  }

  self->duration = _x_event_duration;
  self->tic = _x_event_tic;
  self->toc = _x_event_toc;
  self->toc_stats = _x_event_toc_stats;

  return x_ok();
}

X_INL void x_event_term(x_event* self)
{
  switch (self->m_type) {
    case 0:
      break;
#if X_ENABLE_CU
    case 1:
      cuEventDestroy(self->m_start.cu);
      cuEventDestroy(self->m_stop.cu);
      break;
#endif
#if X_ENABLE_CUDA
    case 2:
      cudaEventDestroy(self->m_start.cuda);
      cudaEventDestroy(self->m_stop.cuda);
      break;
#endif
    default:
      break;
  }
}
// struct _x_event_}}}
// IMPL_Date_and_Time}}}

//****************************************************** IMPL_Error_Handling{{{
X_INL bool x_fail(const x_error err)
{
  if (err.fail) {
    // NOTE: Covers the "custom" and other customized cases.
    return err.fail(err.val);
  } else {
    if (strcmp(err.cat, "socket") == 0) {
      return err.val != 0;
    }
#if X_WINDOWS
    else if (strcmp(err.cat, "win32") == 0) {
      return err.val != 0;
    }
#endif
#if X_ENABLE_CU
    else if (strcmp(err.cat, "cu") == 0) {
      return (CUresult)err.val != CUDA_SUCCESS;
    }
#endif
#if X_ENABLE_CUDA
    else if (strcmp(err.cat, "cuda") == 0) {
      return (cudaError_t)err.val != cudaSuccess;
    }
#endif
    else {
      // NOTE: Covers the "posix" case.
      return err.val != 0;
    }
  }
}

X_INL bool x_succ(const x_error err)
{
  return !x_fail(err);
}

// struct _x_error_{{{
X_INL x_error _x_error_cat(const char* cat)
{
  x_error err = {cat, 0, NULL};

  if (strcmp(cat, "posix") == 0) {
    err.val = (int32_t)errno;
#if X_WINDOWS
  } else if (strcmp(cat, "win32") == 0) {
    err.val = (int32_t)GetLastError();
  } else if (strcmp(cat, "socket") == 0) {
    err.val = (int32_t)WSAGetLastError();
#endif
#if X_ENABLE_CUDA
  } else if (strcmp(cat, "cuda") == 0) {
    err.val = (int32_t)cudaGetLastError();
#endif
  } else {
    fprintf(stderr, "x_error: unsupported error category %s\n", cat);
    exit(EXIT_FAILURE);
  }

  return err;
}

X_INL x_error _x_error_cat_val(const char* cat, ... /*const int32_t val*/)
{
  va_list args;
  va_start(args, cat);
  int32_t val = va_arg(args, int32_t);
  va_end(args);

  x_error err = {cat, val, NULL};

  return err;
}

X_INL const char* x_error_msg(char* msg, const size_t msz, const x_error err)
{
  if (msg == NULL || msz == 0) {
    return "";
  }

#if X_WINDOWS
  if (strcmp(err.cat, "posix") == 0) {
    strerror_s(msg, msz, (int)err.val);
  } else if (strcmp(err.cat, "win32") == 0 || strcmp(err.cat, "socket") == 0) {
    FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM
        | FORMAT_MESSAGE_IGNORE_INSERTS
        | FORMAT_MESSAGE_MAX_WIDTH_MASK,
        NULL, (DWORD)err.val,
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        msg, (DWORD)msz, NULL);
  }
#else
  if (strcmp(err.cat, "posix") == 0 || strcmp(err.cat, "socket") == 0) {
    msg = strerror((int)err.val);
  }
#endif
#if X_ENABLE_CU
  else if (strcmp(err.cat, "cu") == 0) {
    const char* buf = NULL;
    CUresult cres = cuGetErrorString((CUresult)err.val, &buf);
    if (cres == CUDA_SUCCESS) {
      x_strcpy(msg, msz, buf);
    } else {
      snprintf(msg, msz, "Unknown CUDA driver error %d", cres);
    }
  }
#endif
#if X_ENABLE_CUDA
  else if (strcmp(err.cat, "cuda") == 0) {
    const char* buf = cudaGetErrorString((cudaError_t)err.val);
    x_strcpy(msg, msz, buf);
  }
#endif
  else {
    snprintf(msg, msz, "Custom error %d", err.val);
  }

  return msg;
}

X_INL x_error x_ok()
{
  static x_error err = {"posix", 0, NULL};
  return err;
}
// struct _x_error_}}}
// IMPL_Error_Handling}}}

//********************************************************* IMPL_File_System{{{
X_INL bool x_fexist(const char* file)
{
  int ierr = 0;

#if X_WINDOWS
  struct _stat64 s = {0};
  ierr = _stat64(file, &s);
#else
  struct stat s = {0};
  ierr = stat(file, &s);
#endif

  return ierr == 0;
}

X_INL x_error x_fopen(FILE** stream, const char* file, const char* mode)
{
#if X_WINDOWS
  errno_t ierr = fopen_s(stream, file, mode);
  if (ierr != 0) {
    return x_error_set("posix", ierr);
  }
#else
  *stream = fopen(file, mode);
  if (*stream == NULL) {
    return x_error_set("posix");
  }
#endif

  return x_ok();
}

X_INL const char* x_fpath(char* dst, const char* src)
{
#if X_WINDOWS
  return dst != NULL ? _fullpath(dst, src, X_PATH_MAX) : NULL;
#else
  return dst != NULL ? realpath(src, dst) : NULL;
#endif
}

X_INL int64_t x_fsize(const char* file)
{
  int ierr = 0;

#if X_WINDOWS
  struct _stat64 s = {0};
  ierr = _stat64(file, &s);
#else
  struct stat s = {0};
  ierr = stat(file, &s);
#endif

  return ierr == 0 ? (int64_t)s.st_size : -1;
}

X_INL x_error x_split_path(
    const char *path,
    char *root, const size_t rsz, char *dir, const size_t dsz,
    char *file, const size_t fsz, char *ext, const size_t esz)
{
  char full[X_PATH_MAX] = {0};
  x_fpath(full, path);

  if (!x_fexist(full)) {
    return x_error_set("posix", ENOENT);
  }

#if X_WINDOWS
  return x_error_set(
      "posix", _splitpath_s(full, root, rsz, dir, dsz, file, fsz, ext, esz));
#else
  if (root == NULL || rsz == 0 || dir == NULL || dsz == 0
      || file == NULL || fsz == 0 || ext == NULL || esz == 0) {
    return x_error_set("posix", EINVAL);
  }

  if (root != NULL) { root[0] = '\0'; }
  if (dir != NULL) { dir[0] = '\0'; }
  if (file != NULL) { file[0] = '\0'; }
  if (ext != NULL) { ext[0] = '\0'; }

  size_t psz = strlen(full);
  size_t sz = 0;
  char* begin = NULL;
  char* end = NULL;

  // root
  begin = strchr((char*)path, '/');
  if (begin == NULL) {
    return x_error_set("posix", ENOENT);
  }

  end = strchr(begin + 1, '/');
  if (end == NULL) {
    end = full + psz;
  }

  if (root != NULL) {
    sz = end - begin;
    if (sz >= rsz) {
      return x_error_set("posix", ENOBUFS);
    }

    memcpy(root, begin, sz);
    root[sz] = '\0';

    if (end == NULL) {
      return x_ok();
    }
  }

  // dir
  begin = strchr(end, '/');
  if (begin == NULL) {
    return x_ok();
  }

  end = strrchr((char*)path, '/');
  if (end <= begin) {
    return x_ok();
  }
  if (end == NULL) {
    end = full + psz;
  }

  if (dir != NULL) {
    sz = end - begin;
    if (sz >= dsz) {
      return x_error_set("posix", ENOBUFS);
    }

    memcpy(dir, begin, sz);
    dir[sz] = '\0';

    if (end == NULL) {
      return x_ok();
    }
  }

  // file
  begin = end + 1;
  if ((begin - full) >= 0) {
    return x_ok();
  }

  end = strrchr((char*)path, '.');
  if (end <= begin) {
    return x_ok();
  }
  if (end == NULL) {
    end = full + psz;
  }

  if (file != NULL) {
    sz = end - begin;
    if (sz >= fsz) {
      return x_error_set("posix", ENOBUFS);
    }

    memcpy(file, begin, sz);
    file[sz] = '\0';
  }

  // ext
  if (ext != NULL) {
    begin = end;
    end = full + psz;
    if (end <= begin) {
      return x_ok();
    }

    sz = end - begin;
    memcpy(ext, begin, sz);
    ext[sz] = '\0';
  }

  return x_ok();
#endif
}
// IMPL_File_System}}}

//************************************************************ IMPL_Hardware{{{
X_INL size_t x_ncpu()
{
#if X_WINDOWS
  SYSTEM_INFO info = {0};
  GetSystemInfo(&info);
  return (size_t)info.dwNumberOfProcessors;
#else
  return (size_t)sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

X_INL size_t x_ngpu(const char* api)
{
  if (x_strmty(api)) {
    return 0;
  }

  if (strcmp(api, "cu") == 0) {
#if X_ENABLE_CU
    int count = 0;
    CUresult cres = cuDeviceGetCount(&count);
    if (cres != CUDA_SUCCESS) {
      const char* msg = NULL;
      cres = cuGetErrorString(cres, &msg);
      if (cres == CUDA_SUCCESS) {
        fprintf(stderr, "cuDeviceGetCount: %s\n", msg);
      } else {
        fprintf(stderr, "cuDeviceGetCount: unknown error %d\n", cres);
      }
      return 0;
    }

    return (size_t)count;
#else
    return 0;
#endif
  } else if (strcmp(api, "cuda") == 0) {
#if X_ENABLE_CUDA
    int count = 0;
    cudaError_t cerr = cudaGetDeviceCount(&count);
    if (cerr != cudaSuccess) {
      fprintf(stderr, "cudaGetDeviceCount: %s\n", cudaGetErrorString(cerr));
      return 0;
    }

    return (size_t)count;
#else
    return 0;
#endif
  }

  return 0;
}
// IMPL_Hardware}}}

//********************************************************* IMPL_Mathematics{{{
X_INL uint64_t x_gcd(const uint64_t m, const uint64_t n)
{
  return n == 0 ? m : x_gcd(n, m % n);
}

X_INL uint64_t x_lcm(const uint64_t m, const uint64_t n)
{
  if (m == 0 || n == 0) {
    return 0;
  }

  return m / x_gcd(m, n) * n;
}

X_INL uint64_t x_next_exp(const uint64_t base, const uint64_t src)
{
  if (src == 0) {
    return 1;
  }

  if (base == 2) {
    if ((src & (src - 1)) == 0) {
      return src;
    }

    uint64_t s = src;
    uint64_t count = 0;
    while (s != 0) {
      s >>= 1;
      count += 1;
    }

    return 1 << count;
  } else {
    double exp = log((double)src) / log((double)base);

    if (exp == round(exp)) {
      return src;
    }

    return (uint64_t)pow(base, ceil(exp));
  }
}

X_INL uint64_t x_next_mul(const uint64_t base, const uint64_t src)
{
  return (src / base + 1) * base;
}

X_INL uint64_t x_prev_exp(const uint64_t base, const uint64_t src)
{
  if (src == 0) {
    return 0;
  }

  if (base == 2) {
    if ((src & (src - 1)) == 0) {
      return src;
    }

    uint64_t s = src;
    uint64_t count = 0;
    while (s != 0) {
      s >>= 1;
      count += 1;
    }

    return 1 << (count - 1);
  } else {
    double exp = log((double)src) / log((double)base);

    if (exp == round(exp)) {
      return src;
    }

    return pow(base, floor(exp));
  }
}

X_INL uint64_t x_prev_mul(const uint64_t base, const uint64_t src)
{
  return (src / base) * base;
}
// IMPL_Mathematics}}}

//*************************************************** IMPL_Memory_Management{{{
#if (!X_ENABLE_CU || !X_ENABLE_CUDA) && defined(__cplusplus)
template<typename T, size_t N>
X_INL constexpr size_t x_count(const T (&array)[N])
{
  return N;
}
#endif

#if (!X_ENABLE_CU || !X_ENABLE_CUDA) && defined(__cplusplus)
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
#endif

X_INL x_error x_malloc(void** ptr, const size_t size)
{
  if (*ptr != NULL) {
    return x_error_set("posix", EINVAL);
  }

  *ptr = malloc(size);
  if (*ptr == NULL) {
    return x_error_set("posix");
  }

  return x_ok();
}

X_INL x_error x_memcpy(void* dst, const void* src, const size_t size)
{
  if (dst == NULL || src == NULL) {
    return x_error_set("posix", EINVAL);
  }

  if (size != 0) {
    memcpy(dst, src, size);
  }

  return x_ok();
}

X_INL x_error _x_meminfo_cpu(size_t* avail, size_t* total)
{
  if (avail == NULL && total == NULL) {
    return x_error_set("posix", EINVAL);
  }

#if X_WINDOWS
  MEMORYSTATUSEX status = {0};
  status.dwLength = sizeof(status);

  if (!GlobalMemoryStatusEx(&status)) {
    return x_error_set("win32");
  }

  if (avail != NULL) {
    *avail = (size_t)status.ullAvailPhys;
  }
  if (total != NULL) {
    *total = (size_t)status.ullTotalPhys;
  }
#else
  struct sysinfo info = {0};
  if (sysinfo(&info) != 0) {
    return x_error_set("posix");
  }

  if (avail != NULL) {
    *avail = (size_t)info.freeram;
  }
  if (total != NULL) {
    *total = (size_t)info.totalram;
  }
#endif

  return x_ok();
}

#if X_ENABLE_CU
X_INL x_error _x_meminfo_cu(size_t* avail, size_t* total)
{
  if (avail == NULL && total == NULL) {
    return x_error_set("posix", EINVAL);
  }

  CUresult cres = cuMemGetInfo(avail, total);
  if (cres != CUDA_SUCCESS) {
    return x_error_set("cu", cres);
  }

  return x_ok();
}
#endif

#if X_ENABLE_CUDA
X_INL x_error _x_meminfo_cuda(size_t* avail, size_t* total)
{
  if (avail == NULL && total == NULL) {
    return x_error_set("posix", EINVAL);
  }

  cudaError_t cerr = cudaMemGetInfo(avail, total);
  if (cerr != cudaSuccess) {
    return x_error_set("cuda", cerr);
  }

  return x_ok();
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
    return x_error_set("posix", ENOTSUP);
  }
}

#if X_ENABLE_CU
X_INL const char* _x_memtype_cu(const CUdeviceptr ptr)
{
  static const char* type[] = {"Unknown", "Host", "Device", "Array", "Unified"};

  CUmemorytype attr;
  CUresult cres = cuPointerGetAttribute(&attr, CU_POINTER_ATTRIBUTE_MEMORY_TYPE, ptr);
  if (cres != CUDA_SUCCESS) {
    const char* msg = NULL;
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

X_INL const char* x_memtype(const char* type, ... /*const T* ptr*/)
{
#if X_ENABLE_CU
  if (strcmp(type, "cu") == 0) {
    va_list args;
    va_start(args, type);
    CUdeviceptr ptr = va_arg(args, CUdeviceptr);
    va_end(args);
    return _x_memtype_cu(ptr);
  }
#endif

#if X_ENABLE_CUDA
  if (strcmp(type, "cuda") == 0) {
    va_list args;
    va_start(args, type);
    void* ptr = va_arg(args, void*);
    va_end(args);
    return _x_memtype_cuda(ptr);
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

X_INL void _x_log_prefix(
    char* buf, const size_t bsz, const char level,
    const char* filename, const char* function, const long long line)
{
  char timestamp[26] = {0};

#ifdef NDEBUG
  snprintf(buf, bsz, "[%c %s] ", toupper(level), x_timestamp(timestamp, 26));
#else
  snprintf(
      buf, bsz, "[%c %s | %s - %s - %lld] ",
      toupper(level), x_timestamp(timestamp, 26), filename, function, line);
#endif
}

X_INL void _x_log_impl(
    const char* filename, const char* function, const long long line,
    const char level, FILE* stream, const char* format, ...)
{
  char color_level[8] = {0};
  char color_reset[8] = {0};

  switch (toupper(level)) {
    case 'P':
#if X_LOG_LEVEL >= X_LOG_PLAIN
      snprintf(color_level, 8, _X_LOG_COLOR_P);
      break;
#else
      return;
#endif
    case 'F':
#if X_LOG_LEVEL >= X_LOG_FATAL
      snprintf(color_level, 8, _X_LOG_COLOR_F);
      break;
#else
      return;
#endif
    case 'E':
#if X_LOG_LEVEL >= X_LOG_ERROR
      snprintf(color_level, 8, _X_LOG_COLOR_E);
      break;
#else
      return;
#endif
    case 'W':
#if X_LOG_LEVEL >= X_LOG_WARNING
      snprintf(color_level, 8, _X_LOG_COLOR_W);
      break;
#else
      return;
#endif
    case 'I':
#if X_LOG_LEVEL >= X_LOG_INFO
      snprintf(color_level, 8, _X_LOG_COLOR_I);
      break;
#else
      return;
#endif
    case 'D':
#if X_LOG_LEVEL >= X_LOG_DEBUG
      snprintf(color_level, 8, _X_LOG_COLOR_D);
      break;
#else
      return;
#endif
    default:
      return;
  }

  snprintf(color_reset, 8, _X_COLOR_RESET);

  char prefix[X_LOG_PREFIX_LIMIT] = {0};
  _x_log_prefix(prefix, X_LOG_PREFIX_LIMIT, level, filename, function, line);

  char msg[X_LOG_MSG_LIMIT] = {0};
  va_list args;
  va_start(args, format);
  vsnprintf(msg, X_LOG_MSG_LIMIT, format, args);
  va_end(args);

  if (stream == NULL || stream == stdout || stream == stderr) {
    fprintf(
        stream == NULL ? stdout : stream,
        "%s%s%s%s\n", color_level, prefix, msg, color_reset);
  } else {
    fprintf(stream, "%s%s\n", prefix, msg);
  }
}
// x_log}}}
// IMPL_Standard_IO}}}

//************************************************************** IMPL_String{{{
X_INL x_error x_strcpy(char* dst, const size_t dsz, const char* src)
{
  if (dst == NULL || dsz == 0) {
    return x_error_set("posix", EINVAL);
  }

  size_t cpy_sz = dsz - 1;
  size_t src_sz = strlen(src);

  if (src_sz > 0) {
    cpy_sz = cpy_sz < src_sz ? cpy_sz : src_sz;

    x_error err = x_memcpy(dst, src, cpy_sz);
    if (x_fail(err)) {
      return err;
    }
  }

  dst[cpy_sz] = '\0';

  return x_ok();
}

X_INL bool x_strmty(const char* string)
{
  return string == NULL || string[0] == '\0';
}
// IMPL_String}}}


#endif  // X_H
