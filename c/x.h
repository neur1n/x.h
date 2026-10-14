/******************************************************************************
Copyright (c) 2023 Jihang Li
x.h is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details.
******************************************************************************/

/** @file
 * @brief Cross-platform C utilities.
 * @note On Linux, include this header before any system header so its default
 *       `_GNU_SOURCE` and `_FILE_OFFSET_BITS=64` definitions take effect.
 *       Alternatively, define these macros in the build configuration before
 *       any headers, including precompiled or forced includes.
 * @note On Cygwin and Linux, keep `_FILE_OFFSET_BITS` consistent across
 *       translation units that share file-system interfaces, and configure it
 *       before including system headers.
 */
#ifndef X_H
#define X_H x_version(0, 9, 2)


/// @brief Version and included modules; Core includes Error Handling.
#define X_H_VERBOSE "0.9.2+core.log.checksum.time.fs.hw.math.mem.str.cuda"

/** @internal
 * Table of Contents
 *
 * Feature_Configuration
 * Architecture_Detection
 * Compiler_Detection
 * Operating_System_Detection
 * Platform_Detection
 *
 * Headers
 *
 * Symbol_Visibility
 * Miscellaneous
 *
 * DECL_Error_Handling
 * DECL_Logging
 * DECL_Checksums
 * DECL_Date_and_Time
 * DECL_File_System
 * DECL_Hardware
 * DECL_Mathematics
 * DECL_Memory
 * DECL_String
 * DECL_CUDA
 *
 * IMPL_Error_Handling
 * IMPL_Logging
 * IMPL_Checksums
 * IMPL_Date_and_Time
 * IMPL_File_System
 * IMPL_Hardware
 * IMPL_Mathematics
 * IMPL_Memory
 * IMPL_String
 * IMPL_CUDA
 * @endinternal
 */

/// @brief Generate a version number.
/// @param major The major version number, ranges in [0, 99].
/// @param minor The minor version number, ranges in [0, 99].
/// @param patch The patch version number, ranges in [0, 99999].
/// @return The version number.
/// @remark A big range of patch allow the result version number specifically
///         work with [`_MSC_FULL_VER`](https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-170).
#define x_version(major, minor, patch) \
  (((major) % 100) * 10000000 + ((minor) % 100) * 100000 + ((patch) % 100000))

/******************************************************************************
 * @name Feature_Configuration{{{
 *****************************************************************************/
/// @brief Toggle the availability of CUDA driver API related functions.
#ifndef X_ENABLE_CUDA_DRIVER
#define X_ENABLE_CUDA_DRIVER (0)
#endif

/// @brief Toggle the availability of CUDA runtime API related functions.
#ifndef X_ENABLE_CUDA_RUNTIME
#define X_ENABLE_CUDA_RUNTIME (0)
#endif

/// @brief Toggle the availability of cuBLAS related functions.
#ifndef X_ENABLE_CUBLAS
#define X_ENABLE_CUBLAS (0)
#endif

/// @brief Toggle the built-in `"socket"` error domain.
/// @note Captures `WSAGetLastError()` on Windows and `errno` on POSIX. This
///       does not provide socket operations. Windows requires the Winsock
///       library.
#ifndef X_ENABLE_SOCKET
#define X_ENABLE_SOCKET (0)
#endif
// Feature_Configuration}}}

/******************************************************************************
 * @name Architecture_Detection{{{
 *****************************************************************************/
#if defined(_M_ARM64) || defined(__AARCH64EL__) \
  || defined(__aarch64__) || defined(__arm64)
#define X_ARM64 (1)
#else
#define X_ARM64 (0)
#endif

#if X_ARM64 || defined(_M_ARM) || defined(__ARM_ARCH) \
  || defined(__ARM_ARCH_4__) || defined(__ARM_ARCH_4T__) \
  || defined(__ARM_ARCH_5TE__) || defined(__ARM_ARCH_5TEJ__) \
  || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6KZ__) \
  || defined(__ARM_ARCH_6T2__) || defined(__ARM_ARCH_6Z__) \
  || defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) \
  || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7R__) \
  || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB) \
  || defined(__arm__) || defined(__thumb__)
#define X_ARM (1)
#else
#define X_ARM (0)
#endif

#if defined(_M_IX86) || defined(_X86_) \
  || defined(i386) || defined(__i386) || defined(__i386__) \
  || defined(__i486__) || defined(__i586__) || defined(__i686__) \
  || defined(__I86__) || defined(__INTEL__) || defined(__THW_INTEL__)
#define X_X86 (1)
#else
#define X_X86 (0)
#endif

#if defined(_M_AMD64) || defined(_M_X64) \
  || defined(__amd64) || defined(__amd64__) \
  || defined(__x86_64) || defined(__x86_64__)
#define X_X64 (1)
#else
#define X_X64 (0)
#endif
// Architecture_Detection}}}

/******************************************************************************
 * @name Compiler_Detection{{{
 *****************************************************************************/
#if defined(__clang__)
#define X_CLANG x_version(__clang_major__, __clang_minor__, __clang_patchlevel__)
#else
#define X_CLANG (0)
#endif

#if defined(__GNUC__) \
  && !defined(__ECC) && !defined(__ICC) && !defined(__ICL) \
  && !defined(__INTEL_COMPILER) && !defined(__INTEL_LLVM_COMPILER) \
  && !defined(__TINYC__) && !defined(__clang__)
#if defined(__GNUC_PATCHLEVEL__)
#define X_GCC x_version(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#else
#define X_GCC x_version(__GNUC__, __GNUC_MINOR__, 0)
#endif
#else
#define X_GCC (0)
#endif

#if defined(_MSC_VER) \
  && !X_CLANG && !defined(__ECC) && !defined(__ICC) && !defined(__ICL) \
  && !defined(__INTEL_COMPILER) && !defined(__INTEL_LLVM_COMPILER)
#ifdef _MSC_FULL_VER
#define X_MSVC _MSC_FULL_VER
#else
#define X_MSVC x_version((_MSC_VER / 100) % 100, _MSC_VER % 100, 0)
#endif
#else
#define X_MSVC (0)
#endif

#if defined(__NVCC__)
#if defined(__CUDACC_VER_BUILD__) && defined(__CUDACC_VER_MAJOR__) \
  && defined(__CUDACC_VER_MINOR__)
#define X_NVCC x_version(__CUDACC_VER_MAJOR__, __CUDACC_VER_MINOR__, __CUDACC_VER_BUILD__)
#else
#define X_NVCC (1)
#endif
#else
#define X_NVCC (0)
#endif
// Compiler_Detection}}}

/******************************************************************************
 * @name Operating_System_Detection{{{
 *****************************************************************************/
#if defined(__CYGWIN__)
#include <cygwin/version.h>
/// @note It needs a special handling because `CYGWIN_VERSION_API_MINOR` is
///       greater than 99.
/// @see [Cygwin](https://github.com/cygwin/cygwin/blob/main/winsup/cygwin/include/cygwin/version.h)
#define X_CYGWIN x_version( \
    CYGWIN_VERSION_API_MAJOR + CYGWIN_VERSION_API_MINOR / 100, \
    CYGWIN_VERSION_API_MINOR % 100, 0)
#else
#define X_CYGWIN (0)
#endif

#if defined(__APPLE__) && defined(__MACH__) \
  && defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)
#define X_IOS (__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ * 1000)
#else
#define X_IOS (0)
#endif

#if defined(__gnu_linux__) || defined(__linux) \
  || defined(__linux__) || defined(linux)
#define X_LINUX (1)
#else
#define X_LINUX (0)
#endif

#if defined(Macintosh) || defined(macintosh)
#define X_MACOS x_version(9, 0, 0)
#elif !X_IOS && defined(__APPLE__) && defined(__MACH__)
#define X_MACOS x_version(10, 0, 0)
#else
#define X_MACOS (0)
#endif

#if !X_CYGWIN && (defined(_WIN32) || defined(_WIN64) \
  || defined(__TOS_WIN__) || defined(__TOS_WIX__) \
  || defined(__WIN32__) || defined(__WINDOWS__))
#define X_WINDOWS (1)
#else
#define X_WINDOWS (0)
#endif
// Operating_System_Detection}}}

/******************************************************************************
 * @name Platform_Detection{{{
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
#if defined(__MINGW32_VERSION_MAJOR) && defined(__MINGW32_VERSION_MINOR)
#define X_MINGW32 x_version(__MINGW32_VERSION_MAJOR, __MINGW32_VERSION_MINOR, 0)
#else
#define X_MINGW32 (1)
#endif
#else
#define X_MINGW32 (0)
#endif

#if defined(__MINGW64__)
#if defined(__MINGW64_VERSION_MAJOR) && defined(__MINGW64_VERSION_MINOR)
#define X_MINGW64 x_version(__MINGW64_VERSION_MAJOR, __MINGW64_VERSION_MINOR, 0)
#else
#define X_MINGW64 (1)
#endif
#else
#define X_MINGW64 (0)
#endif
// Platform_Detection}}}

// ********************************************************** Configurations{{{
// Feature-test macros must be defined before including any system header.
#if X_LINUX
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#endif

#if X_CYGWIN || X_LINUX
#ifndef _FILE_OFFSET_BITS
// Enable large-file interfaces on supported POSIX targets.
#define _FILE_OFFSET_BITS (64)
#endif
#endif

// Architecture width detection requires <stdint.h>.
#include <stdint.h>
#if INTPTR_MAX == INT64_MAX
#define X_32BIT (0)
#define X_64BIT (1)
#elif INTPTR_MAX == INT32_MAX
#define X_32BIT (1)
#define X_64BIT (0)
#else
#error "Only 32-bit or 64-bit architecture is supported."
#endif

#if !X_MSVC && defined(_MSC_VER)
#define _X_MSVC_COMPAT (1)
#else
#define _X_MSVC_COMPAT (0)
#endif
// Configurations}}}

// ***************************************************************** Headers{{{
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#if X_WINDOWS
#if X_ENABLE_SOCKET
#if X_MSVC || _X_MSVC_COMPAT
#pragma comment(lib, "Ws2_32")
#endif
#include <winsock2.h>
#endif
#include <windows.h>
#include <winternl.h>
#else
#if X_LINUX
#include <sys/sysinfo.h>
#elif X_IOS || X_MACOS
#include <mach/mach.h>
#include <sys/sysctl.h>
#endif
#include <unistd.h>
#endif

#if X_ENABLE_CUDA_DRIVER
#include <cuda.h>
#endif
#if X_ENABLE_CUDA_RUNTIME
#include <cuda_runtime.h>
#endif
#if X_ENABLE_CUBLAS
#include <cublas_v2.h>
#endif
// Headers}}}

/******************************************************************************
 * @name Symbol_Visibility{{{
 * @see [Microsoft Docs](https://docs.microsoft.com/en-us/cpp/cpp/dllexport-dllimport?view=msvc-170)
 *      and [GCC Wiki](https://gcc.gnu.org/wiki/Visibility).
 *****************************************************************************/
#if X_CYGWIN || X_WINDOWS
/// @brief Hidden-visibility annotation is not applied to Cygwin/Windows DLLs.
#define X_HID

/// @brief Mark a symbol exported by the current Cygwin or Windows DLL.
#define X_EXP __declspec(dllexport)

/// @brief Mark a symbol imported from another Cygwin or Windows DLL.
#define X_IMP __declspec(dllimport)
#else
/// @brief Mark a symbol as internal to the current shared library.
#define X_HID __attribute__ ((visibility("hidden")))

/// @brief Mark a symbol exported by the current shared library.
#define X_EXP __attribute__ ((visibility("default")))

/// @brief Mark a symbol imported from another shared library.
/// @note ELF imports use the exported visibility; hidden visibility is for
///       internals.
#define X_IMP __attribute__ ((visibility("default")))
#endif
// Symbol_Visibility}}}

/******************************************************************************
 * @name Miscellaneous{{{
 *****************************************************************************/
/// @brief Unify the function signature macro between compilers.
#if X_MSVC || _X_MSVC_COMPAT
#define X_PRETTY_FUNCTION __FUNCSIG__
#else
#define X_PRETTY_FUNCTION __PRETTY_FUNCTION__
#endif

#ifdef _X_MSVC_COMPAT
#undef _X_MSVC_COMPAT
#endif

#ifdef __cplusplus
#define X_INL inline
#else
#define X_INL static inline
#endif

X_INL const char* _x_filename(const char* file)
{
  const char* sep = strrchr(file, '/');

#if X_WINDOWS
  const char* alt = strrchr(file, '\\');
  if (alt != NULL && (sep == NULL || alt > sep)) {
    sep = alt;
  }
#endif

  return sep == NULL ? file : sep + 1;
}

/// @brief Get the base name from a full path.
/// @note Windows recognizes both `'/'` and `'\\'`; POSIX recognizes only
///       `'/'`.
#define X_FILENAME _x_filename(__FILE__)

// Shared Core helper for Logging and Date and Time.
X_INL const char* _x_timestamp(char* const buffer, const size_t size)
{
  if (buffer == NULL || size == 0) {
    return "";
  }

  buffer[0] = '\0';
  if (size < 26) {
    return buffer;
  }

  time_t now = time(NULL);
  if (now == (time_t)-1) {
    return buffer;
  }

#if X_WINDOWS
  if (ctime_s(buffer, size, &now) != 0) {
    buffer[0] = '\0';
    return buffer;
  }
#else
  if (ctime_r(&now, buffer) == NULL) {
    buffer[0] = '\0';
    return buffer;
  }
#endif

  size_t len = strlen(buffer);
  if (len > 0 && buffer[len - 1] == '\n') {
    buffer[len - 1] = '\0';
  }

  return buffer;
}
// Miscellaneous}}}

/******************************************************************************
 * @name DECL_Error_Handling{{{
 * @brief A collection of error handling utilities.
 *****************************************************************************/
/// @brief Assert an expression and abort on failure.
/// @param ... The required expression followed by an optional C string,
///            `NULL`, or an empty string to omit the message.
/// @note The expression is evaluated once; the message is evaluated once only
///       when the assertion fails. Diagnostic text reflects macro expansion.
#define x_assert(...) \
  _X_ASSERT_SELECT(__VA_ARGS__, _X_ASSERT_TWO, _X_ASSERT_ONE, 0)(__VA_ARGS__)

/// @brief Evaluate a domain status-code expression and report failure.
/// @param domain The nonempty name of a registered error domain.
/// @param error A writable @ref x_error lvalue receiving the result.
/// @param expression A complete expression yielding a domain code fitting
///                   `int32_t`.
/// @note All three arguments are evaluated once. Failures print to `stderr`
///       independently of Logging; Debug diagnostics include source context
///       and the expression text.
///       Status-code failures do not abort.
/// @attention Does not capture `errno`/`GetLastError()` or accept @ref x_error
///            results.
///            APIs returning sentinel values require a failure-checking
///            wrapper. Parenthesize expressions containing macro-visible
///            commas.
#define x_check(domain, error, expression) do { \
  (error) = _x_check_impl( \
      (domain), (int32_t)(expression), \
      X_FILENAME, #expression, (long long)__LINE__); \
} while (false)

/// @brief Total registry capacity, including built-in domains.
/// @note Use the same capacity and feature configuration in every translation
///       unit, and define `X_ERROR_DOMAIN_IMPLEMENTATION` in exactly one of
///       them.
#ifndef X_ERROR_DOMAIN_LIMIT
#define X_ERROR_DOMAIN_LIMIT (16)
#endif

#if X_ERROR_DOMAIN_LIMIT \
  < (2 + !!(X_IOS | X_MACOS) + 3 * !!X_WINDOWS + !!X_ENABLE_SOCKET \
    + !!X_ENABLE_CUDA_DRIVER + !!X_ENABLE_CUDA_RUNTIME + !!X_ENABLE_CUBLAS)
#error "X_ERROR_DOMAIN_LIMIT must accommodate all enabled built-in domains."
#endif

/// @brief Error interpretation and optional capture callbacks for a domain.
/// @note A `NULL` `failed` callback uses `value != 0`; `NULL` `capture` means
///       unsupported. `message` must terminate `buffer` when `buffer != NULL`
///       and `size > 0`.
/// @attention Keep the domain, `name`, and callbacks valid and unchanged while
///            registered or referenced by an @ref x_error.
typedef struct _x_error_domain_
{
  int32_t (*capture)(void);
  bool (*failed)(const int32_t value);
  char* (*message)(char* buffer, const size_t size, const int32_t value);

  const char* name;
} x_error_domain;

typedef struct _x_error_
{
  const x_error_domain* domain;
  int32_t value;
} x_error;

/// @brief Construct a successful error value.
X_INL x_error x_ok(void);

/// @brief Return whether an error value indicates failure.
/// @note A `NULL` `domain` or `failed` callback uses `value != 0`.
/// @note `"wincom"` uses `FAILED()` (so `S_FALSE` is successful); `"winnt"`
///       uses `!NT_SUCCESS()` (warnings count as failure); `"mach"` uses
///       `value != KERN_SUCCESS`. Other built-in domains use nonzero failure.
X_INL bool x_fail(const x_error error);

/// @brief Return whether an error value indicates success.
X_INL bool x_succ(const x_error error);

/// @brief Format an error message into caller-provided storage.
/// @param buffer Writable destination buffer.
/// @param size Capacity in bytes, including the terminator.
/// @return `buffer`. A `NULL` `buffer` or zero `size` performs no write.
/// @note Messages may be truncated to fit the buffer.
///       The `"generic"` domain formats the numeric code without a system
///       message lookup.
X_INL const char* x_error_message(
    char* const buffer, const size_t size, const x_error error);

/// @brief Register a named user-defined error domain.
/// @return 0 on success; `EINVAL` for a `NULL` domain or `NULL`/empty name;
///         `EEXIST` for a duplicate name (including built-ins); `ENOSPC` when
///         full.
/// @attention Register before concurrent access; registration is not
///            synchronized. See @ref x_error_domain for lifetime requirements.
X_INL int32_t x_error_register(const x_error_domain* domain);

/// @brief `x_error_set(name)` captures; `x_error_set(name, value)` sets a code.
/// @note Built-in domains and capture availability:
///       - `"generic"`: always available; capture unsupported.
///       - `"posix"`: always available; captures `errno`.
///       - `"mach"`: iOS/macOS; capture unsupported.
///       - `"win32"`: Windows; captures `GetLastError()`.
///       - `"wincom"` (HRESULT): Windows; capture unsupported.
///       - `"winnt"` (NTSTATUS): Windows; capture unsupported.
///       - `"socket"`: requires `X_ENABLE_SOCKET`; captures
///         `WSAGetLastError()` on Windows and `errno` otherwise.
///       - `"cudadrv"`: requires `X_ENABLE_CUDA_DRIVER`; capture unsupported.
///       - `"cudart"`: requires `X_ENABLE_CUDA_RUNTIME`; captures
///         `cudaGetLastError()`, reading and clearing last-error.
///       - `"cublas"`: requires `X_ENABLE_CUBLAS`; capture unsupported.
/// @note Capture `errno`/Win32/socket last-error state only after API failure;
///       it may be stale after success. CUDA Runtime capture reads the current
///       host thread's last-error in that Runtime instance and resets it to
///       `cudaSuccess`, without synchronizing GPU work.
/// @attention `NULL`/empty/unknown names or unsupported capture requests print
///            a diagnostic and abort.
#define x_error_set(...) \
  _X_ERROR_SELECT( \
      __VA_ARGS__, _x_error_set_value, _x_error_capture, 0)(__VA_ARGS__)
// DECL_Error_Handling}}}

/******************************************************************************
 * @name DECL_Logging{{{
 * @brief A collection of logging utilities.
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

/// @brief Log prefix buffer capacity in bytes, including the terminator.
/// @note Must be at least 1.
#ifndef X_LOG_PREFIX_LIMIT
#ifdef NDEBUG
#define X_LOG_PREFIX_LIMIT (64)
#else
#define X_LOG_PREFIX_LIMIT (256)
#endif
#endif

#if X_LOG_PREFIX_LIMIT < 1
#error "X_LOG_PREFIX_LIMIT must be at least 1."
#endif

/// @brief Log message buffer capacity in bytes, including the terminator.
/// @note Must be at least 1.
#ifndef X_LOG_MSG_LIMIT
#define X_LOG_MSG_LIMIT (256)
#endif

#if X_LOG_MSG_LIMIT < 1
#error "X_LOG_MSG_LIMIT must be at least 1."
#endif

/// @brief Log a message with a specified log level.
/// @param level The log level, one of `'p'`, `'f'`, `'e'`, `'w'`, `'i'`, or
///              `'d'`.
/// @param stream Optional output stream for the log; defaults to `stdout`.
/// @param ... The required format string followed by optional arguments as in
///            `printf()`.
/// @attention The log level is case insensitive, and must be known at compile
///            time.
#define x_log(level, stream, ...) do { \
  _x_log_impl( \
      X_FILENAME, __func__, (long long)__LINE__, \
      (level), (stream), __VA_ARGS__); \
} while (false)
// DECL_Logging}}}

/******************************************************************************
 * @name DECL_Checksums{{{
 * @brief Data checksum utilities.
 *****************************************************************************/
/// @brief Calculate the CRC-32/ISO-HDLC checksum of a data chunk.
/// @note A `NULL` `previous` starts a new calculation. Otherwise, `*previous`
///       is the checksum returned by the preceding calculation. Chunks of any
///       length may be processed incrementally. `data` need not be aligned.
/// @note If `data` is `NULL`, `size` is ignored. The function returns
///       `*previous` when `previous` is non-`NULL`, or 0 otherwise.
X_INL uint32_t x_checksum_crc32(
    const void* data, const size_t size, const uint32_t* previous);

/// @brief Calculate the internet checksum of a data chunk.
/// @see [RFC1071](https://www.rfc-editor.org/info/rfc1071)
/// @note A `NULL` `previous` starts a new calculation. Otherwise, `*previous`
///       is the checksum returned by the preceding calculation. For
///       incremental calculation over concatenated chunks, every chunk except
///       the last must have even length. `data` need not be aligned.
/// @note The result is a platform-independent numeric checksum. Convert it to
///       network byte order before storing it in a network-order 16-bit field
///       (e.g., using `htons()`, declared by the caller's platform headers).
/// @note If `data` is `NULL`, `size` is ignored. The function returns
///       `*previous` when `previous` is non-`NULL`, or `0xFFFF` otherwise.
X_INL uint16_t x_checksum_rfc1071(
    const void* data, const size_t size, const uint16_t* previous);

/// @brief Calculate the XOR checksum of a data chunk.
/// @note A `NULL` `previous` starts a new calculation. Otherwise, `*previous`
///       is the checksum returned by the preceding calculation. Chunks of any
///       length may be processed incrementally. `data` need not be aligned.
/// @note If `data` is `NULL`, `size` is ignored. The function returns
///       `*previous` when `previous` is non-`NULL`, or 0 otherwise.
X_INL uint8_t x_checksum_xor(
    const void* data, const size_t size, const uint8_t* previous);
// DECL_Checksums}}}

/******************************************************************************
 * @name DECL_Date_and_Time{{{
 * @brief A collection of date and time utilities.
 *****************************************************************************/
/// @brief Sleep for a specified amount of time in milliseconds.
/// @note On Windows, `ULONG_MAX` means an infinite wait. The caller is
///       responsible for choosing a finite interval when required.
X_INL void x_sleep(const unsigned long millisecond);

/// @brief Get the current local time as a fixed-format string.
/// @note Function alias to the shared Core helper `_x_timestamp()`.
/// @param buffer Destination buffer to store the timestamp.
/// @param size Buffer capacity in bytes. Requires at least 26 bytes.
/// @return `buffer` when `buffer` is non-`NULL` and `size` is positive;
///         otherwise an empty string literal. Insufficient capacity or a
///         time/conversion failure produces an empty string in a nonempty
///         destination buffer.
/// @note Uses the `ctime()`-style format with the trailing newline removed.
#define x_timestamp _x_timestamp

/// @struct x_event_agg
/// @brief A structure for stopwatch aggregation.
/// @note Initialize with @ref x_event_agg_init. `min` and `max` have
///       measurement meaning only when `cyc` is positive.
/// @var x_event_agg::ready
///      Whether the group has reached its target number of valid samples.
/// @var x_event_agg::cyc
///      The number of valid samples accumulated since initialization/reset.
/// @var x_event_agg::sum
///      The total elapsed time in nanoseconds.
/// @var x_event_agg::avg
///      The average elapsed time in nanoseconds.
/// @var x_event_agg::max
///      Zero-based index and duration in nanoseconds of the maximum sample.
/// @var x_event_agg::min
///      Zero-based index and duration in nanoseconds of the minimum sample.
typedef struct _x_event_agg_ x_event_agg;
struct _x_event_agg_
{
  /// @brief Print the aggregation, or a no-samples message when `cyc` is zero.
  /// @param unit The unit, `"ns"`, `"us"`, `"ms"`, `"s"`, `"m"`, or `"h"`.
  ///        `NULL`, empty, and unknown units use `"ns"`.
  /// @param title A valid null-terminated title, `NULL`, or an empty string.
  ///        `NULL` and empty titles use a timestamp generated at echo time.
  /// @param stream The output stream; `NULL` selects `stdout`.
  void (*echo)(
      x_event_agg* self, const char* unit, const char* title,
      FILE* const stream);

  /// @brief Clear the samples and `ready` flag, retaining the helper
  ///        functions.
  void (*reset)(x_event_agg* self);

  bool ready;
  size_t cyc;
  double sum;
  double avg;
  struct {
    size_t idx;
    double val;
  } max, min;
};

/// @brief Initialize/reset aggregation data and install its helper functions.
X_INL void x_event_agg_init(x_event_agg* self);

/// @struct x_event
/// @brief A CPU or CUDA elapsed-time recorder.
/// @note Measurement operations require successful @ref x_event_init. After a
///       failed initialization or a termination attempt, use @ref x_event_term
///       for any remaining cleanup before reinitializing.
/// @note CPU timing uses a monotonic clock. CUDA @ref x_event::tic and
///       @ref x_event::toc enqueue event records; @ref x_event::dur and
///       @ref x_event::agg wait for the stop event to calculate durations.
/// @note Request-check failures in @ref x_event::tic, @ref x_event::toc, and
///       @ref x_event::agg are returned without changing status or samples.
///       Aggregation-only resets, no-ops, and count overflow also preserve
///       @ref x_event::status. Backend recording/calculation failures
///       update it.
typedef struct _x_event_ x_event;
struct _x_event_
{
  /// @brief Record an endpoint and accumulate one valid sample on success.
  /// @param aggregation The multi-cycle aggregation of the event.
  /// @param cycle Target valid-sample count; keep it fixed within a group.
  ///        Zero resets aggregation without recording. A ready aggregation is
  ///        not updated; reset it to start another group.
  /// @param stream CUDA stream; `NULL` selects the default. Ignored for CPU.
  /// @param flags CUDA recording flags; 0 selects defaults. Ignored for CPU.
  /// @return The result; measurement failures update @ref x_event::status.
  /// @note Failed samples leave aggregation unchanged. Without a start sample,
  ///       reports the recording failure, if any, or `ENODATA`. Call
  ///       @ref x_event::tic for each cycle to measure independent intervals.
  /// @attention Initialize `aggregation` with @ref x_event_agg_init before
  ///            passing a nonzero `cycle`.
  x_error (*agg)(
      x_event* self, x_event_agg* aggregation, size_t cycle,
      void* stream, unsigned int flags);

  /// @brief Calculate the duration between two events.
  /// @param unit The duration unit, `"h"`, `"m"`, `"s"`, `"ms"`, `"us"`, or
  ///        `"ns"`. `NULL` and empty strings default to `"ns"`; unknown units
  ///        fail.
  /// @return The elapsed time, or -1.0 on error in every unit. Invalid event
  ///         or unit requests leave @ref x_event::status unchanged;
  ///         measurement failures are stored in @ref x_event::status.
  /// @note Incomplete samples preserve an existing measurement failure, or
  ///       set `ENODATA`. Invalid CPU frequency sets `EINVAL`; NaN sets
  ///       `EDOM`; negative or infinite durations set `ERANGE`. A successful
  ///       calculation clears the measurement error.
  /// @attention Requires successful @ref x_event::tic and @ref x_event::toc
  ///            for the current measurement.
  double (*dur)(x_event* self, const char* unit);

  /// @brief Record the start of a new measurement.
  /// @param stream CUDA stream; `NULL` selects the default. Ignored for CPU.
  /// @param flags CUDA recording flags; 0 selects defaults. Ignored for CPU.
  /// @return The result; backend recording updates @ref x_event::status.
  /// @note A backend recording attempt invalidates the previous duration,
  ///       including on failure. A successful attempt clears the old error.
  x_error (*tic)(x_event* self, void* stream, const unsigned int flags);

  /// @brief Record the end of the current measurement.
  /// @param stream CUDA stream; `NULL` selects the default. Ignored for CPU.
  /// @param flags CUDA recording flags; 0 selects defaults. Ignored for CPU.
  /// @return The result; backend recording updates @ref x_event::status.
  /// @note Without a start sample, returns the existing measurement failure,
  ///       if any, or `ENODATA`, without changing status. A failed backend
  ///       attempt invalidates the stop sample; @ref x_event::toc may be
  ///       retried.
  /// @attention Requires a successful @ref x_event::tic for the current
  ///            measurement.
  x_error (*toc)(x_event* self, void* stream, const unsigned int flags);

  union {
    struct timespec cpu;
#if X_WINDOWS
    int64_t counter;
#endif
#if X_ENABLE_CUDA_DRIVER
    CUevent cu;
#endif
#if X_ENABLE_CUDA_RUNTIME
    cudaEvent_t cuda;
#endif
  } start, stop;

#if X_WINDOWS
  int64_t frequency;  // QPC counts per second, obtained during initialization.
#endif
  int32_t type;  // For faster branching.
  bool started;
  bool stopped;
  /// @brief Current measurement/backend lifecycle status, not for every call.
  /// @note Success does not by itself imply complete samples; see `started`
  ///       and `stopped`. Initialization, termination, and successful backend
  ///       recording/calculation update this state.
  x_error status;
};

/// @brief Initialize a CPU (`"cpu"`), CUDA driver (`"cudadrv"`), or CUDA
///        runtime (`"cudart"`) event.
/// @return The status, also stored in `self->status` when `self` is
///         non-`NULL`. A failed initialization leaves an object safe to
///         terminate. If rollback destruction fails, @ref x_event_term can
///         retry releasing the remaining handle.
/// @note Release all owned resources successfully before reinitializing.
///       CUDA events must support timing; disable-timing flags are rejected.
///       Creation flags differ from the recording flags accepted by
///       @ref x_event::tic, @ref x_event::toc, and @ref x_event::agg. `type`
///       identifies the backend owning any remaining resources, including
///       after a failed initialization; it is not a success flag.
X_INL x_error x_event_init(
    x_event* self, const char* type, const unsigned int flags);

/// @brief Release event resources and invalidate recorded samples.
/// @return The status, also stored in `self->status` when `self` is
///         non-`NULL`.
/// @note Destruction is attempted for each non-`NULL` CUDA start and stop
///       event handle, even if destroying the other handle fails. Failed
///       handles are retained for a retry. Repeated termination after success
///       is harmless.
X_INL x_error x_event_term(x_event* self);
// DECL_Date_and_Time}}}

/******************************************************************************
 * @name DECL_File_System{{{
 * @brief A collection of file system utilities.
 *****************************************************************************/
/// @brief Query if a file or directory exists.
/// @param file The file or directory to query.
/// @return `true` if the status query succeeds, `false` otherwise.
/// @note A failed query does not necessarily mean the path is absent.
X_INL bool x_fexist(const char* file);

/// @brief Open a file stream with error handling.
/// @param stream Writable pointer receiving the opened stream.
/// @param file A nonempty, null-terminated file path.
/// @param mode A nonempty, null-terminated access mode supported by the
///             platform's C runtime library.
/// @return Success, or an error in the `"posix"` domain.
/// @note A `NULL` `stream` or `NULL`/empty `file`/`mode` returns `EINVAL`
///       without changing `*stream`. Other failures may change it; use it
///       only after success.
/// @attention Mode syntax is not validated. Unsupported Windows modes may
///            invoke the CRT invalid parameter handler (terminates by
///            default).
X_INL x_error x_fopen(FILE** stream, const char* file, const char* mode);

/// @brief Get the absolute path of a file or directory.
/// @param destination Writable destination buffer.
/// @param size Destination capacity in bytes, including the terminator.
/// @param source A nonempty, null-terminated source path.
/// @return `destination` on success, or `NULL` on failure with `errno` set.
/// @note Invalid arguments set `EINVAL`; insufficient capacity sets `ENOBUFS`.
///       The destination is unchanged on failure.
/// @note Windows `_fullpath()` does not require existence or resolve symlinks;
///       POSIX `realpath()` requires an existing target and resolves symlinks.
X_INL const char* x_fpath(
    char* destination, const size_t size, const char* source);

/// @brief Get the size of a file.
/// @param file The file to query.
/// @return The size of the file. If an error occurs, the return value is -1.
X_INL int64_t x_fsize(const char* file);
// DECL_File_System}}}

/******************************************************************************
 * @name DECL_Hardware{{{
 * @brief A collection of hardware utilities.
 *****************************************************************************/
/// @brief Get an online logical-processor count.
/// @return A positive count, or 0 if the query fails or yields no processors.
/// @note This is provided as a thread-count hint. It does not account for
///       affinity or CPU quotas and does not guarantee an optimal thread
///       count. Older Windows targets query only the current processor group.
///       Using all processor groups may require explicit thread placement on
///       older Windows versions.
/// @attention The caller must handle a zero result and choose an appropriate
///            fallback before using the result as a thread count.
X_INL size_t x_ncpu(void);
// DECL_Hardware}}}

/******************************************************************************
 * @name DECL_Mathematics{{{
 * @brief A collection of mathematical utilities.
 *****************************************************************************/
/// @brief Constant PI of user-defined type.
/// @var x_Pi
/// @param T The user-defined type.
/// @return The constant PI.
#define x_Pi(T) ((T)3.141592653589793238462643383279502884197169399375L)

/// @brief Kibibyte constant generator, i.e., 1 KiB = 1024 bytes.
/// @param scale The scaling factor.
/// @note Capacity macros evaluate `scale` once and do not check overflow.
///       Choose `T` to represent the unit constant and the scaled result;
///       multiplication follows the usual arithmetic conversions, including
///       integer promotion.
#define x_KiB(T, scale) ((T)(scale) * (T)1024)

/// @brief Mebibyte constant generator, i.e., 1 MiB = 1048576 bytes.
/// @param scale The scaling factor.
#define x_MiB(T, scale) ((T)(scale) * (T)1048576)

/// @brief Gibibyte constant generator, i.e., 1 GiB = 1073741824 bytes.
/// @param scale The scaling factor.
#define x_GiB(T, scale) ((T)(scale) * (T)1073741824)

/// @brief Tebibyte constant generator, i.e., 1 TiB = 1099511627776 bytes.
/// @param scale The scaling factor.
#define x_TiB(T, scale) ((T)(scale) * (T)1099511627776)

/// @brief Pebibyte constant generator, i.e., 1 PiB = 1125899906842624 bytes.
/// @param scale The scaling factor.
#define x_PiB(T, scale) ((T)(scale) * (T)1125899906842624)

/// @brief Generate a mask with only the specified bit set.
/// @param bit The zero-based bit index, which must be in [0, 63].
/// @return A `uint64_t` bit mask.
/// @note A constant-expression index yields a constant expression; a `const`
///       variable alone is not a constant expression in C99/C11.
/// @attention The index is not checked and `errno` is not changed.
/// @see C++'s `std::bitset` for a more versatile solution.
#define x_bit(bit) (UINT64_C(1) << (bit))

/// @brief Calculate the greatest common divisor of two integers.
/// @param m The first integer.
/// @param n The second integer.
/// @return The greatest common divisor; `gcd(0, 0)` is defined here as 0.
/// @note Does not modify `errno`.
X_INL uint64_t x_gcd(const uint64_t m, const uint64_t n);

/// @brief Calculate the least common multiple of two integers.
/// @param m The first integer.
/// @param n The second integer.
/// @return The least common multiple, or 0 if either argument is 0.
/// @note An unrepresentable result returns 0 and sets `errno` to `ERANGE`.
///       Success leaves `errno` unchanged; clear it first to distinguish a
///       valid zero.
X_INL uint64_t x_lcm(const uint64_t m, const uint64_t n);

/// @brief Round up to an integer power of a base, including `base^0 == 1`.
/// @param base The base, which must be at least 2.
/// @param source The source number.
/// @return The smallest power greater than or equal to source; 0 yields 1.
/// @note Invalid `base` returns 0 and sets `errno` to `EINVAL`, even for
///       `source == 0`. An unrepresentable result returns 0 and sets `errno`
///       to `ERANGE`. Success leaves `errno` unchanged.
X_INL uint64_t x_next_exp(const uint64_t base, const uint64_t source);

/// @brief Calculate the next multiple of a base.
/// @param base The base, which must be positive.
/// @param source The source number.
/// @return The smallest multiple strictly greater than source, including when
///         `source` is already a multiple. A zero `source` yields `base`.
/// @note Invalid `base` returns 0 and sets `errno` to `EINVAL`. An
///       unrepresentable result returns 0 and sets `errno` to `ERANGE`.
///       Success leaves `errno` unchanged.
X_INL uint64_t x_next_mul(const uint64_t base, const uint64_t source);

/// @brief Round down to an integer power of a base, including `base^0 == 1`.
/// @param base The base, which must be at least 2.
/// @param source The source number.
/// @return The largest power less than or equal to source; 0 yields 0.
/// @note Invalid `base` returns 0 and sets `errno` to `EINVAL`, even for
///       `source == 0`. Success leaves `errno` unchanged; clear it first to
///       distinguish a valid zero from failure.
X_INL uint64_t x_prev_exp(const uint64_t base, const uint64_t source);

/// @brief Calculate the previous multiple of a base.
/// @param base The base, which must be positive.
/// @param source The source number.
/// @return The largest multiple less than or equal to source.
/// @note Invalid `base` returns 0 and sets `errno` to `EINVAL`. Success leaves
///       `errno` unchanged; clear it first to distinguish a valid zero from
///       failure.
X_INL uint64_t x_prev_mul(const uint64_t base, const uint64_t source);
// DECL_Mathematics}}}

/******************************************************************************
 * @name DECL_Memory{{{
 * @brief A collection of memory management utilities.
 *****************************************************************************/
/// @brief Get number of items in an array.
/// @param array An array object, regardless of its storage duration.
/// @attention Pointers, including array function parameters (which are
///            pointers), are not supported. This macro does not reliably
///            detect pointer arguments.
#define x_count(array) \
  ((sizeof(array) / sizeof(*(array))) \
    / (size_t)(!(sizeof(array) % sizeof(*(array)))))

/// @brief Free a memory block allocated on the heap and set it to `NULL`.
/// @param pointer A writable object-pointer lvalue whose non-`NULL` value is
///                valid to pass to `free()`.
/// @note The argument may be evaluated multiple times and must have no side
///       effects. `ptr`, `obj.ptr`, and `ptrs[i]` are supported;
///       `ptrs[i++]` is not.
/// @remark If the pointer is `NULL`, this macro does nothing.
#define x_free(pointer) do { \
  if ((pointer) != NULL) { \
    free((void*)(pointer)); \
    (pointer) = NULL; \
  } \
} while (false)

/// @brief Wrapper of `malloc()` with error handling.
/// @param pointer Pointer to a writable `void*` object initialized to `NULL`.
/// @param size Number of bytes to allocate; must be positive.
/// @return Success, or an error in the `posix` domain.
/// @note A `NULL` `pointer`, a non-`NULL` `*pointer`, or a zero `size` returns
///       `EINVAL` without changing the output. Allocation failure leaves
///       `*pointer` `NULL` and captures the allocator's `errno`.
/// @attention The function reads `*pointer` before allocating. For typed
///            pointers, receive into a `void*` temporary and then assign it to
///            the typed pointer; casting `T**` to `void**` does not make
///            access to a `T*` object through `void**` portable.
X_INL x_error x_malloc(void** pointer, const size_t size);

/// @brief Wrapper of `memcpy()` with error handling.
/// @param destination Destination buffer.
/// @param source Source buffer.
/// @param size Number of bytes to copy.
/// @return Success, or `EINVAL` in the `posix` domain when `size` is nonzero
///         and either pointer is `NULL` or the ranges overlap.
/// @note A zero `size` succeeds without accessing either buffer; both pointers
///       may be `NULL` in that case. The destination is unchanged on failure.
/// @attention Provide readable/writable ranges of at least `size` bytes;
///            pointer validity and capacity are not checked. Identical
///            pointers are rejected. Overlap detection assumes `uintptr_t`
///            byte-address ordering.
X_INL x_error x_memcpy(void* destination, const void* source, const size_t size);

/// @brief Query size available and total memories on CPU or GPU.
/// @param type Can be `"cpu"`, `"cudadrv"` (via CUDA driver API), or
///             `"cudart"` (via CUDA runtime API).
/// @param available Size of available memory.
/// @param total Size of total memory.
/// @return An instance of @ref x_error.
/// @note Either output may be `NULL`, but not both. Outputs are unchanged on
///       query failure. Unknown types and disabled backends return `ENOTSUP`.
/// @note CPU queries support Linux, iOS, macOS, and Windows. Other platforms
///       return `ENOTSUP`; an unrepresentable byte count returns `ERANGE`.
/// @note GPU queries use the current CUDA context/device. Driver queries
///       require prior successful `cuInit(0)` and a current context.
/// @note CPU available is a system-wide snapshot, not an allocation guarantee:
///       Linux `freeram`, iOS/macOS `free_count` * page size (includes
///       speculative pages), Windows `ullAvailPhys`. Mach failures use the
///       `mach` domain; `sysctlbyname()` uses `posix`.
X_INL x_error x_meminfo(const char* type, size_t* available, size_t* total);
// DECL_Memory}}}

/******************************************************************************
 * @name DECL_String{{{
 * @brief A collection of string utilities.
 *****************************************************************************/
/// @brief Copy a string with truncation and argument checking.
/// @param destination Writable destination buffer of at least `size` bytes.
/// @param size Destination capacity in bytes, including the terminator.
/// @param source A valid null-terminated source string.
/// @return Success, or `EINVAL` in the `posix` domain for a `NULL` pointer,
///         zero capacity, or overlapping source and destination write ranges.
/// @note Copies at most `size - 1` characters and appends a terminator.
///       Truncation is successful. The destination is unchanged on failure.
/// @attention The destination write range and full source string must not
///            overlap, including terminators and identical pointers. Pointer
///            validity and capacity are not checked. Detection assumes
///            `uintptr_t` byte-address ordering.
X_INL x_error x_strcpy(char* destination, const size_t size, const char* source);

/// @brief Check if a string is empty.
/// @return `true` for `NULL` or a string whose first character is `'\0'`.
/// @note A non-`NULL` argument must point to a valid null-terminated string.
X_INL bool x_strmty(const char* string);
// DECL_String}}}

/******************************************************************************
 * @name DECL_CUDA{{{
 * @brief A collection of CUDA utilities.
 *****************************************************************************/
#if (X_ENABLE_CUDA_DRIVER || X_ENABLE_CUDA_RUNTIME)
/// @brief Query the type of a block of memory.
/// @param api The API to which the memory is corresponding. Must be
///            `"cudadrv"` (CUDA driver) or `"cudart"` (CUDA runtime).
/// @param pointer The memory. This argument is always required and must be:
///                exactly `CUdeviceptr` for `"cudadrv"` or `void*` for
///                `"cudart"`.
/// @return The underlying API's memory type name, `"Unknown"` on query failure
///         or an unrecognized memory type, or `"N/A"` for a `NULL`/empty
///         `api`, an unknown `api`, or a disabled backend.
/// @note Driver queries require prior successful `cuInit(0)` and unified
///       virtual addressing as required by `cuPointerGetAttribute()`. Driver
///       and runtime memory classifications are not normalized to each other.
/// @attention Variadic arguments do not convert typed pointers to `void*`.
///            For `"cudadrv"`, cast a zero address to `CUdeviceptr` rather
///            than passing an integer literal or pointer. For `"cudart"`,
///            explicitly cast object pointers (including `const` pointers and
///            `NULL`/`nullptr`) to `void*` before passing them. The memory is
///            not modified.
X_INL const char* x_memtype(const char* api, ... /* CUdeviceptr or void* */);

/// @brief Query the number of GPU devices.
/// @param api `"cudadrv"` (CUDA driver) or `"cudart"` (CUDA runtime).
/// @return The device count, or 0 on query failure, a `NULL`/empty or unknown
///         `api`, or a disabled backend. Query failures are reported to
///         `stderr`.
/// @note For `"cudadrv"`, the caller must first successfully call `cuInit(0)`.
X_INL size_t x_ngpu(const char* api);
#endif
// DECL_CUDA}}}

// ***************************************************** IMPL_Error_Handling{{{
// x_error_common{{{
X_INL char* _x_error_copy(char* buffer, const size_t size, const char* text)
{
  if (buffer == NULL || size == 0) {
    return buffer;
  }

  const size_t len = strlen(text);
  const size_t cnt = len < size - 1 ? len : size - 1;
  if (cnt > 0) {
    memmove(buffer, text, cnt);
  }

  buffer[cnt] = '\0';

  return buffer;
}

X_INL char* _x_error_number(
    char* buffer, const size_t size, const char* prefix, const int32_t value)
{
  if (buffer == NULL || size == 0) {
    return buffer;
  }

  // Format numeric fallbacks in sufficiently large storage, then truncate
  // explicitly. Small caller buffers are valid, including under `-Werror`.
  // Longest built-in prefix + space + INT32_MIN + terminator needs 30 bytes.
  char msg[32] = {0};
  snprintf(msg, sizeof(msg), "%s %d", prefix, value);

  return _x_error_copy(buffer, size, msg);
}

X_INL bool _x_error_failed_nonzero(const int32_t value)
{
  return value != 0;
}
// x_error_common}}}

// x_error_generic{{{
X_INL char* _x_error_message_generic(
    char* buffer, const size_t size, const int32_t value)
{
  return _x_error_number(buffer, size, "Generic error", value);
}

static const x_error_domain error_domain_generic = {
  NULL,
  _x_error_failed_nonzero,
  _x_error_message_generic,
  "generic"
};
// x_error_generic}}}

// x_error_posix{{{
X_INL int32_t _x_error_capture_posix(void)
{
  return (int32_t)errno;
}

X_INL char* _x_error_message_posix(
    char* buffer, const size_t size, const int32_t value)
{
  if (buffer == NULL || size == 0) {
    return buffer;
  }

#if X_WINDOWS
  char msg[256] = {0};
  errno_t perr = strerror_s(msg, sizeof(msg), value);
  if (perr != 0 || msg[0] == '\0') {
    _x_error_number(buffer, size, "Unknown error", value);
  } else {
    _x_error_copy(buffer, size, msg);
  }
#else
  buffer[0] = '\0';
#if defined(_GNU_SOURCE) && (defined(__GLIBC__) || X_CYGWIN)
  // GNU strerror_r may return immutable text instead of using buffer.
  const char* msg = strerror_r(value, buffer, size);
  if (msg == NULL || msg[0] == '\0') {
    _x_error_number(buffer, size, "Unknown error", value);
  } else if (msg != buffer) {
    _x_error_copy(buffer, size, msg);
  }
#else
  int perr = strerror_r(value, buffer, size);
  // An XSI ERANGE result need not leave a terminated buffer.
  buffer[size - 1] = '\0';
  if ((perr != 0 && perr != ERANGE) || buffer[0] == '\0') {
    _x_error_number(buffer, size, "Unknown error", value);
  }
#endif
#endif

  return buffer;
}

static const x_error_domain error_domain_posix = {
  _x_error_capture_posix,
  _x_error_failed_nonzero,
  _x_error_message_posix,
  "posix"
};
// x_error_posix}}}

// x_error_mach{{{
#if X_IOS || X_MACOS
X_INL bool _x_error_failed_mach(const int32_t value)
{
  return (kern_return_t)value != KERN_SUCCESS;
}

X_INL char* _x_error_message_mach(
    char* buffer, const size_t size, const int32_t value)
{
  if (buffer == NULL || size == 0) {
    return buffer;
  }

  const char* msg = mach_error_string((mach_error_t)value);
  if (msg == NULL || msg[0] == '\0') {
    return _x_error_number(buffer, size, "Mach error", value);
  }

  return _x_error_copy(buffer, size, msg);
}

static const x_error_domain error_domain_mach = {
  NULL,
  _x_error_failed_mach,
  _x_error_message_mach,
  "mach"
};
#endif  // X_IOS || X_MACOS
// x_error_mach}}}

// x_error_windows{{{
#if X_WINDOWS
X_INL DWORD _x_error_message_windows(
    char* buffer, const size_t size, const DWORD flags, const void* source,
    const DWORD value)
{
  DWORD len = FormatMessageA(
      flags | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK,
      source, value, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), buffer,
      (DWORD)size, NULL);
  while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
    buffer[--len] = '\0';
  }

  return len;
}

X_INL int32_t _x_error_capture_win32(void)
{
  return (int32_t)GetLastError();
}

X_INL char* _x_error_message_win32(
    char* buffer, const size_t size, const int32_t value)
{
  if (buffer == NULL || size == 0) {
    return buffer;
  }

  DWORD len = _x_error_message_windows(
      buffer, size, FORMAT_MESSAGE_FROM_SYSTEM, NULL, (DWORD)value);
  if (len == 0) {
    _x_error_number(buffer, size, "Win32 error", value);
  }

  return buffer;
}

static const x_error_domain error_domain_win32 = {
  _x_error_capture_win32,
  _x_error_failed_nonzero,
  _x_error_message_win32,
  "win32"
};

X_INL bool _x_error_failed_wincom(const int32_t value)
{
  return FAILED((HRESULT)value);
}

X_INL char* _x_error_message_wincom(
    char* buffer, const size_t size, const int32_t value)
{
  if (buffer == NULL || size == 0) {
    return buffer;
  }

  DWORD len = _x_error_message_windows(
      buffer, size, FORMAT_MESSAGE_FROM_SYSTEM, NULL, (DWORD)value);
  if (len == 0 && HRESULT_FACILITY((HRESULT)value) == FACILITY_WIN32) {
    len = _x_error_message_windows(
        buffer, size, FORMAT_MESSAGE_FROM_SYSTEM, NULL,
        (DWORD)HRESULT_CODE((HRESULT)value));
  }
  if (len == 0) {
    char msg[32] = {0};
    snprintf(msg, sizeof(msg), "HRESULT 0x%08x", (unsigned int)(uint32_t)value);
    _x_error_copy(buffer, size, msg);
  }

  return buffer;
}

static const x_error_domain error_domain_wincom = {
  NULL,
  _x_error_failed_wincom,
  _x_error_message_wincom,
  "wincom"
};

X_INL bool _x_error_failed_winnt(const int32_t value)
{
  return !NT_SUCCESS((NTSTATUS)value);
}

X_INL char* _x_error_message_winnt(
    char* buffer, const size_t size, const int32_t value)
{
  if (buffer == NULL || size == 0) {
    return buffer;
  }

  HMODULE module = GetModuleHandleA("ntdll.dll");
  if (module == NULL
      || _x_error_message_windows(
        buffer, size, FORMAT_MESSAGE_FROM_HMODULE, module, (DWORD)value) == 0) {
    char msg[32] = {0};
    snprintf(msg, sizeof(msg), "NTSTATUS 0x%08x", (unsigned int)(uint32_t)value);
    _x_error_copy(buffer, size, msg);
  }

  return buffer;
}

static const x_error_domain error_domain_winnt = {
  NULL,
  _x_error_failed_winnt,
  _x_error_message_winnt,
  "winnt"
};
#endif  // X_WINDOWS
// x_error_windows}}}

// x_error_socket{{{
#if X_ENABLE_SOCKET
X_INL int32_t _x_error_capture_socket(void)
{
#if X_WINDOWS
  return (int32_t)WSAGetLastError();
#else
  return (int32_t)errno;
#endif
}

X_INL char* _x_error_message_socket(
    char* buffer, const size_t size, const int32_t value)
{
#if X_WINDOWS
  return _x_error_message_win32(buffer, size, value);
#else
  return _x_error_message_posix(buffer, size, value);
#endif
}

static const x_error_domain error_domain_socket = {
  _x_error_capture_socket,
  _x_error_failed_nonzero,
  _x_error_message_socket,
  "socket"
};
#endif  // X_ENABLE_SOCKET
// x_error_socket}}}

// x_error_cuda_driver{{{
#if X_ENABLE_CUDA_DRIVER
X_INL bool _x_error_failed_cuda_driver(const int32_t value)
{
  return (CUresult)value != CUDA_SUCCESS;
}

X_INL char* _x_error_message_cuda_driver(
    char* buffer, const size_t size, const int32_t value)
{
  if (buffer == NULL || size == 0) {
    return buffer;
  }

  const char* msg = NULL;
  CUresult cres = cuGetErrorString((CUresult)value, &msg);
  if (cres != CUDA_SUCCESS || msg == NULL) {
    _x_error_number(buffer, size, "CUDA driver error", value);
    return buffer;
  }

  _x_error_copy(buffer, size, msg);

  return buffer;
}

static const x_error_domain error_domain_cuda_driver = {
  NULL,
  _x_error_failed_cuda_driver,
  _x_error_message_cuda_driver,
  "cudadrv"
};
#endif  // X_ENABLE_CUDA_DRIVER
// x_error_cuda_driver}}}

// x_error_cuda_runtime{{{
#if X_ENABLE_CUDA_RUNTIME
X_INL int32_t _x_error_capture_cuda_runtime(void)
{
  return (int32_t)cudaGetLastError();
}

X_INL bool _x_error_failed_cuda_runtime(const int32_t value)
{
  return (cudaError_t)value != cudaSuccess;
}

X_INL char* _x_error_message_cuda_runtime(
    char* buffer, const size_t size, const int32_t value)
{
  if (buffer == NULL || size == 0) {
    return buffer;
  }

  const char* msg = cudaGetErrorString((cudaError_t)value);
  _x_error_copy(buffer, size, msg == NULL ? "CUDA runtime error" : msg);

  return buffer;
}

static const x_error_domain error_domain_cuda_runtime = {
  _x_error_capture_cuda_runtime,
  _x_error_failed_cuda_runtime,
  _x_error_message_cuda_runtime,
  "cudart"
};
#endif  // X_ENABLE_CUDA_RUNTIME
// x_error_cuda_runtime}}}

// x_error_cublas{{{
#if X_ENABLE_CUBLAS
X_INL bool _x_error_failed_cublas(const int32_t value)
{
  return (cublasStatus_t)value != CUBLAS_STATUS_SUCCESS;
}

X_INL char* _x_error_message_cublas(
    char* buffer, const size_t size, const int32_t value)
{
  if (buffer == NULL || size == 0) {
    return buffer;
  }

  const char* msg = cublasGetStatusString((cublasStatus_t)value);
  _x_error_copy(buffer, size, msg == NULL ? "cuBLAS error" : msg);

  return buffer;
}

static const x_error_domain error_domain_cublas = {
  NULL,
  _x_error_failed_cublas,
  _x_error_message_cublas,
  "cublas"
};
#endif  // X_ENABLE_CUBLAS
// x_error_cublas}}}

typedef struct _x_error_domain_registry_
{
  const x_error_domain* list[X_ERROR_DOMAIN_LIMIT];
  size_t count;
} x_error_domain_registry;

#ifdef X_ERROR_DOMAIN_IMPLEMENTATION
x_error_domain_registry error_domain_registry = {
  {
    &error_domain_generic,
    &error_domain_posix,
#if X_IOS || X_MACOS
    &error_domain_mach,
#endif
#if X_WINDOWS
    &error_domain_win32,
    &error_domain_wincom,
    &error_domain_winnt,
#endif
#if X_ENABLE_SOCKET
    &error_domain_socket,
#endif
#if X_ENABLE_CUDA_DRIVER
    &error_domain_cuda_driver,
#endif
#if X_ENABLE_CUDA_RUNTIME
    &error_domain_cuda_runtime,
#endif
#if X_ENABLE_CUBLAS
    &error_domain_cublas,
#endif
  },
  (size_t)(2 + !!(X_IOS | X_MACOS) + 3 * !!X_WINDOWS + !!X_ENABLE_SOCKET
      + !!X_ENABLE_CUDA_DRIVER + !!X_ENABLE_CUDA_RUNTIME + !!X_ENABLE_CUBLAS)
};
#else
extern x_error_domain_registry error_domain_registry;
#endif

// x_error_domain{{{
X_INL const x_error_domain* _x_error_domain_lookup(const char* name)
{
  if (name == NULL || name[0] == '\0') {
    return NULL;
  }

  const x_error_domain* domain = NULL;
  for (size_t i = 0; i < error_domain_registry.count; ++i) {
    domain = error_domain_registry.list[i];
    if (domain != NULL && domain->name != NULL
        && strcmp(name, domain->name) == 0) {
      return domain;
    }
  }

  return NULL;
}

X_INL const x_error_domain* _x_error_domain_require(const char* name)
{
  const x_error_domain* domain = _x_error_domain_lookup(name);
  if (domain == NULL) {
    fprintf(
        stderr, "Unknown error domain: \"%s\".\n",
        name == NULL ? "(null)" : name);
    abort();
  }

  return domain;
}

X_INL char* _x_error_domain_message(
    char* buffer, const size_t size, const int32_t value,
    const x_error_domain* domain)
{
  if (buffer == NULL || size == 0) {
    return buffer;
  }

  if (domain == NULL || domain->message == NULL) {
    return _x_error_number(buffer, size, "Unknown error", value);
  }

  return domain->message(buffer, size, value);
}
// x_error_domain}}}

// x_error_set{{{
#define _X_ERROR_SELECT(_1, _2, function, ...) function

X_INL x_error _x_error_set_domain(
    const x_error_domain* domain, const int32_t value)
{
  x_error err = {domain == NULL ? &error_domain_generic : domain, value};
  return err;
}

X_INL x_error _x_error_set_value(const char* name, const int32_t value)
{
  return _x_error_set_domain(_x_error_domain_require(name), value);
}
// x_error_set}}}

// x_error_capture{{{
X_INL x_error _x_error_capture(const char* name)
{
  const x_error_domain* domain = _x_error_domain_require(name);
  if (domain->capture == NULL) {
    fprintf(
        stderr, "Error domain \"%s\" does not support error capturing.\n",
        domain->name);
    abort();
  }

  return _x_error_set_domain(domain, domain->capture());
}
// x_error_capture}}}

X_INL x_error x_ok(void)
{
  return _x_error_set_domain(&error_domain_posix, 0);
}

X_INL bool x_fail(const x_error error)
{
  if (error.domain != NULL && error.domain->failed != NULL) {
    return error.domain->failed(error.value);
  } else {
    return error.value != 0;
  }
}

X_INL bool x_succ(const x_error error)
{
  return !x_fail(error);
}

X_INL const char* x_error_message(
    char* const buffer, const size_t size, const x_error error)
{
  if (buffer == NULL || size == 0) {
    return buffer;
  }

  const x_error_domain* domain = error.domain;
  if (domain == NULL) {
    domain = &error_domain_generic;
  }

  _x_error_domain_message(buffer, size, error.value, domain);

  return buffer;
}

X_INL int32_t x_error_register(const x_error_domain* domain)
{
  if (domain == NULL || domain->name == NULL || domain->name[0] == '\0') {
    return EINVAL;
  }

  for (size_t i = 0; i < error_domain_registry.count; ++i) {
    if (strcmp(error_domain_registry.list[i]->name, domain->name) == 0) {
      return EEXIST;
    }
  }

  if (error_domain_registry.count >= X_ERROR_DOMAIN_LIMIT) {
    return ENOSPC;
  }

  error_domain_registry.list[error_domain_registry.count] = domain;
  error_domain_registry.count += 1;

  return 0;
}

// Support optional assertion messages in strict C99/C11. The trailing dummy
// argument in x_assert keeps the selector's variadic tail nonempty.
#define _X_ASSERT_SELECT(_1, _2, function, ...) function
#define _X_ASSERT_ONE(expression) _X_ASSERT_TWO(expression, NULL)
#define _X_ASSERT_TWO(expression, message) do { \
  if (!(expression)) { \
    _x_assert_fail( \
        #expression, (message), X_FILENAME, (long long)__LINE__, \
        X_PRETTY_FUNCTION); \
  } \
} while (false)

X_INL void _x_assert_fail(
    const char* expression, const char* message,
    const char* file, const long long line, const char* function)
{
  fprintf(stderr, "Assertion failed: %s\n", expression);

  if (message != NULL && message[0] != '\0') {
    fprintf(stderr, "Message: %s\n", message);
  }

  fprintf(stderr, "Position: %s:%lld: %s\n", file, line, function);

  abort();
}

X_INL x_error _x_check_impl(
    const char* domain, const int32_t value,
    const char* file, const char* expression, const long long line)
{
  x_error err = _x_error_set_value(domain, value);
  if (x_fail(err)) {
    char msg[256] = {0};
    x_error_message(msg, sizeof(msg), err);
#ifdef NDEBUG
    (void)file;
    (void)expression;
    (void)line;
    fprintf(stderr, "Check failed: %s\n", msg);
#else
    fprintf(
        stderr, "Check failed: %s\nPosition: %s:%lld: %s\n",
        msg, file, line, expression);
#endif
  }

  return err;
}
// IMPL_Error_Handling}}}

// ************************************************************ IMPL_Logging{{{
#define _X_COLOR_WHITE   "\033[97m"
#define _X_COLOR_RED     "\033[91m"
#define _X_COLOR_GREEN   "\033[92m"
#define _X_COLOR_YELLOW  "\033[93m"
#define _X_COLOR_MAGENTA "\033[95m"
#define _X_COLOR_CYAN    "\033[96m"
#define _X_COLOR_RESET   "\033[0m"

X_INL void _x_log_prefix(
    char* buffer, const size_t size, const unsigned char level,
    const char* file, const char* function, const long long line)
{
  char ts[26] = {0};

#ifdef NDEBUG
  (void)file;
  (void)function;
  (void)line;
  snprintf(
      buffer, size, "[%c %s] ",
      toupper(level), _x_timestamp(ts, sizeof(ts)));
#else
  snprintf(
      buffer, size, "[%c %s | %s - %s - %lld] ",
      toupper(level), _x_timestamp(ts, sizeof(ts)), file, function, line);
#endif
}

X_INL void _x_log_impl(
    const char* file, const char* function, const long long line,
    const unsigned char level, FILE* stream, const char* format, ...)
{
  int lvl = X_LOG_NONE;
  const char* color = "";

  switch (level) {
    case 'P':
    case 'p':
      lvl = X_LOG_PLAIN;
      color = _X_COLOR_WHITE;
      break;
    case 'F':
    case 'f':
      lvl = X_LOG_FATAL;
      color = _X_COLOR_MAGENTA;
      break;
    case 'E':
    case 'e':
      lvl = X_LOG_ERROR;
      color = _X_COLOR_RED;
      break;
    case 'W':
    case 'w':
      lvl = X_LOG_WARNING;
      color = _X_COLOR_YELLOW;
      break;
    case 'I':
    case 'i':
      lvl = X_LOG_INFO;
      color = _X_COLOR_GREEN;
      break;
    case 'D':
    case 'd':
      lvl = X_LOG_DEBUG;
      color = _X_COLOR_CYAN;
      break;
    default:
      return;
  }

  if (lvl > X_LOG_LEVEL) {
    return;
  }

  char prefix[X_LOG_PREFIX_LIMIT] = {0};
  _x_log_prefix(prefix, sizeof(prefix), level, file, function, line);

  char msg[X_LOG_MSG_LIMIT] = {0};
  va_list args;
  va_start(args, format);
  vsnprintf(msg, sizeof(msg), format, args);
  va_end(args);

  FILE* output = stream == NULL ? stdout : stream;
  if (output == stdout || output == stderr) {
    fprintf(output, "%s%s%s%s\n", color, prefix, msg, _X_COLOR_RESET);
  } else {
    fprintf(output, "%s%s\n", prefix, msg);
  }
}
// IMPL_Logging}}}

// ********************************************************** IMPL_Checksums{{{
X_INL uint32_t x_checksum_crc32(
    const void* data, const size_t size, const uint32_t* previous)
{
  const uint8_t* bytes = (const uint8_t*)data;
  uint32_t cks = previous == NULL ? 0xFFFFFFFFu : ~(*previous);

  if (bytes == NULL) {
    return ~cks;
  }

  for (size_t i = 0; i < size; ++i) {
    cks ^= bytes[i];
    for (size_t bit = 0; bit < 8; ++bit) {
      cks = (cks >> 1) ^ (-(int32_t)(cks & 1) & 0xEDB88320);
    }
  }

  return ~cks;
}

X_INL uint16_t x_checksum_rfc1071(
    const void* data, const size_t size, const uint16_t* previous)
{
  const uint8_t* bytes = (const uint8_t*)data;
  uint32_t cks = previous == NULL ? 0u : (uint16_t)~(*previous);

  if (bytes == NULL) {
    return (uint16_t)~cks;
  }

  size_t idx = 0;

  while (size - idx >= 2) {
    cks += ((uint32_t)bytes[idx] << 8) | bytes[idx + 1];
    cks = (cks & 0xFFFF) + (cks >> 16);
    idx += 2;
  }

  if (idx < size) {
    cks += (uint32_t)bytes[idx] << 8;
  }

  while ((cks >> 16) != 0) {
    cks = (cks & 0xFFFF) + (cks >> 16);
  }

  return (uint16_t)(~cks);
}

X_INL uint8_t x_checksum_xor(
    const void* data, const size_t size, const uint8_t* previous)
{
  const uint8_t* bytes = (const uint8_t*)data;
  uint64_t cks = previous == NULL ? 0u : (uint64_t)(*previous);

  if (bytes == NULL) {
    return (uint8_t)cks;
  }

  uint64_t blk[8] = {0};
  const size_t bsz = sizeof(blk);
  size_t idx = 0;

  while (size - idx >= bsz) {
    memcpy(blk, bytes + idx, sizeof(blk));

    cks ^= blk[0] ^ blk[1] ^ blk[2] ^ blk[3]
        ^  blk[4] ^ blk[5] ^ blk[6] ^ blk[7];

    idx += sizeof(blk);
  }

  while (idx < size) {
    cks ^= bytes[idx];
    idx += 1;
  }

  cks ^= cks >> 32;
  cks ^= cks >> 16;
  cks ^= cks >> 8;

  return (uint8_t)cks;
}
// IMPL_Checksums}}}

// ****************************************************** IMPL_Date_and_Time{{{
X_INL void x_sleep(const unsigned long millisecond)
{
#if X_WINDOWS
  Sleep(millisecond);
#else
  struct timespec req = {
    (time_t)(millisecond / 1000),
    (long)((millisecond % 1000) * 1000000)
  };

  while (nanosleep(&req, &req) != 0 && errno == EINTR) {
  }
#endif
}

// x_event_internal{{{
X_INL x_error _x_event_agg(
    x_event* self, x_event_agg* aggregation,
    size_t cycle, void* stream, unsigned int flags);
X_INL void _x_event_agg_echo(
    x_event_agg* self, const char* unit, const char* title, FILE* const stream);
X_INL void _x_event_agg_reset(x_event_agg* self);
X_INL double _x_event_dur(x_event* self, const char* unit);
X_INL x_error _x_event_dur_cpu(const x_event* self, double* ns);
#if X_ENABLE_CUDA_DRIVER
X_INL x_error _x_event_dur_cuda_driver(const x_event* self, double* ns);
#endif  // X_ENABLE_CUDA_DRIVER
#if X_ENABLE_CUDA_RUNTIME
X_INL x_error _x_event_dur_cuda_runtime(const x_event* self, double* ns);
#endif  // X_ENABLE_CUDA_RUNTIME
X_INL x_error _x_event_record(
    x_event* self, const bool stop, void* stream, const unsigned int flags);
X_INL x_error _x_event_record_cpu(x_event* self, const bool stop);
#if X_ENABLE_CUDA_DRIVER
X_INL x_error _x_event_record_cuda_driver(
    x_event* self, const bool stop, void* stream, const unsigned int flags);
#endif  // X_ENABLE_CUDA_DRIVER
#if X_ENABLE_CUDA_RUNTIME
X_INL x_error _x_event_record_cuda_runtime(
    x_event* self, const bool stop, void* stream, const unsigned int flags);
#endif  // X_ENABLE_CUDA_RUNTIME
X_INL x_error _x_event_status(x_event* self, const x_error error);
X_INL x_error _x_event_tic(x_event* self, void* stream, unsigned int flags);
X_INL x_error _x_event_toc(x_event* self, void* stream, unsigned int flags);
X_INL bool _x_event_type_valid(const int32_t type);
X_INL double _x_event_unit_scale(const char* unit);

X_INL x_error _x_event_agg(
    x_event* self, x_event_agg* aggregation,
    size_t cycle, void* stream, unsigned int flags)
{
  if (self == NULL || !_x_event_type_valid(self->type) || aggregation == NULL) {
    return x_error_set("posix", EINVAL);
  }

  if (cycle == 0) {
    // NOTE: To ensure member functions are available.
    x_event_agg_init(aggregation);
    return x_ok();
  }

  // NOTE: If the statistics are ready, do not update them.
  if (aggregation->ready) {
    return x_ok();
  }

  if (aggregation->cyc == SIZE_MAX) {
    return x_error_set("posix", EOVERFLOW);
  }

  x_error err = _x_event_toc(self, stream, flags);
  if (x_fail(err)) {
    return err;
  }

  double dur = _x_event_dur(self, "ns");
  if (dur < 0.0) {
    return self->status;
  }

  // Only mutate statistics after obtaining a valid sample.
  if (aggregation->cyc == 0) {
    _x_event_agg_reset(aggregation);
  }

  if (dur > aggregation->max.val) {
    aggregation->max.idx = aggregation->cyc;
    aggregation->max.val = dur;
  }

  if (dur < aggregation->min.val) {
    aggregation->min.idx = aggregation->cyc;
    aggregation->min.val = dur;
  }

  aggregation->sum += dur;
  aggregation->cyc += 1;
  aggregation->avg = aggregation->sum / (double)aggregation->cyc;

  if (aggregation->cyc % cycle == 0) {
    aggregation->ready = true;
  }

  return x_ok();
}

X_INL void _x_event_agg_echo(
    x_event_agg* self, const char* unit, const char* title, FILE* const stream)
{
  if (self == NULL) {
    return;
  }

  char ts[26] = {0};

  const char* prefix = title == NULL || title[0] == '\0'
      ? x_timestamp(ts, sizeof(ts)) : title;

  if (self->cyc == 0) {
    fprintf(
        stream == NULL ? stdout : stream,
        "[%s] no samples (0 cycles)\n", prefix);
    return;
  }

  const char* suffix = unit == NULL || unit[0] == '\0' ? "ns" : unit;

  double scale = _x_event_unit_scale(suffix);
  if (scale == 0.0) {
    suffix = "ns";
    scale = 1.0;
  }

  fprintf(
      stream == NULL ? stdout : stream,
      "[%s] %f%s in %zu cycles - avg: %f%s, min(%zu): %f%s, max(%zu): %f%s\n",
      prefix, self->sum / scale, suffix, self->cyc, self->avg / scale, suffix,
      self->min.idx, self->min.val / scale, suffix,
      self->max.idx, self->max.val / scale, suffix);
}

X_INL void _x_event_agg_reset(x_event_agg* self)
{
  if (self == NULL) {
    return;
  }

  self->ready = false;
  self->cyc = 0;
  self->sum = 0.0;
  self->avg = 0.0;
  self->max.idx = 0;
  self->max.val = -DBL_MAX;
  self->min.idx = 0;
  self->min.val = DBL_MAX;
}

X_INL double _x_event_dur(x_event* self, const char* unit)
{
  if (self == NULL || !_x_event_type_valid(self->type)) {
    return -1.0;
  }

  const double scale = _x_event_unit_scale(unit);
  if (scale == 0.0) {
    return -1.0;
  }

  if (!self->started || !self->stopped) {
    if (x_succ(self->status)) {
      self->status = x_error_set("posix", ENODATA);
    }

    return -1.0;
  }

  double ns = -1.0;
  x_error err = x_ok();

  switch (self->type) {
    case 0:
      err = _x_event_dur_cpu(self, &ns);
      break;
#if X_ENABLE_CUDA_DRIVER
    case 1:
      err = _x_event_dur_cuda_driver(self, &ns);
      break;
#endif
#if X_ENABLE_CUDA_RUNTIME
    case 2:
      err = _x_event_dur_cuda_runtime(self, &ns);
      break;
#endif
    default:
      return -1.0;
  }

  self->status = err;
  if (x_fail(err)) {
    return -1.0;
  }

  return ns / scale;
}

// Backend helpers write nanoseconds only on success. Sample state and units
// are checked by _x_event_dur; these helpers do not mutate self->status.
X_INL x_error _x_event_dur_cpu(const x_event* self, double* ns)
{
  double dur = 0.0;

#if X_WINDOWS
  if (self->frequency <= 0) {
    return x_error_set("posix", EINVAL);
  }

  if (self->stop.counter < self->start.counter) {
    return x_error_set("posix", ERANGE);
  }

  // Subtract as unsigned integers to avoid signed overflow, then convert the
  // difference rather than rounding the two absolute counts to double.
  const uint64_t diff = (uint64_t)self->stop.counter
                      - (uint64_t)self->start.counter;
  dur = (double)diff * (1e9 / (double)self->frequency);
#else
  // difftime avoids overflowing time_t subtraction before conversion.
  dur = difftime(self->stop.cpu.tv_sec, self->start.cpu.tv_sec) * 1e9
      + (double)self->stop.cpu.tv_nsec - (double)self->start.cpu.tv_nsec;
#endif

  if (isnan(dur)) {
    return x_error_set("posix", EDOM);
  }

  if (!isfinite(dur) || dur < 0.0) {
    return x_error_set("posix", ERANGE);
  }

  *ns = dur;

  return x_ok();
}

#if X_ENABLE_CUDA_DRIVER
X_INL x_error _x_event_dur_cuda_driver(const x_event* self, double* ns)
{
  CUresult cres = cuEventSynchronize(self->stop.cu);
  if (cres != CUDA_SUCCESS) {
    return x_error_set("cudadrv", (int32_t)cres);
  }

  float ms = 0.0f;
  cres = cuEventElapsedTime(&ms, self->start.cu, self->stop.cu);
  if (cres != CUDA_SUCCESS) {
    return x_error_set("cudadrv", (int32_t)cres);
  }

  if (isnan(ms)) {
    return x_error_set("posix", EDOM);
  }

  if (!isfinite(ms) || ms < 0.0f) {
    return x_error_set("posix", ERANGE);
  }

  *ns = (double)ms * 1e6;

  return x_ok();
}
#endif  // X_ENABLE_CUDA_DRIVER

#if X_ENABLE_CUDA_RUNTIME
X_INL x_error _x_event_dur_cuda_runtime(const x_event* self, double* ns)
{
  cudaError_t cerr = cudaEventSynchronize(self->stop.cuda);
  if (cerr != cudaSuccess) {
    return x_error_set("cudart", (int32_t)cerr);
  }

  float ms = 0.0f;
  cerr = cudaEventElapsedTime(&ms, self->start.cuda, self->stop.cuda);
  if (cerr != cudaSuccess) {
    return x_error_set("cudart", (int32_t)cerr);
  }

  if (isnan(ms)) {
    return x_error_set("posix", EDOM);
  }

  if (!isfinite(ms) || ms < 0.0f) {
    return x_error_set("posix", ERANGE);
  }

  *ns = (double)ms * 1e6;

  return x_ok();
}
#endif  // X_ENABLE_CUDA_RUNTIME

X_INL x_error _x_event_record(
    x_event* self, const bool stop, void* stream, const unsigned int flags)
{
  if (self == NULL || !_x_event_type_valid(self->type)) {
    return x_error_set("posix", EINVAL);
  }

  if (stop && !self->started) {
    if (x_fail(self->status)) {
      return self->status;
    }

    return x_error_set("posix", ENODATA);
  }

  self->stopped = false;
  if (!stop) {
    self->started = false;
  }

  (void)stream;
  (void)flags;
  x_error err = x_ok();

  switch (self->type) {
    case 0:
      err = _x_event_record_cpu(self, stop);
      break;
#if X_ENABLE_CUDA_DRIVER
    case 1:
      err = _x_event_record_cuda_driver(self, stop, stream, flags);
      break;
#endif
#if X_ENABLE_CUDA_RUNTIME
    case 2:
      err = _x_event_record_cuda_runtime(self, stop, stream, flags);
      break;
#endif
    default:
      err = x_error_set("posix", EINVAL);
      break;
  }

  if (x_succ(err)) {
    if (stop) {
      self->stopped = true;
    } else {
      self->started = true;
    }
  }

  return _x_event_status(self, err);
}

X_INL x_error _x_event_record_cpu(x_event* self, const bool stop)
{
#if X_WINDOWS
  LARGE_INTEGER counter = {0};
  if (!QueryPerformanceCounter(&counter)) {
    return x_error_set("win32");
  }

  if (stop) {
    self->stop.counter = (int64_t)counter.QuadPart;
  } else {
    self->start.counter = (int64_t)counter.QuadPart;
  }
#else
  struct timespec ts = {0};
  if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
    return x_error_set("posix");
  }

  if (stop) {
    self->stop.cpu = ts;
  } else {
    self->start.cpu = ts;
  }
#endif

  return x_ok();
}

#if X_ENABLE_CUDA_DRIVER
X_INL x_error _x_event_record_cuda_driver(
    x_event* self, const bool stop, void* stream, const unsigned int flags)
{
  CUresult cres = cuEventRecordWithFlags(
      stop ? self->stop.cu : self->start.cu, (CUstream)stream, flags);
  if (cres != CUDA_SUCCESS) {
    return x_error_set("cudadrv", (int32_t)cres);
  }

  return x_ok();
}
#endif  // X_ENABLE_CUDA_DRIVER

#if X_ENABLE_CUDA_RUNTIME
X_INL x_error _x_event_record_cuda_runtime(
    x_event* self, const bool stop, void* stream, const unsigned int flags)
{
  cudaError_t cerr = cudaEventRecordWithFlags(
      stop ? self->stop.cuda : self->start.cuda, (cudaStream_t)stream, flags);
  if (cerr != cudaSuccess) {
    return x_error_set("cudart", (int32_t)cerr);
  }

  return x_ok();
}
#endif  // X_ENABLE_CUDA_RUNTIME

X_INL x_error _x_event_status(x_event* self, const x_error error)
{
  if (self != NULL) {
    self->status = error;
  }

  return error;
}

X_INL x_error _x_event_tic(x_event* self, void* stream, unsigned int flags)
{
  return _x_event_record(self, false, stream, flags);
}

X_INL x_error _x_event_toc(x_event* self, void* stream, unsigned int flags)
{
  return _x_event_record(self, true, stream, flags);
}

X_INL bool _x_event_type_valid(const int32_t type)
{
  switch (type) {
    case 0:
#if X_ENABLE_CUDA_DRIVER
    case 1:
#endif
#if X_ENABLE_CUDA_RUNTIME
    case 2:
#endif
      return true;
    default:
      return false;
  }
}

X_INL double _x_event_unit_scale(const char* unit)
{
  if (unit == NULL || unit[0] == '\0' || strcmp(unit, "ns") == 0) {
    return 1.0;
  } else if (strcmp(unit, "us") == 0) {
    return 1e3;
  } else if (strcmp(unit, "ms") == 0) {
    return 1e6;
  } else if (strcmp(unit, "s") == 0) {
    return 1e9;
  } else if (strcmp(unit, "m") == 0) {
    return 6e10;
  } else if (strcmp(unit, "h") == 0) {
    return 3.6e12;
  }

  return 0.0;
}
// x_event_internal}}}

// x_event_agg{{{
X_INL void x_event_agg_init(x_event_agg* self)
{
  if (self == NULL) {
    return;
  }

  _x_event_agg_reset(self);
  self->echo = _x_event_agg_echo;
  self->reset = _x_event_agg_reset;
}
// x_event_agg}}}

// x_event{{{
X_INL x_error x_event_init(
    x_event* self, const char* type, const unsigned int flags)
{
  (void)flags;
  if (self == NULL) {
    return x_error_set("posix", EINVAL);
  }

  x_event init = {0};
  *self = init;
  self->type = -1;
  self->agg = _x_event_agg;
  self->dur = _x_event_dur;
  self->tic = _x_event_tic;
  self->toc = _x_event_toc;

  if (type == NULL) {
    return _x_event_status(self, x_error_set("posix", EINVAL));
  }

  if (strcmp(type, "cpu") == 0) {
#if X_WINDOWS
    LARGE_INTEGER freq = {0};
    if (!QueryPerformanceFrequency(&freq)) {
      return _x_event_status(self, x_error_set("win32"));
    }

    if (freq.QuadPart <= 0) {
      return _x_event_status(self, x_error_set("posix", EINVAL));
    }

    self->frequency = (int64_t)freq.QuadPart;
#endif
    self->type = 0;
#if X_ENABLE_CUDA_DRIVER
  } else if (strcmp(type, "cudadrv") == 0) {
    if ((flags & CU_EVENT_DISABLE_TIMING) != 0) {
      return _x_event_status(
          self, x_error_set("cudadrv", CUDA_ERROR_INVALID_VALUE));
    }

    CUevent start = NULL;
    CUresult cres = cuEventCreate(&start, flags);
    if (cres != CUDA_SUCCESS) {
      return _x_event_status(self, x_error_set("cudadrv", (int32_t)cres));
    }
    self->start.cu = start;

    self->stop.cu = NULL;

    // Record resource ownership before creating the second event so term
    // can clean up (or retain for a retry) after a partial initialization.
    self->type = 1;

    CUevent stop = NULL;
    cres = cuEventCreate(&stop, flags);
    if (cres != CUDA_SUCCESS) {
      // Preserve the creation error even if rollback destruction also fails.
      x_error err = x_error_set("cudadrv", (int32_t)cres);
      x_event_term(self);
      return _x_event_status(self, err);
    }
    self->stop.cu = stop;
#endif
#if X_ENABLE_CUDA_RUNTIME
  } else if (strcmp(type, "cudart") == 0) {
    if ((flags & cudaEventDisableTiming) != 0) {
      return _x_event_status(self, x_error_set("cudart", cudaErrorInvalidValue));
    }

    cudaEvent_t start = NULL;
    cudaError_t cerr = cudaEventCreateWithFlags(&start, flags);
    if (cerr != cudaSuccess) {
      return _x_event_status(self, x_error_set("cudart", (int32_t)cerr));
    }
    self->start.cuda = start;

    self->stop.cuda = NULL;

    // Record resource ownership before creating the second event so term
    // can clean up (or retain for a retry) after a partial initialization.
    self->type = 2;

    cudaEvent_t stop = NULL;
    cerr = cudaEventCreateWithFlags(&stop, flags);
    if (cerr != cudaSuccess) {
      x_error err = x_error_set("cudart", (int32_t)cerr);
      x_event_term(self);
      return _x_event_status(self, err);
    }
    self->stop.cuda = stop;
#endif
  } else {
    return _x_event_status(self, x_error_set("posix", ENOTSUP));
  }

  return _x_event_status(self, x_ok());
}

X_INL x_error x_event_term(x_event* self)
{
  if (self == NULL) {
    return x_error_set("posix", EINVAL);
  }

  x_error err = x_ok();

  switch (self->type) {
#if X_ENABLE_CUDA_DRIVER
    case 1: {
      if (self->start.cu != NULL) {
        CUresult cres = cuEventDestroy_v2(self->start.cu);
        if (cres == CUDA_SUCCESS) {
          self->start.cu = NULL;
        } else {
          err = x_error_set("cudadrv", (int32_t)cres);
        }
      }

      if (self->stop.cu != NULL) {
        CUresult cres = cuEventDestroy_v2(self->stop.cu);
        if (cres == CUDA_SUCCESS) {
          self->stop.cu = NULL;
        } else if (x_succ(err)) {
          err = x_error_set("cudadrv", (int32_t)cres);
        }
      }

      if (self->start.cu == NULL && self->stop.cu == NULL) {
        self->type = -1;
      }

      break;
    }
#endif
#if X_ENABLE_CUDA_RUNTIME
    case 2: {
      if (self->start.cuda != NULL) {
        cudaError_t cerr = cudaEventDestroy(self->start.cuda);
        if (cerr == cudaSuccess) {
          self->start.cuda = NULL;
        } else {
          err = x_error_set("cudart", (int32_t)cerr);
        }
      }

      if (self->stop.cuda != NULL) {
        cudaError_t cerr = cudaEventDestroy(self->stop.cuda);
        if (cerr == cudaSuccess) {
          self->stop.cuda = NULL;
        } else if (x_succ(err)) {
          err = x_error_set("cudart", (int32_t)cerr);
        }
      }

      if (self->start.cuda == NULL && self->stop.cuda == NULL) {
        self->type = -1;
      }

      break;
    }
#endif
    default:
      self->type = -1;
      break;
  }

  // Even a partially released event cannot supply a valid measurement.
  self->started = false;
  self->stopped = false;

  return _x_event_status(self, err);
}
// x_event}}}
// IMPL_Date_and_Time}}}

// ******************************************************** IMPL_File_System{{{
X_INL bool x_fexist(const char* file)
{
  if (file == NULL) {
    return false;
  }

#if X_WINDOWS
  struct _stat64 st = {0};
  return _stat64(file, &st) == 0;
#else
  struct stat st = {0};
  return stat(file, &st) == 0;
#endif
}

X_INL x_error x_fopen(FILE** stream, const char* file, const char* mode)
{
  if (stream == NULL
      || file == NULL || file[0] == '\0'
      || mode == NULL || mode[0] == '\0') {
    return x_error_set("posix", EINVAL);
  }

#if X_WINDOWS
  errno_t perr = fopen_s(stream, file, mode);
  return perr == 0 ? x_ok() : x_error_set("posix", (int32_t)perr);
#else
  *stream = fopen(file, mode);
  return *stream == NULL ? x_error_set("posix") : x_ok();
#endif
}

X_INL const char* x_fpath(
    char* destination, const size_t size, const char* source)
{
  if (destination == NULL || size == 0
      || source == NULL || source[0] == '\0') {
    errno = EINVAL;
    return NULL;
  }

#if X_WINDOWS
  char* path = _fullpath(NULL, source, size);
#else
  char* path = realpath(source, NULL);
#endif
  if (path == NULL) {
    return NULL;
  }

  const size_t len = strlen(path);
  if (len >= size) {
    free(path);
    errno = ENOBUFS;
    return NULL;
  }

  memcpy(destination, path, len + 1);
  free(path);

  return destination;
}

X_INL int64_t x_fsize(const char* file)
{
  if (file == NULL) {
    return -1;
  }

#if X_WINDOWS
  struct _stat64 st = {0};
  return _stat64(file, &st) == 0 ? (int64_t)st.st_size : -1;
#else
  struct stat st = {0};
  return stat(file, &st) == 0 ? (int64_t)st.st_size : -1;
#endif
}
// IMPL_File_System}}}

// *********************************************************** IMPL_Hardware{{{
X_INL size_t x_ncpu(void)
{
#if X_WINDOWS
#if defined(_WIN32_WINNT) && _WIN32_WINNT >= 0x0601
  return (size_t)GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
#else
  SYSTEM_INFO info = {0};
  GetSystemInfo(&info);
  return (size_t)info.dwNumberOfProcessors;
#endif
#else
  long cnt = sysconf(_SC_NPROCESSORS_ONLN);
  return cnt < 0 ? 0 : (size_t)cnt;
#endif
}
// IMPL_Hardware}}}

// ******************************************************** IMPL_Mathematics{{{
X_INL uint64_t x_gcd(const uint64_t m, const uint64_t n)
{
  return n == 0 ? m : x_gcd(n, m % n);
}

X_INL uint64_t x_lcm(const uint64_t m, const uint64_t n)
{
  if (m == 0 || n == 0) {
    return 0;
  }

  const uint64_t factor = m / x_gcd(m, n);
  if (factor > UINT64_MAX / n) {
    errno = ERANGE;
    return 0;
  }

  return factor * n;
}

X_INL uint64_t x_next_exp(const uint64_t base, const uint64_t source)
{
  if (base < 2) {
    errno = EINVAL;
    return 0;
  }

  uint64_t power = 1;
  while (power < source) {
    if (power > UINT64_MAX / base) {
      errno = ERANGE;
      return 0;
    }

    power *= base;
  }

  return power;
}

X_INL uint64_t x_next_mul(const uint64_t base, const uint64_t source)
{
  if (base == 0) {
    errno = EINVAL;
    return 0;
  }

  const uint64_t step = base - source % base;
  if (source > UINT64_MAX - step) {
    errno = ERANGE;
    return 0;
  }

  return source + step;
}

X_INL uint64_t x_prev_exp(const uint64_t base, const uint64_t source)
{
  if (base < 2) {
    errno = EINVAL;
    return 0;
  }

  if (source == 0) {
    return 0;
  }

  uint64_t power = 1;
  const uint64_t limit = source / base;
  while (power <= limit) {
    // The quotient bound guarantees power * base <= source, without overflow.
    power *= base;
  }

  return power;
}

X_INL uint64_t x_prev_mul(const uint64_t base, const uint64_t source)
{
  if (base == 0) {
    errno = EINVAL;
    return 0;
  }

  return source - source % base;
}
// IMPL_Mathematics}}}

// ************************************************************* IMPL_Memory{{{
X_INL x_error x_malloc(void** pointer, const size_t size)
{
  if (pointer == NULL || *pointer != NULL || size == 0) {
    return x_error_set("posix", EINVAL);
  }

  *pointer = malloc(size);

  return *pointer == NULL ? x_error_set("posix") : x_ok();
}

X_INL x_error x_memcpy(void* destination, const void* source, const size_t size)
{
  if (size == 0) {
    return x_ok();
  }

  if (destination == NULL || source == NULL) {
    return x_error_set("posix", EINVAL);
  }

  const uintptr_t dst_addr = (uintptr_t)destination;
  const uintptr_t src_addr = (uintptr_t)source;

  // Subtract addresses to avoid overflowing either range endpoint.
  if ((dst_addr >= src_addr && dst_addr - src_addr < size)
      || (dst_addr < src_addr && src_addr - dst_addr < size)) {
    return x_error_set("posix", EINVAL);
  }

  // Reject overlap first. `memmove()` avoids optimizer `restrict` warnings
  // on rejected calls whose integer-address guard is not understood by GCC.
  memmove(destination, source, size);

  return x_ok();
}

X_INL x_error _x_meminfo_cpu(size_t* available, size_t* total)
{
  if (available == NULL && total == NULL) {
    return x_error_set("posix", EINVAL);
  }

  size_t avail_size = 0;
  size_t total_size = 0;

#if X_LINUX
  struct sysinfo info = {0};
  if (sysinfo(&info) != 0) {
    return x_error_set("posix");
  }

  if (info.mem_unit == 0
      || (available != NULL && info.freeram > SIZE_MAX / info.mem_unit)
      || (total != NULL && info.totalram > SIZE_MAX / info.mem_unit)) {
    return x_error_set("posix", ERANGE);
  }

  if (available != NULL) {
    avail_size = (size_t)info.freeram * info.mem_unit;
  }
  if (total != NULL) {
    total_size = (size_t)info.totalram * info.mem_unit;
  }
#elif X_IOS || X_MACOS
  if (total != NULL) {
    uint64_t mem = 0;
    size_t msz = sizeof(mem);
    if (sysctlbyname("hw.memsize", &mem, &msz, NULL, 0) != 0) {
      return x_error_set("posix");
    }
    if (msz != sizeof(mem)) {
      return x_error_set("posix", EIO);
    }
    if (mem > SIZE_MAX) {
      return x_error_set("posix", ERANGE);
    }
    total_size = (size_t)mem;
  }

  if (available != NULL) {
    const host_t host = mach_host_self();
    vm_size_t page_size = 0;
    vm_statistics64_data_t info = {0};
    mach_msg_type_number_t cnt = HOST_VM_INFO64_COUNT;

    kern_return_t ret = host_page_size(host, &page_size);
    if (ret == KERN_SUCCESS) {
      ret = host_statistics64(host, HOST_VM_INFO64, (host_info64_t)&info, &cnt);
    }

    const kern_return_t rel_ret = mach_port_deallocate(mach_task_self(), host);
    if (ret == KERN_SUCCESS) {
      ret = rel_ret;
    }
    if (ret != KERN_SUCCESS) {
      return x_error_set("mach", (int32_t)ret);
    }
    if (page_size == 0 || info.free_count > SIZE_MAX / page_size) {
      return x_error_set("posix", ERANGE);
    }

    // free_count already includes speculative_count; do not count it twice.
    avail_size = (size_t)info.free_count * (size_t)page_size;
  }
#elif X_WINDOWS
  MEMORYSTATUSEX info = {0};
  info.dwLength = sizeof(info);
  if (!GlobalMemoryStatusEx(&info)) {
    return x_error_set("win32");
  }

  if ((available != NULL && info.ullAvailPhys > SIZE_MAX)
      || (total != NULL && info.ullTotalPhys > SIZE_MAX)) {
    return x_error_set("posix", ERANGE);
  }

  if (available != NULL) {
    avail_size = (size_t)info.ullAvailPhys;
  }
  if (total != NULL) {
    total_size = (size_t)info.ullTotalPhys;
  }
#else
  return x_error_set("posix", ENOTSUP);
#endif

  if (available != NULL) {
    *available = avail_size;
  }
  if (total != NULL) {
    *total = total_size;
  }

  return x_ok();
}

#if X_ENABLE_CUDA_DRIVER
X_INL x_error _x_meminfo_cuda_driver(size_t* available, size_t* total)
{
  if (available == NULL && total == NULL) {
    return x_error_set("posix", EINVAL);
  }

  size_t avail_size = 0;
  size_t total_size = 0;
  const x_error err = x_error_set(
      "cudadrv", (int32_t)cuMemGetInfo_v2(&avail_size, &total_size));
  if (x_fail(err)) {
    return err;
  }

  if (available != NULL) {
    *available = avail_size;
  }
  if (total != NULL) {
    *total = total_size;
  }

  return err;
}
#endif

#if X_ENABLE_CUDA_RUNTIME
X_INL x_error _x_meminfo_cuda_runtime(size_t* available, size_t* total)
{
  if (available == NULL && total == NULL) {
    return x_error_set("posix", EINVAL);
  }

  size_t avail_size = 0;
  size_t total_size = 0;
  const x_error err = x_error_set(
      "cudart", (int32_t)cudaMemGetInfo(&avail_size, &total_size));
  if (x_fail(err)) {
    return err;
  }

  if (available != NULL) {
    *available = avail_size;
  }
  if (total != NULL) {
    *total = total_size;
  }

  return err;
}
#endif

X_INL x_error x_meminfo(const char* type, size_t* available, size_t* total)
{
  if (type == NULL) {
    return x_error_set("posix", EINVAL);
  }

  if (strcmp(type, "cpu") == 0) {
    return _x_meminfo_cpu(available, total);
  }

#if X_ENABLE_CUDA_DRIVER
  if (strcmp(type, "cudadrv") == 0) {
    return _x_meminfo_cuda_driver(available, total);
  }
#endif

#if X_ENABLE_CUDA_RUNTIME
  if (strcmp(type, "cudart") == 0) {
    return _x_meminfo_cuda_runtime(available, total);
  }
#endif

  return x_error_set("posix", ENOTSUP);
}
// IMPL_Memory}}}

// ************************************************************* IMPL_String{{{
X_INL x_error x_strcpy(char* destination, const size_t size, const char* source)
{
  if (destination == NULL || source == NULL || size == 0) {
    return x_error_set("posix", EINVAL);
  }

  const size_t src_size = strlen(source);
  const size_t cpy_size = src_size < size - 1 ? src_size : size - 1;
  const uintptr_t dst_addr = (uintptr_t)destination;
  const uintptr_t src_addr = (uintptr_t)source;

  // Include both terminators; subtract addresses to avoid endpoint overflow.
  if ((dst_addr >= src_addr && dst_addr - src_addr <= src_size)
      || (dst_addr < src_addr && src_addr - dst_addr <= cpy_size)) {
    return x_error_set("posix", EINVAL);
  }

  if (cpy_size != 0) {
    // As in `x_memcpy`, `memmove()` avoids spurious `restrict` warnings.
    memmove(destination, source, cpy_size);
  }

  destination[cpy_size] = '\0';

  return x_ok();
}

X_INL bool x_strmty(const char* string)
{
  return string == NULL || string[0] == '\0';
}
// IMPL_String}}}

// *************************************************************** IMPL_CUDA{{{
#if X_ENABLE_CUDA_DRIVER
X_INL const char* _x_memtype_cuda_driver(const CUdeviceptr pointer)
{
  unsigned int attr = 0;
  if (cuPointerGetAttribute(&attr, CU_POINTER_ATTRIBUTE_MEMORY_TYPE, pointer)
      != CUDA_SUCCESS) {
    return "Unknown";
  }

  switch (attr) {
    case CU_MEMORYTYPE_HOST:
      return "Host";
    case CU_MEMORYTYPE_DEVICE:
      return "Device";
    case CU_MEMORYTYPE_ARRAY:
      return "Array";
    case CU_MEMORYTYPE_UNIFIED:
      return "Unified";
    default:
      return "Unknown";
  }
}
#endif  // X_ENABLE_CUDA_DRIVER

#if X_ENABLE_CUDA_RUNTIME
X_INL const char* _x_memtype_cuda_runtime(const void* pointer)
{
  struct cudaPointerAttributes attr;
  if (cudaPointerGetAttributes(&attr, pointer) != cudaSuccess) {
    return "Unknown";
  }

  switch (attr.type) {
    case cudaMemoryTypeUnregistered:
      return "Unregistered";
    case cudaMemoryTypeHost:
      return "Host";
    case cudaMemoryTypeDevice:
      return "Device";
    case cudaMemoryTypeManaged:
      return "Managed";
    default:
      return "Unknown";
  }
}
#endif  // X_ENABLE_CUDA_RUNTIME

#if (X_ENABLE_CUDA_DRIVER || X_ENABLE_CUDA_RUNTIME)
X_INL const char* x_memtype(const char* api, ...)
{
  if (api == NULL || api[0] == '\0') {
    return "N/A";
  }

#if X_ENABLE_CUDA_DRIVER
  if (strcmp(api, "cudadrv") == 0) {
    va_list args;
    va_start(args, api);
    CUdeviceptr ptr = va_arg(args, CUdeviceptr);
    va_end(args);
    return _x_memtype_cuda_driver(ptr);
  }
#endif

#if X_ENABLE_CUDA_RUNTIME
  if (strcmp(api, "cudart") == 0) {
    va_list args;
    va_start(args, api);
    void* ptr = va_arg(args, void*);
    va_end(args);
    return _x_memtype_cuda_runtime(ptr);
  }
#endif

  return "N/A";
}

X_INL size_t x_ngpu(const char* api)
{
  if (api == NULL || api[0] == '\0') {
    return 0;
  }

#if X_ENABLE_CUDA_DRIVER
  if (strcmp(api, "cudadrv") == 0) {
    int cnt = 0;
    CUresult cres = cuDeviceGetCount(&cnt);
    if (cres != CUDA_SUCCESS) {
      const char* msg = NULL;
      CUresult msg_cres = cuGetErrorString(cres, &msg);
      if (msg_cres == CUDA_SUCCESS && msg != NULL) {
        fprintf(stderr, "cuDeviceGetCount: %s\n", msg);
      } else {
        fprintf(stderr, "cuDeviceGetCount: CUDA driver error %d\n", (int)cres);
      }
      return 0;
    }

    return (size_t)cnt;
  }
#endif

#if X_ENABLE_CUDA_RUNTIME
  if (strcmp(api, "cudart") == 0) {
    int cnt = 0;
    cudaError_t cerr = cudaGetDeviceCount(&cnt);
    if (cerr != cudaSuccess) {
      fprintf(stderr, "cudaGetDeviceCount: %s\n", cudaGetErrorString(cerr));
      return 0;
    }

    return (size_t)cnt;
  }
#endif

  return 0;
}
#endif  // (X_ENABLE_CUDA_DRIVER || X_ENABLE_CUDA_RUNTIME)
// IMPL_CUDA}}}


#endif  // X_H
