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
 * @brief Cross-platform C++ utilities.
 * @note On Linux, include this header before any system header so its default
 *       `_GNU_SOURCE` and `_FILE_OFFSET_BITS=64` definitions take effect.
 *       Alternatively, define these macros in the build configuration before
 *       any headers, including precompiled or forced includes.
 * @note On Cygwin and Linux, keep `_FILE_OFFSET_BITS` consistent across
 *       translation units that share file-system interfaces, and configure it
 *       before including system headers.
 */
#ifndef X_H
#define X_H x_version(0, 9, 1)

/// @brief Version and included modules; Core includes Error Handling.
#define X_H_VERBOSE "0.9.1+core.log.checksum.time.fs.hw.math.mem.str.cuda"

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

// Architecture width detection requires <cstdint>.
#include <cstdint>
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
#include <cctype>
#include <cerrno>
#include <cfloat>
#include <climits>
#include <cmath>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <limits>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <type_traits>
#include <utility>

#if X_WINDOWS
#if X_ENABLE_SOCKET
#if defined(_MSC_VER)
#pragma comment(lib, "Ws2_32")
#endif
#include <winsock2.h>
#endif
#include <windows.h>
#else
#if X_LINUX
#include <sys/sysinfo.h>
#elif X_MACOS
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

#define X_INL inline

X_INL const char* _x_filename(const char* file)
{
  const char* sep = std::strrchr(file, '/');

#if X_WINDOWS
  const char* alt = std::strrchr(file, '\\');
  if (alt != nullptr && (sep == nullptr || alt > sep)) {
    sep = alt;
  }
#endif

  return sep == nullptr ? file : sep + 1;
}

/// @brief Get the base name from a full path.
/// @note Windows recognizes both `'/'` and `'\\'`; POSIX recognizes only
///       `'/'`.
#define X_FILENAME _x_filename(__FILE__)

// Shared Core helper for Logging and Date and Time.
X_INL const char* _x_timestamp(char* const buffer, const size_t size)
{
  if (buffer == nullptr || size == 0) {
    return "";
  }

  buffer[0] = '\0';
  if (size < 26) {
    return buffer;
  }

  const time_t now = std::time(nullptr);
  if (now == static_cast<time_t>(-1)) {
    return buffer;
  }

#if X_WINDOWS
  if (ctime_s(buffer, size, &now) != 0) {
#else
  if (ctime_r(&now, buffer) == nullptr) {
#endif
    buffer[0] = '\0';
    return buffer;
  }

  const size_t len = std::strlen(buffer);
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
///            `nullptr`, or an empty string to omit the message.
/// @note The expression is evaluated once; the message is evaluated once only
///       when the assertion fails. Diagnostic text reflects macro expansion.
#define x_assert(...) \
  _X_ASSERT_SELECT(__VA_ARGS__, _X_ASSERT_TWO, _X_ASSERT_ONE, 0)(__VA_ARGS__)

/// @brief Evaluate a domain status-code or @ref x_error expression and report
///        failure.
/// @param domain The nonempty registered domain name for status codes;
///               ignored for @ref x_error results.
/// @param error A writable @ref x_error lvalue receiving the result.
/// @param expression A complete expression yielding a code fitting `int32_t`
///                   or @ref x_error.
/// @note All three arguments are evaluated once. Failures print to `stderr`
///       independently of Logging; Debug diagnostics include source context
///       and the expression text.
///       Status-code failures do not abort.
/// @attention Does not capture `errno`/`GetLastError()`. APIs returning
///            sentinel values require a failure-checking wrapper. Parenthesize
///            expressions containing macro-visible commas, including template
///            commas.
#define x_check(domain, error, expression) do { \
  (error) = _x_check_impl( \
      (domain), [&]() { return (expression); }, \
      X_FILENAME, #expression, static_cast<long long>(__LINE__)); \
} while (false)

/// @brief Total registry capacity, including built-in domains.
/// @note Use the same capacity and feature configuration in every translation
///       unit.
#ifndef X_ERROR_DOMAIN_LIMIT
#define X_ERROR_DOMAIN_LIMIT (16)
#endif

#if X_ERROR_DOMAIN_LIMIT < (2 + !!X_WINDOWS + !!X_ENABLE_SOCKET \
    + !!X_ENABLE_CUDA_DRIVER + !!X_ENABLE_CUDA_RUNTIME + !!X_ENABLE_CUBLAS)
#error "X_ERROR_DOMAIN_LIMIT must accommodate all enabled built-in domains."
#endif

/// @brief Error interpretation and optional capture callbacks for a domain.
/// @note A `nullptr` `failed` callback uses `value != 0`; `nullptr` `capture`
///       means unsupported. `message` must terminate `buffer` when
///       `buffer != nullptr` and `size > 0`.
/// @attention Keep the domain, `name`, and callbacks valid and unchanged while
///            registered or referenced by an @ref x_error.
struct x_error_domain
{
  int32_t (*capture)();
  bool (*failed)(const int32_t value);
  char* (*message)(char* buffer, const size_t size, const int32_t value);

  const char* name;
};

/// @brief An error value; conversion to `bool` means failure.
/// @note `"generic"` uses 0 for success and numeric messages; capture is
///       unsupported. All @ref x_error::set overloads return `*this` and clear
///       the previous message override unless a new one is supplied. See
///       @ref x_error_domain for lifetime requirements.
class x_error
{
public:
  /// @brief Construct a successful error value in the `posix` domain.
  X_INL x_error();

  /// @brief Capture the current error value for a registered domain name.
  /// @attention `nullptr`/empty/unknown names or unsupported capture requests
  ///            print a diagnostic and abort. Capture only after API failure;
  ///            last-error state may be stale after success.
  X_INL explicit x_error(const char* name);

  /// @brief Construct an error from a registered domain name and value.
  /// @attention `nullptr`/empty/unknown names print a diagnostic and abort.
  X_INL x_error(const char* name, const int32_t value);

  /// @brief Construct an error with a copied, user-supplied message.
  /// @param message A C string to override domain formatting; `nullptr`
  ///                selects domain formatting; an empty string is retained.
  /// @attention The same name requirements as `x_error(name, value)` apply.
  X_INL x_error(const char* name, const int32_t value, const char* message);

  /// @brief Construct an error referring directly to a domain and value.
  /// @note The domain need not be registered; its lifetime rules still apply.
  X_INL x_error(const x_error_domain& domain, const int32_t value);

  /// @brief Capture the current error value for a registered domain name.
  /// @attention The name and capture requirements of `x_error(name)` apply.
  X_INL x_error& set(const char* name);

  /// @brief Set a registered domain name and value.
  /// @attention The same name requirements as `x_error(name, value)` apply.
  X_INL x_error& set(const char* name, const int32_t value);

  /// @brief Set a registered domain name, value, and copied message override.
  /// @note The name/message rules of the three-argument constructor apply.
  X_INL x_error& set(
      const char* name, const int32_t value, const char* message);

  /// @brief Set a domain directly and a value.
  /// @note The domain need not be registered; its lifetime rules still apply.
  X_INL x_error& set(const x_error_domain& domain, const int32_t value);

  /// @brief Get the domain name, or `"generic"` when the domain has no name.
  X_INL const char* domain() const;

  /// @brief Get an owned message string.
  /// @note A message override takes precedence and is not truncated.
  ///       Otherwise, domain messages use 256 bytes including the terminator
  ///       and may truncate. Text remains valid until mutation or the next
  ///       no-argument @ref x_error::message() call. Concurrent no-argument
  ///       calls on this object are not safe.
  X_INL const char* message() const;

  /// @brief Format an error message into caller-provided storage.
  /// @param buffer Writable destination buffer.
  /// @param size Capacity in bytes, including the terminator.
  /// @return `buffer`. A `nullptr` `buffer` or zero `size` performs no write.
  /// @note Messages may be truncated to fit the buffer. Uses caller capacity,
  ///       not the no-argument limit, and does not mutate cached text.
  X_INL const char* message(char* buffer, const size_t size) const;

  /// @brief Get the domain status code.
  X_INL int32_t value() const;

  /// @brief Return whether this error value indicates failure.
  /// @note Uses the domain's predicate; a `nullptr` predicate uses
  ///       `value != 0`.
  X_INL operator bool() const;

  /// @brief Register a named user-defined error domain.
  /// @return 0 on success; `EINVAL` for a `nullptr` domain or `nullptr`/empty
  ///         name; `EEXIST` for a duplicate name (including built-ins);
  ///         `ENOSPC` when full.
  /// @attention Register before concurrent access; registration is not
  ///            synchronized. See @ref x_error_domain for lifetime
  ///            requirements.
  X_INL static int32_t register_domain(const x_error_domain* domain);

private:
  const x_error_domain* m_domain{nullptr};
  int32_t m_value{0};
  bool m_has_message{false};
  mutable std::string m_message;
};

/// @brief Construct a successful error value.
X_INL x_error x_ok();

/// @brief Return whether an error value indicates failure.
/// @note Uses the domain predicate; a missing predicate uses `value != 0`.
X_INL bool x_fail(const x_error& error);

/// @brief Return whether an error value indicates success.
X_INL bool x_succ(const x_error& error);
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
  _x_log_impl<level>( \
      X_FILENAME, __func__, static_cast<long long>(__LINE__), \
      (stream), __VA_ARGS__); \
} while (false)
// DECL_Logging}}}

/******************************************************************************
 * @name DECL_Checksums{{{
 * @brief Data checksum utilities.
 *****************************************************************************/
/// @brief Calculate the CRC-32/ISO-HDLC checksum of a data chunk.
/// @note A `nullptr` `previous` starts a new calculation. Otherwise,
///       `*previous` is the checksum returned by the preceding calculation.
///       Chunks of any length may be processed incrementally. `data` need not
///       be aligned.
/// @note If `data` is `nullptr`, `size` is ignored. The function returns
///       `*previous` when `previous` is non-null, or 0 otherwise.
X_INL uint32_t x_checksum_crc32(
    const void* data, const size_t size, const uint32_t* previous = nullptr);

/// @brief Calculate the internet checksum of a data chunk.
/// @see [RFC1071](https://www.rfc-editor.org/info/rfc1071)
/// @note A `nullptr` `previous` starts a new calculation. Otherwise,
///       `*previous` is the checksum returned by the preceding calculation.
///       For incremental calculation over concatenated chunks, every chunk
///       except the last must have even length. `data` need not be aligned.
/// @note The result is a platform-independent numeric checksum. Convert it to
///       network byte order before storing it in a network-order 16-bit field
///       (e.g., using `htons()`, declared by the caller's platform headers).
/// @note If `data` is `nullptr`, `size` is ignored. The function returns
///       `*previous` when `previous` is non-null, or `0xFFFF` otherwise.
X_INL uint16_t x_checksum_rfc1071(
    const void* data, const size_t size, const uint16_t* previous = nullptr);

/// @brief Calculate the XOR checksum of a data chunk.
/// @note A `nullptr` `previous` starts a new calculation. Otherwise,
///       `*previous` is the checksum returned by the preceding calculation.
///       Chunks of any length may be processed incrementally. `data` need not
///       be aligned.
/// @note If `data` is `nullptr`, `size` is ignored. The function returns
///       `*previous` when `previous` is non-null, or 0 otherwise.
X_INL uint8_t x_checksum_xor(
    const void* data, const size_t size, const uint8_t* previous = nullptr);
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
/// @return `buffer` when `buffer` is non-null and `size` is positive;
///         otherwise an empty string literal. Insufficient capacity or a
///         time/conversion failure produces an empty string in a nonempty
///         destination buffer.
/// @note Uses the `ctime()`-style format with the trailing newline removed.
#define x_timestamp _x_timestamp

/// @struct x_event_agg
/// @brief A structure for stopwatch aggregation.
/// @note Default construction initializes the aggregation. `min` and `max`
///       have measurement meaning only when `cyc` is positive.
/// @var x_event_agg::ready
///      Whether the group has reached its target number of valid samples.
/// @var x_event_agg::cyc
///      The number of valid samples accumulated since construction/reset.
/// @var x_event_agg::sum
///      The total elapsed time in nanoseconds.
/// @var x_event_agg::avg
///      The average elapsed time in nanoseconds.
/// @var x_event_agg::max
///      Zero-based index and duration in nanoseconds of the maximum sample.
/// @var x_event_agg::min
///      Zero-based index and duration in nanoseconds of the minimum sample.
struct x_event_agg
{
  /// @brief Print the aggregation, or a no-samples message when `cyc` is zero.
  /// @param unit The unit, `"ns"`, `"us"`, `"ms"`, `"s"`, `"m"`, or `"h"`.
  ///        `nullptr`, empty, and unknown units use `"ns"`.
  /// @param title A null-terminated title, `nullptr`, or an empty string.
  ///        `nullptr` and empty titles use a timestamp generated at echo time.
  /// @param stream The output stream; `nullptr` selects `stdout`.
  X_INL void echo(
      const char* unit, const char* title = nullptr,
      FILE* stream = stdout) const;

  /// @brief Clear the samples and `ready` flag.
  X_INL void reset();

  bool ready{false};
  size_t cyc{0};
  double sum{0.0};
  double avg{0.0};
  struct {
    size_t idx{0};
    double val{-DBL_MAX};
  } max;
  struct {
    size_t idx{0};
    double val{DBL_MAX};
  } min;
};

/// @class x_event
/// @brief A CPU or CUDA elapsed-time recorder.
/// @note CPU timing uses a monotonic clock. CUDA @ref x_event::tic and
///       @ref x_event::toc enqueue event records; @ref x_event::dur and
///       @ref x_event::agg wait for the stop event to calculate durations.
/// @note Request-check failures in @ref x_event::tic, @ref x_event::toc, and
///       @ref x_event::agg are returned without changing status or samples.
///       Aggregation-only resets, no-ops, and count overflow also preserve
///       @ref x_event::status. Backend recording/calculation failures
///       update it.
class x_event
{
public:
  /// @brief Construct a CPU (`"cpu"`), CUDA driver (`"cu"`), or CUDA runtime
  ///        (`"cuda"`) event.
  /// @param type The non-null name of an enabled backend.
  /// @param flags Backend creation flags; ignored for CPU.
  /// @note CUDA events must support timing; disable-timing flags are rejected.
  ///       Creation flags differ from the recording flags accepted by
  ///       @ref x_event::tic and @ref x_event::toc.
  ///       Constructor unwinding attempts to release successfully acquired
  ///       handles, including after partial initialization.
  /// @throws std::invalid_argument for invalid requests.
  /// @throws std::runtime_error for backend creation/query failures.
  X_INL explicit x_event(const char* type, unsigned int flags = 0);

  /// @brief Release the owned event resources automatically.
  /// @note Destruction is attempted for each non-null CUDA start and stop
  ///       handle, even if destroying the other handle fails. Destruction is
  ///       best effort, does not throw, and does not retry a failed release.
  X_INL ~x_event() = default;

  /// @brief Event resources cannot be copied.
  x_event(const x_event&) = delete;
  x_event& operator=(const x_event&) = delete;

  /// @brief Record an endpoint and accumulate one valid sample on success.
  /// @param aggregation The multi-cycle aggregation of the event.
  /// @param cycle Target valid-sample count; keep it fixed within a group.
  ///        Zero resets aggregation without recording. A ready aggregation is
  ///        not updated; reset it to start another group.
  /// @param stream CUDA stream; `nullptr` selects default. Ignored for CPU.
  /// @param flags CUDA recording flags; 0 selects defaults. Ignored for CPU.
  /// @return The result; measurement failures update @ref x_event::status.
  /// @note Failed samples leave aggregation unchanged. Without a start sample,
  ///       reports the recording failure, if any, or `ENODATA`. Call
  ///       @ref x_event::tic for each cycle to measure independent intervals.
  X_INL x_error agg(
      x_event_agg& aggregation, size_t cycle,
      void* stream = nullptr, unsigned int flags = 0);

  /// @brief Calculate the duration between two events.
  /// @param unit The duration unit, `"h"`, `"m"`, `"s"`, `"ms"`, `"us"`, or
  ///        `"ns"`. `nullptr` and empty strings default to `"ns"`; unknown
  ///        units fail.
  /// @return The elapsed time, or -1.0 on error in every unit. Invalid event
  ///         or unit requests leave @ref x_event::status unchanged;
  ///         measurement failures are stored in @ref x_event::status.
  /// @note Incomplete samples preserve an existing measurement failure, or
  ///       set `ENODATA`. Invalid CPU frequency sets `EINVAL`; NaN sets
  ///       `EDOM`; negative or infinite durations set `ERANGE`. A successful
  ///       calculation clears the measurement error.
  /// @attention Requires successful @ref x_event::tic and @ref x_event::toc
  ///            for the current measurement.
  X_INL double dur(const char* unit = "ns") const;

  /// @brief Get the current measurement status, not the result of every call.
  /// @note Success does not by itself imply complete samples. Successful
  ///       backend recording/calculation clears the measurement error.
  ///       The returned reference reflects subsequent measurement updates;
  ///       copy it to retain a snapshot.
  X_INL const x_error& status() const noexcept;

  /// @brief Record the start of a new measurement.
  /// @param stream CUDA stream; `nullptr` selects default. Ignored for CPU.
  /// @param flags CUDA recording flags; 0 selects defaults. Ignored for CPU.
  /// @return The result; backend recording updates @ref x_event::status.
  /// @note A backend recording attempt invalidates the previous duration,
  ///       including on failure. A successful attempt clears the old error.
  X_INL x_error tic(void* stream = nullptr, unsigned int flags = 0);

  /// @brief Record the end of the current measurement.
  /// @param stream CUDA stream; `nullptr` selects default. Ignored for CPU.
  /// @param flags CUDA recording flags; 0 selects defaults. Ignored for CPU.
  /// @return The result; backend recording updates @ref x_event::status.
  /// @note Without a start sample, returns the existing measurement failure,
  ///       if any, or `ENODATA`, without changing status. A failed backend
  ///       attempt invalidates the stop sample; @ref x_event::toc may be
  ///       retried.
  /// @attention Requires a successful @ref x_event::tic for the current
  ///            measurement.
  X_INL x_error toc(void* stream = nullptr, unsigned int flags = 0);

private:
  // Members own resources, including during constructor unwinding.
  struct endpoint
  {
    int type{-1};
    timespec cpu{};
#if X_WINDOWS
    int64_t counter{0};
#endif
#if X_ENABLE_CUDA_DRIVER
    CUevent cu{nullptr};
#endif
#if X_ENABLE_CUDA_RUNTIME
    cudaEvent_t cuda{nullptr};
#endif
    X_INL ~endpoint() noexcept;
  } m_start, m_stop;

  int m_type{-1};
#if X_WINDOWS
  int64_t m_frequency{0};
#endif
  bool m_started{false};
  bool m_stopped{false};
  mutable x_error m_status;

  X_INL x_error dur_cpu(double& ns) const;

#if X_ENABLE_CUDA_DRIVER
  X_INL x_error dur_cuda_driver(double& ns) const;
#endif  // X_ENABLE_CUDA_DRIVER

#if X_ENABLE_CUDA_RUNTIME
  X_INL x_error dur_cuda_runtime(double& ns) const;
#endif  // X_ENABLE_CUDA_RUNTIME

  X_INL x_error record(bool stop, void* stream, unsigned int flags);

  X_INL x_error record_cpu(endpoint& target);

#if X_ENABLE_CUDA_DRIVER
  X_INL x_error record_cuda_driver(
      endpoint& target, void* stream, unsigned int flags);
#endif  // X_ENABLE_CUDA_DRIVER

#if X_ENABLE_CUDA_RUNTIME
  X_INL x_error record_cuda_runtime(
      endpoint& target, void* stream, unsigned int flags);
#endif  // X_ENABLE_CUDA_RUNTIME
};
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
/// @note A `nullptr` `stream` or `nullptr`/empty `file`/`mode` returns
///       `EINVAL` without changing `*stream`. Other failures may change it;
///       use it only after success.
/// @attention Mode syntax is not validated. Unsupported Windows modes may
///            invoke the CRT invalid parameter handler (terminates by
///            default).
X_INL x_error x_fopen(FILE** stream, const char* file, const char* mode);

/// @brief Get the absolute path of a file or directory.
/// @param destination Writable destination buffer.
/// @param size Destination capacity in bytes, including the terminator.
/// @param source A nonempty, null-terminated source path.
/// @return `destination` on success, or `nullptr` on failure with `errno` set.
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
X_INL size_t x_ncpu();
// DECL_Hardware}}}

/******************************************************************************
 * @name DECL_Mathematics{{{
 * @brief A collection of mathematical utilities.
 *****************************************************************************/
/// @brief Constant PI of user-defined type.
/// @var x_Pi
/// @tparam T The user-defined type.
/// @return The constant PI.
template<typename T>
inline constexpr T x_Pi =
  static_cast<T>(3.141592653589793238462643383279502884197169399375L);

/// @brief Kibibyte constant generator, i.e., 1 KiB = 1024 bytes.
/// @param scale The scaling factor.
/// @note Capacity helpers evaluate `scale` once and do not check overflow.
///       Choose `T` to represent the unit constant and the scaled result;
///       multiplication follows the usual arithmetic conversions, including
///       integer promotion. The result is returned as `T`.
/// @note `T` is deduced from `scale` unless explicitly specified. For large
///       integer capacities, use a suitable type, e.g., `x_TiB<uint64_t>(1)`;
///       an `int` literal alone does not select a wider type.
template<typename T>
X_INL constexpr T x_KiB(T scale);

/// @brief Mebibyte constant generator, i.e., 1 MiB = 1048576 bytes.
/// @param scale The scaling factor.
template<typename T>
X_INL constexpr T x_MiB(T scale);

/// @brief Gibibyte constant generator, i.e., 1 GiB = 1073741824 bytes.
/// @param scale The scaling factor.
template<typename T>
X_INL constexpr T x_GiB(T scale);

/// @brief Tebibyte constant generator, i.e., 1 TiB = 1099511627776 bytes.
/// @param scale The scaling factor.
template<typename T>
X_INL constexpr T x_TiB(T scale);

/// @brief Pebibyte constant generator, i.e., 1 PiB = 1125899906842624 bytes.
/// @param scale The scaling factor.
template<typename T>
X_INL constexpr T x_PiB(T scale);

/// @brief Generate a mask with only the specified bit set.
/// @param bit The zero-based bit index, which must be in [0, 63].
/// @return A `uint64_t` bit mask.
/// @note `T` must be integral, excluding `bool`. A constant-expression index
///       yields a constant expression.
/// @attention The index is not checked and `errno` is not changed.
/// @see C++'s `std::bitset` for a more versatile solution.
template<typename T>
X_INL constexpr uint64_t x_bit(T bit);

/// @brief Calculate the greatest common divisor of two integers.
/// @param m The first integer.
/// @param n The second integer.
/// @return The greatest common divisor; `gcd(0, 0)` is defined here as 0.
/// @note Integer helpers (gcd, lcm, next/prev exp/mul) require integral `T`
///       other than `bool` and nonnegative inputs. Negative inputs return 0
///       with `errno = EINVAL`. Success preserves `errno`; valid calls can
///       be `constexpr`.
template<typename T>
X_INL constexpr T x_gcd(T m, T n);

/// @brief Calculate the least common multiple of two integers.
/// @param m The first integer.
/// @param n The second integer.
/// @return The least common multiple, or 0 if either argument is 0.
/// @note The integer-helper rules documented by @ref x_gcd apply. Overflow
///       returns 0 with `errno = ERANGE`; clear `errno` first to distinguish a
///       valid zero.
template<typename T>
X_INL constexpr T x_lcm(T m, T n);

/// @brief Round up to an integer power of a base, including `base^0 == 1`.
/// @param base The base, which must be at least 2.
/// @param source The nonnegative source number.
/// @return The smallest power greater than or equal to source; 0 yields 1.
/// @note The integer-helper rules documented by @ref x_gcd apply. Invalid
///       `base` returns 0 with `errno = EINVAL`, even for `source == 0`.
///       Overflow returns 0 with `errno = ERANGE`.
template<typename T>
X_INL constexpr T x_next_exp(T base, T source);

/// @brief Calculate the next multiple of a base.
/// @param base The base, which must be positive.
/// @param source The nonnegative source number.
/// @return The smallest multiple strictly greater than source, including when
///         `source` is already a multiple. A zero `source` yields `base`.
/// @note The integer-helper rules documented by @ref x_gcd apply. Invalid
///       `base` returns 0 with `errno = EINVAL`; overflow returns 0 with
///       `errno = ERANGE`.
template<typename T>
X_INL constexpr T x_next_mul(T base, T source);

/// @brief Round down to an integer power of a base, including `base^0 == 1`.
/// @param base The base, which must be at least 2.
/// @param source The nonnegative source number.
/// @return The largest power less than or equal to source; 0 yields 0.
/// @note The integer-helper rules documented by @ref x_gcd apply. Invalid
///       `base` returns 0 with `errno = EINVAL`, even for `source == 0`.
///       Clear `errno` first to distinguish a valid zero.
template<typename T>
X_INL constexpr T x_prev_exp(T base, T source);

/// @brief Calculate the previous multiple of a base.
/// @param base The base, which must be positive.
/// @param source The nonnegative source number.
/// @return The largest multiple less than or equal to source.
/// @note The integer-helper rules documented by @ref x_gcd apply. Invalid
///       `base` returns 0 with `errno = EINVAL`; clear `errno` first to
///       distinguish a valid zero.
template<typename T>
X_INL constexpr T x_prev_mul(T base, T source);
// DECL_Mathematics}}}

/******************************************************************************
 * @name DECL_Memory{{{
 * @brief A collection of memory management utilities.
 *****************************************************************************/
/// @brief Get number of items in an array.
/// @param array An array object, regardless of its storage duration.
/// @attention Pointers, including array function parameters (which are
///            pointers), are not supported.
template<typename T, size_t N>
X_INL constexpr size_t x_count(const T (&array)[N]);

/// @brief Delete a dynamically allocated object or array and set its pointer
///        to `nullptr`.
/// @tparam array `true` selects `delete[]`; `false` selects `delete`.
/// @param pointer A writable pointer lvalue allocated by the matching `new`.
/// @remark If the pointer is `nullptr`, deletion has no effect.
template<bool array, typename T>
X_INL void x_delete(T*& pointer);

/// @brief Free a memory block allocated on the heap and set it to `nullptr`.
/// @param pointer A writable object-pointer lvalue whose non-null value is
///                valid to pass to `free()`.
/// @note The argument is evaluated once and bound by reference.
///       Pointers to `const`/`volatile`-qualified types are supported.
/// @remark If the pointer is `nullptr`, this helper does nothing.
template<typename T>
X_INL void x_free(T*& pointer);

/// @brief Wrapper of `malloc()` with error handling.
/// @param pointer Pointer to a writable `T*` object initialized to `nullptr`.
/// @param size Number of bytes to allocate; must be positive.
/// @return Success, or an error in the `posix` domain.
/// @note A `nullptr` `pointer`, a non-null `*pointer`, or a zero `size`
///       returns `EINVAL` without changing the output. Allocation failure
///       leaves `*pointer` null and captures the allocator's `errno`.
/// @attention The function reads `*pointer` before allocating. This allocates
///            raw storage without constructing objects; callers must satisfy
///            their objects' alignment and lifetime requirements.
template<typename T>
X_INL x_error x_malloc(T** pointer, const size_t size);

/// @brief Wrapper of `memcpy()` with error handling.
/// @param destination Destination buffer.
/// @param source Source buffer.
/// @param size Number of bytes to copy.
/// @return Success, or `EINVAL` in the `posix` domain when `size` is nonzero
///         and either pointer is `nullptr` or the ranges overlap.
/// @note A zero `size` succeeds without accessing either buffer; both pointers
///       may be `nullptr` in that case. Failure preserves the destination.
/// @attention Provide readable/writable ranges of at least `size` bytes;
///            pointer validity and capacity are not checked. Identical
///            pointers are rejected. Overlap detection assumes `uintptr_t`
///            byte-address ordering. Copying object values requires trivially
///            copyable types and valid object lifetimes.
X_INL x_error x_memcpy(
    void* destination, const void* source, const size_t size);

/// @brief Query size available and total memories on CPU or GPU.
/// @param type Can be `"cpu"`, `"cu"` (via CUDA driver API), or `"cuda"`
///             (via CUDA runtime API).
/// @param available Size of available memory.
/// @param total Size of total memory.
/// @return An instance of @ref x_error.
/// @note Either output may be `nullptr`, but not both. Query failures leave
///       outputs unchanged. Unknown types and disabled backends return
///       `ENOTSUP`.
/// @note CPU queries support Linux, macOS, and Windows. Other platforms return
///       `ENOTSUP`; an unrepresentable byte count returns `ERANGE`.
/// @note GPU queries use the current CUDA context/device. Driver queries
///       require prior successful `cuInit(0)` and a current context.
/// @note CPU available is a system-wide snapshot, not an allocation guarantee:
///       Linux `freeram`, macOS `free_count` * page size (includes speculative
///       pages), Windows `ullAvailPhys`. Mach failures use `generic` codes;
///       `sysctlbyname()` uses `posix`.
X_INL x_error x_meminfo(
    const char* type, size_t* available, size_t* total);
// DECL_Memory}}}

/******************************************************************************
 * @name DECL_String{{{
 * @brief A collection of string utilities.
 *****************************************************************************/
/// @brief Copy a string with truncation and argument checking.
/// @param destination Writable destination buffer of at least `size` bytes.
/// @param size Destination capacity in bytes, including the terminator.
/// @param source A valid null-terminated source string.
/// @return Success, or `EINVAL` in the `posix` domain for a `nullptr` pointer,
///         zero capacity, or overlapping source and destination write ranges.
/// @note Copies at most `size - 1` characters and appends a terminator.
///       Truncation is successful. The destination is unchanged on failure.
/// @attention The destination write range and full source string must not
///            overlap, including terminators and identical pointers. Pointer
///            validity and capacity are not checked. Detection assumes
///            `uintptr_t` byte-address ordering.
X_INL x_error x_strcpy(
    char* destination, const size_t size, const char* source);

/// @brief Check if a string is empty.
/// @return `true` for `nullptr` or a string whose first character is `'\0'`.
/// @note A non-null argument must point to a valid null-terminated string.
X_INL bool x_strmty(const char* string);
// DECL_String}}}

/******************************************************************************
 * @name DECL_CUDA{{{
 * @brief A collection of CUDA utilities.
 *****************************************************************************/
#if (X_ENABLE_CUDA_DRIVER || X_ENABLE_CUDA_RUNTIME)
/// @brief Query the type of a block of memory.
/// @param api The API to which the memory is corresponding. Must be `"cu"`
///            (CUDA driver) or `"cuda"` (CUDA runtime).
/// @param pointer The memory. This argument is always required and must be:
///                exactly `CUdeviceptr` for `"cu"`, or an object pointer
///                (including `const`/`volatile` pointers) or `nullptr` for
///                `"cuda"`.
/// @return The underlying API's memory type name, `"Unknown"` on query failure
///         or an unrecognized memory type, or `"N/A"` for a `nullptr`/empty
///         `api`, an unknown `api`, an unsupported pointer type, or a disabled
///         backend.
/// @note Driver queries require prior successful `cuInit(0)` and unified
///       virtual addressing as required by `cuPointerGetAttribute()`. Driver
///       and runtime memory classifications are not normalized to each other.
/// @attention For `"cu"`, cast a zero address to `CUdeviceptr` rather than
///            passing an integer literal or pointer. For `"cuda"`, typed
///            object pointers and `nullptr` are accepted directly. The memory
///            is not modified.
template<typename T>
X_INL const char* x_memtype(const char* api, T pointer);

/// @brief Query the number of GPU devices.
/// @param api `"cu"` (CUDA driver) or `"cuda"` (CUDA runtime).
/// @return The device count, or 0 on query failure, a `nullptr`/empty or
///         unknown `api`, or a disabled backend. Query failures are reported
///         to `stderr`.
/// @note For `"cu"`, the caller must first successfully call `cuInit(0)`.
X_INL size_t x_ngpu(const char* api);
#endif
// DECL_CUDA}}}

// ***************************************************** IMPL_Error_Handling{{{
X_INL int32_t _x_error_capture_posix();
#if X_ENABLE_SOCKET
X_INL int32_t _x_error_capture_socket();
#endif  // X_ENABLE_SOCKET
#if X_WINDOWS
X_INL int32_t _x_error_capture_win32();
#endif  // X_WINDOWS

X_INL char* _x_error_copy(char* buffer, const size_t size, const char* text);
X_INL char* _x_error_number(
    char* buffer, const size_t size, const char* prefix, const int32_t value);
X_INL char* _x_error_message_generic(
    char* buffer, const size_t size, const int32_t value);
X_INL char* _x_error_message_posix(
    char* buffer, const size_t size, const int32_t value);
#if X_ENABLE_SOCKET
X_INL char* _x_error_message_socket(
    char* buffer, const size_t size, const int32_t value);
#endif  // X_ENABLE_SOCKET
#if X_WINDOWS
X_INL char* _x_error_message_win32(
    char* buffer, const size_t size, const int32_t value);
#endif  // X_WINDOWS

X_INL bool _x_error_failed_nonzero(const int32_t value);
X_INL const x_error_domain* _x_error_domain_lookup(const char* name);
X_INL const x_error_domain* _x_error_domain_require(const char* name);
X_INL char* _x_error_domain_message(
    char* buffer, const size_t size, const int32_t value,
    const x_error_domain* domain);

inline const x_error_domain error_domain_generic{
  nullptr,
  _x_error_failed_nonzero,
  _x_error_message_generic,
  "generic"
};

inline const x_error_domain error_domain_posix{
  _x_error_capture_posix,
  _x_error_failed_nonzero,
  _x_error_message_posix,
  "posix"
};

#if X_ENABLE_SOCKET
inline const x_error_domain error_domain_socket{
  _x_error_capture_socket,
  _x_error_failed_nonzero,
  _x_error_message_socket,
  "socket"
};
#endif  // X_ENABLE_SOCKET

#if X_WINDOWS
inline const x_error_domain error_domain_win32{
  _x_error_capture_win32,
  _x_error_failed_nonzero,
  _x_error_message_win32,
  "win32"
};
#endif  // X_WINDOWS

#if X_ENABLE_CUDA_DRIVER
X_INL bool _x_error_failed_cuda_driver(const int32_t value);
X_INL char* _x_error_message_cuda_driver(
    char* buffer, const size_t size, const int32_t value);

inline const x_error_domain error_domain_cuda_driver{
  nullptr,
  _x_error_failed_cuda_driver,
  _x_error_message_cuda_driver,
  "cu"
};
#endif  // X_ENABLE_CUDA_DRIVER

#if X_ENABLE_CUDA_RUNTIME
X_INL bool _x_error_failed_cuda_runtime(const int32_t value);
X_INL char* _x_error_message_cuda_runtime(
    char* buffer, const size_t size, const int32_t value);

inline const x_error_domain error_domain_cuda_runtime{
  nullptr,
  _x_error_failed_cuda_runtime,
  _x_error_message_cuda_runtime,
  "cuda"
};
#endif  // X_ENABLE_CUDA_RUNTIME

#if X_ENABLE_CUBLAS
X_INL bool _x_error_failed_cublas(const int32_t value);
X_INL char* _x_error_message_cublas(
    char* buffer, const size_t size, const int32_t value);

inline const x_error_domain error_domain_cublas{
  nullptr,
  _x_error_failed_cublas,
  _x_error_message_cublas,
  "cublas"
};
#endif  // X_ENABLE_CUBLAS

struct x_error_domain_registry
{
  const x_error_domain* list[X_ERROR_DOMAIN_LIMIT];
  size_t count;
};

inline x_error_domain_registry error_domain_registry{
  {
    &error_domain_generic,
    &error_domain_posix,
#if X_ENABLE_SOCKET
    &error_domain_socket,
#endif
#if X_WINDOWS
    &error_domain_win32,
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
  static_cast<size_t>(2 + !!X_WINDOWS + !!X_ENABLE_SOCKET
      + !!X_ENABLE_CUDA_DRIVER + !!X_ENABLE_CUDA_RUNTIME + !!X_ENABLE_CUBLAS)
};

X_INL x_error x_ok()
{
  return x_error{};
}

X_INL bool x_fail(const x_error& error)
{
  return static_cast<bool>(error);
}

X_INL bool x_succ(const x_error& error)
{
  return !x_fail(error);
}

// x_error_capture{{{
X_INL int32_t _x_error_capture_posix()
{
  return static_cast<int32_t>(errno);
}

#if X_ENABLE_SOCKET
X_INL int32_t _x_error_capture_socket()
{
#if X_WINDOWS
  return static_cast<int32_t>(WSAGetLastError());
#else
  return static_cast<int32_t>(errno);
#endif
}
#endif  // X_ENABLE_SOCKET

#if X_WINDOWS
X_INL int32_t _x_error_capture_win32()
{
  return static_cast<int32_t>(GetLastError());
}
#endif  // X_WINDOWS
// x_error_capture}}}

// x_error_domain{{{
X_INL bool _x_error_failed_nonzero(const int32_t value)
{
  return value != 0;
}

X_INL const x_error_domain* _x_error_domain_lookup(const char* name)
{
  if (name == nullptr || name[0] == '\0') {
    return nullptr;
  }

  const x_error_domain* domain{nullptr};
  for (size_t i{0}; i < error_domain_registry.count; ++i) {
    domain = error_domain_registry.list[i];
    if (domain != nullptr && domain->name != nullptr
        && std::strcmp(name, domain->name) == 0) {
      return domain;
    }
  }

  return nullptr;
}

X_INL const x_error_domain* _x_error_domain_require(const char* name)
{
  const auto* domain = _x_error_domain_lookup(name);
  if (domain == nullptr) {
    std::fprintf(
        stderr, "Unknown error domain: \"%s\".\n",
        name == nullptr ? "(null)" : name);
    std::abort();
  }

  return domain;
}

X_INL char* _x_error_domain_message(
    char* buffer, const size_t size, const int32_t value,
    const x_error_domain* domain)
{
  if (buffer == nullptr || size == 0) {
    return buffer;
  }

  if (domain == nullptr || domain->message == nullptr) {
    return _x_error_number(buffer, size, "Unknown error", value);
  }

  return domain->message(buffer, size, value);
}
// x_error_domain}}}

// x_error_message{{{
X_INL char* _x_error_copy(char* buffer, const size_t size, const char* text)
{
  if (buffer == nullptr || size == 0) {
    return buffer;
  }

  const size_t len = std::strlen(text);
  const size_t cnt = len < size - 1 ? len : size - 1;
  if (cnt > 0) {
    std::memmove(buffer, text, cnt);
  }

  buffer[cnt] = '\0';

  return buffer;
}

X_INL char* _x_error_number(
    char* buffer, const size_t size,
    const char* prefix, const int32_t value)
{
  if (buffer == nullptr || size == 0) {
    return buffer;
  }

  // Format numeric fallbacks in sufficiently large storage, then truncate
  // explicitly. Small caller buffers are valid, including under -Werror.
  char msg[64]{};
  std::snprintf(msg, sizeof(msg), "%s %d", prefix, value);

  return _x_error_copy(buffer, size, msg);
}

X_INL char* _x_error_message_generic(
    char* buffer, const size_t size, const int32_t value)
{
  return _x_error_number(buffer, size, "Generic error", value);
}

X_INL char* _x_error_message_posix(
    char* buffer, const size_t size, const int32_t value)
{
  if (buffer == nullptr || size == 0) {
    return buffer;
  }

#if X_WINDOWS
  char msg[256]{};
  if (strerror_s(msg, sizeof(msg), value) != 0 || msg[0] == '\0') {
    _x_error_number(buffer, size, "Unknown error", value);
  } else {
    _x_error_copy(buffer, size, msg);
  }
#else
  buffer[0] = '\0';
#if defined(_GNU_SOURCE) && (defined(__GLIBC__) || X_CYGWIN)
  // GNU strerror_r may return immutable text instead of using buffer.
  const char* msg = strerror_r(value, buffer, size);
  if (msg == nullptr || msg[0] == '\0') {
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

#if X_ENABLE_SOCKET
X_INL char* _x_error_message_socket(
    char* buffer, const size_t size, const int32_t value)
{
#if X_WINDOWS
  return _x_error_message_win32(buffer, size, value);
#else
  return _x_error_message_posix(buffer, size, value);
#endif
}
#endif  // X_ENABLE_SOCKET

#if X_WINDOWS
X_INL char* _x_error_message_win32(
    char* buffer, const size_t size, const int32_t value)
{
  if (buffer == nullptr || size == 0) {
    return buffer;
  }

  DWORD len = FormatMessageA(
      FORMAT_MESSAGE_FROM_SYSTEM
      | FORMAT_MESSAGE_IGNORE_INSERTS
      | FORMAT_MESSAGE_MAX_WIDTH_MASK,
      nullptr, static_cast<DWORD>(value),
      MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), buffer,
      static_cast<DWORD>(size), nullptr);
  if (len == 0) {
    _x_error_number(buffer, size, "Win32 error", value);
  } else {
    while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
      buffer[--len] = '\0';
    }
  }

  return buffer;
}
#endif  // X_WINDOWS
// x_error_message}}}

// x_error_set{{{
X_INL x_error::x_error()
  :m_domain{&error_domain_posix}
{
}

X_INL x_error::x_error(const char* name)
{
  this->set(name);
}

X_INL x_error::x_error(const char* name, const int32_t value)
{
  this->set(name, value);
}

X_INL x_error::x_error(const x_error_domain& domain, const int32_t value)
{
  this->set(domain, value);
}

X_INL x_error::x_error(
    const char* name, const int32_t value, const char* message)
{
  this->set(name, value, message);
}

X_INL x_error& x_error::set(const char* name)
{
  const auto* domain = _x_error_domain_require(name);
  if (domain->capture == nullptr) {
    std::fprintf(
        stderr, "Error domain \"%s\" does not support error capturing.\n",
        domain->name);
    std::abort();
  }

  // Capture before touching string storage or doing any further library work.
  return this->set(*domain, domain->capture());
}

X_INL x_error& x_error::set(const char* name, const int32_t value)
{
  return this->set(*_x_error_domain_require(name), value);
}

X_INL x_error& x_error::set(const x_error_domain& domain, const int32_t value)
{
  this->m_domain = &domain;
  this->m_value = value;
  this->m_has_message = false;
  this->m_message.clear();

  return *this;
}

X_INL x_error& x_error::set(
    const char* name, const int32_t value, const char* message)
{
  // Copy first: message may refer to this value's previous message() text.
  std::string msg = message == nullptr ? "" : message;
  this->set(name, value);
  this->m_message = std::move(msg);
  this->m_has_message = message != nullptr;

  return *this;
}
// x_error_set}}}

X_INL const char* x_error::domain() const
{
  if (this->m_domain != nullptr && this->m_domain->name != nullptr) {
    return this->m_domain->name;
  } else {
    return "generic";
  }
}

X_INL const char* x_error::message() const
{
  if (!this->m_has_message) {
    char buf[256]{};
    this->message(buf, sizeof(buf));
    this->m_message = buf;
  }

  return this->m_message.c_str();
}

X_INL const char* x_error::message(char* buffer, const size_t size) const
{
  if (buffer == nullptr || size == 0) {
    return buffer;
  }

  if (this->m_has_message) {
    _x_error_copy(buffer, size, this->m_message.c_str());
  } else {
    const auto* domain = this->m_domain == nullptr
        ? &error_domain_generic : this->m_domain;
    _x_error_domain_message(buffer, size, this->m_value, domain);
  }

  return buffer;
}

X_INL int32_t x_error::value() const
{
  return this->m_value;
}

X_INL x_error::operator bool() const
{
  if (this->m_domain != nullptr && this->m_domain->failed != nullptr) {
    return this->m_domain->failed(this->m_value);
  } else {
    return this->m_value != 0;
  }
}

X_INL int32_t x_error::register_domain(const x_error_domain* domain)
{
  if (domain == nullptr || domain->name == nullptr || domain->name[0] == '\0') {
    return EINVAL;
  }

  if (_x_error_domain_lookup(domain->name) != nullptr) {
    return EEXIST;
  }

  if (error_domain_registry.count >= X_ERROR_DOMAIN_LIMIT) {
    return ENOSPC;
  }

  error_domain_registry.list[error_domain_registry.count++] = domain;

  return 0;
}

// Support optional assertion messages in strict C++17. The trailing dummy
// argument in x_assert keeps the selector's variadic tail nonempty.
#define _X_ASSERT_SELECT(_1, _2, function, ...) function
#define _X_ASSERT_ONE(expression) _X_ASSERT_TWO(expression, nullptr)
#define _X_ASSERT_TWO(expression, message) do { \
  if (!(expression)) { \
    _x_assert_fail( \
        #expression, (message), X_FILENAME, static_cast<long long>(__LINE__), \
        X_PRETTY_FUNCTION); \
  } \
} while (false)

X_INL void _x_assert_fail(
    const char* expression, const char* message,
    const char* file, const long long line, const char* function)
{
  std::fprintf(stderr, "Assertion failed: %s\n", expression);

  if (message != nullptr && message[0] != '\0') {
    std::fprintf(stderr, "Message: %s\n", message);
  }

  std::fprintf(stderr, "Position: %s:%lld: %s\n", file, line, function);

  std::abort();
}

template<typename Callable>
X_INL x_error _x_check_impl(
    const char* domain, Callable&& evaluate,
    const char* file, const char* expression, const long long line)
{
  x_error err;
  if constexpr (std::is_same_v<std::decay_t<decltype(evaluate())>, x_error>) {
    err = evaluate();
  } else {
    err.set(domain, static_cast<int32_t>(evaluate()));
  }

  if (x_fail(err)) {
#ifdef NDEBUG
    (void)file;
    (void)expression;
    (void)line;
    std::fprintf(stderr, "Check failed: %s\n", err.message());
#else
    std::fprintf(
        stderr, "Check failed: %s\nPosition: %s:%lld: %s\n",
        err.message(), file, line, expression);
#endif
  }

  return err;
}

// x_error_cuda_driver{{{
#if X_ENABLE_CUDA_DRIVER
X_INL bool _x_error_failed_cuda_driver(const int32_t value)
{
  return static_cast<CUresult>(value) != CUDA_SUCCESS;
}

X_INL char* _x_error_message_cuda_driver(
    char* buffer, const size_t size, const int32_t value)
{
  if (buffer == nullptr || size == 0) {
    return buffer;
  }

  const char* msg{nullptr};
  CUresult cres = cuGetErrorString(static_cast<CUresult>(value), &msg);
  if (cres != CUDA_SUCCESS || msg == nullptr) {
    _x_error_number(buffer, size, "CUDA driver error", value);
    return buffer;
  }

  _x_error_copy(buffer, size, msg);

  return buffer;
}
#endif  // X_ENABLE_CUDA_DRIVER
// x_error_cuda_driver}}}

// x_error_cuda_runtime{{{
#if X_ENABLE_CUDA_RUNTIME
X_INL bool _x_error_failed_cuda_runtime(const int32_t value)
{
  return static_cast<cudaError_t>(value) != cudaSuccess;
}

X_INL char* _x_error_message_cuda_runtime(
    char* buffer, const size_t size, const int32_t value)
{
  if (buffer == nullptr || size == 0) {
    return buffer;
  }

  const char* msg = cudaGetErrorString(static_cast<cudaError_t>(value));
  _x_error_copy(buffer, size, msg == nullptr ? "CUDA runtime error" : msg);

  return buffer;
}
#endif  // X_ENABLE_CUDA_RUNTIME
// x_error_cuda_runtime}}}

// x_error_cublas{{{
#if X_ENABLE_CUBLAS
X_INL bool _x_error_failed_cublas(const int32_t value)
{
  return static_cast<cublasStatus_t>(value) != CUBLAS_STATUS_SUCCESS;
}

X_INL char* _x_error_message_cublas(
    char* buffer, const size_t size, const int32_t value)
{
  if (buffer == nullptr || size == 0) {
    return buffer;
  }

  const char* msg = cublasGetStatusString(static_cast<cublasStatus_t>(value));
  _x_error_copy(buffer, size, msg == nullptr ? "cuBLAS error" : msg);

  return buffer;
}
#endif  // X_ENABLE_CUBLAS
// x_error_cublas}}}
// IMPL_Error_Handling}}}

// ************************************************************ IMPL_Logging{{{
#define _X_COLOR_WHITE   "\033[97m"
#define _X_COLOR_RED     "\033[91m"
#define _X_COLOR_GREEN   "\033[92m"
#define _X_COLOR_YELLOW  "\033[93m"
#define _X_COLOR_MAGENTA "\033[95m"
#define _X_COLOR_CYAN    "\033[96m"
#define _X_COLOR_RESET   "\033[0m"

template<unsigned char level>
X_INL void _x_log_prefix(
    char* buffer, const size_t size,
    const char* file, const char* function, long long line)
{
  char ts[26]{};

#ifdef NDEBUG
  (void)file;
  (void)function;
  (void)line;
  std::snprintf(
      buffer, size, "[%c %s] ",
      std::toupper(level), _x_timestamp(ts, sizeof(ts)));
#else
  std::snprintf(
      buffer, size, "[%c %s | %s - %s - %lld] ",
      std::toupper(level), _x_timestamp(ts, sizeof(ts)),
      file, function, line);
#endif
}

template<unsigned char level>
X_INL void _x_log_impl(
    const char* file, const char* function, const long long line,
    FILE* stream, const char* format, ...)
{
  int lvl{X_LOG_NONE};
  const char* color{""};

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

  char prefix[X_LOG_PREFIX_LIMIT]{};
  _x_log_prefix<level>(prefix, sizeof(prefix), file, function, line);

  char msg[X_LOG_MSG_LIMIT]{};
  std::va_list args;
  va_start(args, format);
  std::vsnprintf(msg, sizeof(msg), format, args);
  va_end(args);

  FILE* output = stream == nullptr ? stdout : stream;
  if (output == stdout || output == stderr) {
    std::fprintf(output, "%s%s%s%s\n", color, prefix, msg, _X_COLOR_RESET);
  } else {
    std::fprintf(output, "%s%s\n", prefix, msg);
  }
}
// IMPL_Logging}}}

// ********************************************************** IMPL_Checksums{{{
X_INL uint32_t x_checksum_crc32(
    const void* data, const size_t size, const uint32_t* previous)
{
  const auto* bytes{static_cast<const uint8_t*>(data)};
  uint32_t cks = previous == nullptr ? 0xFFFFFFFFu : ~*previous;

  if (bytes == nullptr) {
    return ~cks;
  }

  for (size_t i{0}; i < size; ++i) {
    cks ^= bytes[i];
    for (size_t bit{0}; bit < 8; ++bit) {
      cks = (cks >> 1) ^ (-static_cast<int32_t>(cks & 1u) & 0xEDB88320u);
    }
  }

  return ~cks;
}

X_INL uint16_t x_checksum_rfc1071(
    const void* data, const size_t size, const uint16_t* previous)
{
  const auto* bytes{static_cast<const uint8_t*>(data)};
  uint32_t cks = previous == nullptr ? 0u : static_cast<uint16_t>(~*previous);

  if (bytes == nullptr) {
    return static_cast<uint16_t>(~cks);
  }

  size_t idx{0};

  while (size - idx >= 2) {
    cks += (static_cast<uint32_t>(bytes[idx]) << 8) | bytes[idx + 1];
    cks = (cks & 0xFFFFu) + (cks >> 16);
    idx += 2;
  }

  if (idx < size) {
    cks += static_cast<uint32_t>(bytes[idx]) << 8;
  }

  while ((cks >> 16) != 0) {
    cks = (cks & 0xFFFFu) + (cks >> 16);
  }

  return static_cast<uint16_t>(~cks);
}

X_INL uint8_t x_checksum_xor(
    const void* data, const size_t size, const uint8_t* previous)
{
  const auto* bytes{static_cast<const uint8_t*>(data)};
  uint64_t cks = previous == nullptr ? 0u : static_cast<uint64_t>(*previous);

  if (bytes == nullptr) {
    return static_cast<uint8_t>(cks);
  }

  uint64_t blk[8]{};
  const size_t bsz{sizeof(blk)};
  size_t idx{0};

  while (size - idx >= bsz) {
    std::memcpy(blk, bytes + idx, sizeof(blk));

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

  return static_cast<uint8_t>(cks);
}
// IMPL_Checksums}}}

// ****************************************************** IMPL_Date_and_Time{{{
X_INL void x_sleep(const unsigned long millisecond)
{
#if X_WINDOWS
  Sleep(millisecond);
#else
  timespec req{
    static_cast<time_t>(millisecond / 1000),
    static_cast<long>((millisecond % 1000) * 1000000)
  };

  while (nanosleep(&req, &req) != 0 && errno == EINTR) {
  }
#endif
}

// x_event_internal{{{
[[noreturn]] X_INL void _x_event_throw(const x_error& error)
{
  throw std::runtime_error(std::string("x_event: ") + error.domain()
      + ":" + std::to_string(error.value()) + ": " + error.message());
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
  if (unit == nullptr || unit[0] == '\0' || std::strcmp(unit, "ns") == 0) {
    return 1.0;
  } else if (std::strcmp(unit, "us") == 0) {
    return 1e3;
  } else if (std::strcmp(unit, "ms") == 0) {
    return 1e6;
  } else if (std::strcmp(unit, "s") == 0) {
    return 1e9;
  } else if (std::strcmp(unit, "m") == 0) {
    return 6e10;
  } else if (std::strcmp(unit, "h") == 0) {
    return 3.6e12;
  }

  return 0.0;
}
// x_event_internal}}}

// x_event_agg{{{
X_INL void x_event_agg::echo(
    const char* unit, const char* title, FILE* stream) const
{
  char ts[26]{};

  const char* prefix = title == nullptr || title[0] == '\0'
      ? x_timestamp(ts, sizeof(ts)) : title;

  if (this->cyc == 0) {
    std::fprintf(
        stream == nullptr ? stdout : stream,
        "[%s] no samples (0 cycles)\n", prefix);
    return;
  }

  const char* suffix = unit == nullptr || unit[0] == '\0' ? "ns" : unit;

  double scale = _x_event_unit_scale(suffix);
  if (scale == 0.0) {
    suffix = "ns";
    scale = 1.0;
  }

  std::fprintf(
      stream == nullptr ? stdout : stream,
      "[%s] %f%s in %zu cycles - avg: %f%s, min(%zu): %f%s, max(%zu): %f%s\n",
      prefix, this->sum / scale, suffix, this->cyc, this->avg / scale, suffix,
      this->min.idx, this->min.val / scale, suffix,
      this->max.idx, this->max.val / scale, suffix);
}

X_INL void x_event_agg::reset()
{
  this->ready = false;
  this->cyc = 0;
  this->sum = 0.0;
  this->avg = 0.0;
  this->max = {0, -DBL_MAX};
  this->min = {0, DBL_MAX};
}
// x_event_agg}}}

// x_event{{{
X_INL x_event::x_event(const char* type, unsigned int flags)
{
  (void)flags;

  if (type == nullptr) {
    throw std::invalid_argument("x_event: type is null");
  }

  if (std::strcmp(type, "cpu") == 0) {
#if X_WINDOWS
    LARGE_INTEGER freq{};
    if (!QueryPerformanceFrequency(&freq)) {
      _x_event_throw(x_error("win32"));
    }

    if (freq.QuadPart <= 0) {
      throw std::invalid_argument("x_event: invalid frequency");
    }

    this->m_frequency = static_cast<int64_t>(freq.QuadPart);
#endif
    this->m_type = 0;
#if X_ENABLE_CUDA_DRIVER
  } else if (std::strcmp(type, "cu") == 0) {
    if ((flags & CU_EVENT_DISABLE_TIMING) != 0) {
      throw std::invalid_argument("x_event: CUDA driver timing is disabled");
    }

    this->m_type = this->m_start.type = this->m_stop.type = 1;
    CUevent handle{nullptr};
    CUresult cres = cuEventCreate(&handle, flags);
    if (cres == CUDA_SUCCESS) {
      this->m_start.cu = handle;
      handle = nullptr;
      cres = cuEventCreate(&handle, flags);
      if (cres == CUDA_SUCCESS) {
        this->m_stop.cu = handle;
      }
    }

    if (cres != CUDA_SUCCESS) {
      _x_event_throw(x_error("cu", static_cast<int32_t>(cres)));
    }
#endif
#if X_ENABLE_CUDA_RUNTIME
  } else if (std::strcmp(type, "cuda") == 0) {
    if ((flags & cudaEventDisableTiming) != 0) {
      throw std::invalid_argument("x_event: CUDA runtime timing is disabled");
    }

    this->m_type = this->m_start.type = this->m_stop.type = 2;
    cudaEvent_t handle{nullptr};
    cudaError_t cerr = cudaEventCreateWithFlags(&handle, flags);
    if (cerr == cudaSuccess) {
      this->m_start.cuda = handle;
      handle = nullptr;
      cerr = cudaEventCreateWithFlags(&handle, flags);
      if (cerr == cudaSuccess) {
        this->m_stop.cuda = handle;
      }
    }

    if (cerr != cudaSuccess) {
      _x_event_throw(x_error("cuda", static_cast<int32_t>(cerr)));
    }
#endif
  } else {
    throw std::invalid_argument(
        std::string("x_event: unsupported type ") + type);
  }
}

X_INL x_error x_event::agg(
    x_event_agg& aggregation, size_t cycle, void* stream, unsigned int flags)
{
  if (!_x_event_type_valid(this->m_type)) {
    return x_error("posix", EINVAL);
  }

  if (cycle == 0) {
    aggregation.reset();
    return x_ok();
  }

  // NOTE: If the statistics are ready, do not update them.
  if (aggregation.ready) {
    return x_ok();
  }

  if (aggregation.cyc == SIZE_MAX) {
    return x_error("posix", EOVERFLOW);
  }

  x_error err = this->toc(stream, flags);
  if (x_fail(err)) {
    return err;
  }

  const double dur = this->dur("ns");
  if (dur < 0.0) {
    return this->m_status;
  }

  // Only mutate statistics after obtaining a valid sample.
  if (aggregation.cyc == 0) {
    aggregation.reset();
  }

  if (dur > aggregation.max.val) {
    aggregation.max = {aggregation.cyc, dur};
  }

  if (dur < aggregation.min.val) {
    aggregation.min = {aggregation.cyc, dur};
  }

  aggregation.sum += dur;
  aggregation.cyc += 1;
  aggregation.avg = aggregation.sum / static_cast<double>(aggregation.cyc);

  if (aggregation.cyc % cycle == 0) {
    aggregation.ready = true;
  }

  return x_ok();
}

X_INL double x_event::dur(const char* unit) const
{
  if (!_x_event_type_valid(this->m_type)) {
    return -1.0;
  }

  const double scale = _x_event_unit_scale(unit);
  if (scale == 0.0) {
    return -1.0;
  }

  if (!this->m_started || !this->m_stopped) {
    if (x_succ(this->m_status)) {
      this->m_status.set("posix", ENODATA);
    }

    return -1.0;
  }

  x_error err;
  double ns{-1.0};

  switch (this->m_type) {
    case 0:
      err = this->dur_cpu(ns);
      break;
#if X_ENABLE_CUDA_DRIVER
    case 1:
      err = this->dur_cuda_driver(ns);
      break;
#endif
#if X_ENABLE_CUDA_RUNTIME
    case 2:
      err = this->dur_cuda_runtime(ns);
      break;
#endif
    default:
      return -1.0;
  }

  this->m_status = err;
  if (x_fail(err)) {
    return -1.0;
  }

  return ns / scale;
}

X_INL const x_error& x_event::status() const noexcept
{
  return this->m_status;
}

X_INL x_error x_event::tic(void* stream, unsigned int flags)
{
  return this->record(false, stream, flags);
}

X_INL x_error x_event::toc(void* stream, unsigned int flags)
{
  return this->record(true, stream, flags);
}

X_INL x_event::endpoint::~endpoint() noexcept
{
#if X_ENABLE_CUDA_DRIVER
  if (this->type == 1 && this->cu != nullptr) {
    (void)cuEventDestroy_v2(this->cu);
  }
#endif

#if X_ENABLE_CUDA_RUNTIME
  if (this->type == 2 && this->cuda != nullptr) {
    (void)cudaEventDestroy(this->cuda);
  }
#endif
}

// Backend helpers write nanoseconds only on success. Sample state and units
// are checked by dur(); these helpers do not mutate m_status.
X_INL x_error x_event::dur_cpu(double& ns) const
{
  double dur{0.0};

#if X_WINDOWS
  if (this->m_frequency <= 0) {
    return x_error("posix", EINVAL);
  }

  if (this->m_stop.counter < this->m_start.counter) {
    return x_error("posix", ERANGE);
  }

  // Subtract as unsigned integers to avoid signed overflow, then convert the
  // difference rather than rounding the two absolute counts to double.
  const uint64_t diff = static_cast<uint64_t>(this->m_stop.counter)
                      - static_cast<uint64_t>(this->m_start.counter);
  dur = static_cast<double>(diff)
      * (1e9 / static_cast<double>(this->m_frequency));
#else
  // difftime avoids overflowing time_t subtraction before conversion.
  dur = std::difftime(this->m_stop.cpu.tv_sec, this->m_start.cpu.tv_sec) * 1e9
      + static_cast<double>(this->m_stop.cpu.tv_nsec)
      - static_cast<double>(this->m_start.cpu.tv_nsec);
#endif

  if (std::isnan(dur)) {
    return x_error("posix", EDOM);
  }

  if (!std::isfinite(dur) || dur < 0.0) {
    return x_error("posix", ERANGE);
  }

  ns = dur;

  return x_ok();
}

#if X_ENABLE_CUDA_DRIVER
X_INL x_error x_event::dur_cuda_driver(double& ns) const
{
  CUresult cres = cuEventSynchronize(this->m_stop.cu);
  if (cres != CUDA_SUCCESS) {
    return x_error("cu", static_cast<int32_t>(cres));
  }

  float ms{0.0f};
  cres = cuEventElapsedTime(&ms, this->m_start.cu, this->m_stop.cu);
  if (cres != CUDA_SUCCESS) {
    return x_error("cu", static_cast<int32_t>(cres));
  }

  if (std::isnan(ms)) {
    return x_error("posix", EDOM);
  }

  if (!std::isfinite(ms) || ms < 0.0f) {
    return x_error("posix", ERANGE);
  }

  ns = static_cast<double>(ms) * 1e6;

  return x_ok();
}
#endif  // X_ENABLE_CUDA_DRIVER

#if X_ENABLE_CUDA_RUNTIME
X_INL x_error x_event::dur_cuda_runtime(double& ns) const
{
  cudaError_t cerr = cudaEventSynchronize(this->m_stop.cuda);
  if (cerr != cudaSuccess) {
    return x_error("cuda", static_cast<int32_t>(cerr));
  }

  float ms{0.0f};
  cerr = cudaEventElapsedTime(&ms, this->m_start.cuda, this->m_stop.cuda);
  if (cerr != cudaSuccess) {
    return x_error("cuda", static_cast<int32_t>(cerr));
  }

  if (std::isnan(ms)) {
    return x_error("posix", EDOM);
  }

  if (!std::isfinite(ms) || ms < 0.0f) {
    return x_error("posix", ERANGE);
  }

  ns = static_cast<double>(ms) * 1e6;

  return x_ok();
}
#endif  // X_ENABLE_CUDA_RUNTIME

X_INL x_error x_event::record(bool stop, void* stream, unsigned int flags)
{
  if (!_x_event_type_valid(this->m_type)) {
    return x_error("posix", EINVAL);
  }

  if (stop && !this->m_started) {
    if (x_fail(this->m_status)) {
      return this->m_status;
    }

    return x_error("posix", ENODATA);
  }

  this->m_stopped = false;
  if (!stop) {
    this->m_started = false;
  }

  (void)stream;
  (void)flags;

  x_error err;
  endpoint& target = stop ? this->m_stop : this->m_start;

  switch (this->m_type) {
    case 0:
      err = this->record_cpu(target);
      break;
#if X_ENABLE_CUDA_DRIVER
    case 1:
      err = this->record_cuda_driver(target, stream, flags);
      break;
#endif
#if X_ENABLE_CUDA_RUNTIME
    case 2:
      err = this->record_cuda_runtime(target, stream, flags);
      break;
#endif
    default:
      err = x_error("posix", EINVAL);
      break;
  }

  if (x_succ(err)) {
    if (stop) {
      this->m_stopped = true;
    } else {
      this->m_started = true;
    }
  }

  this->m_status = err;

  return err;
}

X_INL x_error x_event::record_cpu(endpoint& target)
{
#if X_WINDOWS
  LARGE_INTEGER counter{};
  if (!QueryPerformanceCounter(&counter)) {
    return x_error("win32");
  }

  target.counter = static_cast<int64_t>(counter.QuadPart);
#else
  timespec ts{};
  if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
    return x_error("posix");
  }

  target.cpu = ts;
#endif

  return x_ok();
}

#if X_ENABLE_CUDA_DRIVER
X_INL x_error x_event::record_cuda_driver(
    endpoint& target, void* stream, unsigned int flags)
{
  CUresult cres =
    cuEventRecordWithFlags(target.cu, static_cast<CUstream>(stream), flags);
  if (cres != CUDA_SUCCESS) {
    return x_error("cu", static_cast<int32_t>(cres));
  }

  return x_ok();
}
#endif  // X_ENABLE_CUDA_DRIVER

#if X_ENABLE_CUDA_RUNTIME
X_INL x_error x_event::record_cuda_runtime(
    endpoint& target, void* stream, unsigned int flags)
{
  cudaError_t cerr =
    cudaEventRecordWithFlags(
        target.cuda, static_cast<cudaStream_t>(stream), flags);
  if (cerr != cudaSuccess) {
    return x_error("cuda", static_cast<int32_t>(cerr));
  }

  return x_ok();
}
#endif  // X_ENABLE_CUDA_RUNTIME
// x_event}}}
// IMPL_Date_and_Time}}}

// ******************************************************** IMPL_File_System{{{
X_INL bool x_fexist(const char* file)
{
  if (file == nullptr) {
    return false;
  }

#if X_WINDOWS
  struct _stat64 st{};
  return _stat64(file, &st) == 0;
#else
  struct stat st{};
  return stat(file, &st) == 0;
#endif
}

X_INL x_error x_fopen(FILE** stream, const char* file, const char* mode)
{
  if (stream == nullptr
      || file == nullptr || file[0] == '\0'
      || mode == nullptr || mode[0] == '\0') {
    return x_error("posix", EINVAL);
  }

#if X_WINDOWS
  const errno_t perr = fopen_s(stream, file, mode);
  return perr == 0 ? x_ok() : x_error("posix", static_cast<int32_t>(perr));
#else
  *stream = std::fopen(file, mode);
  return *stream == nullptr ? x_error("posix") : x_ok();
#endif
}

X_INL const char* x_fpath(
    char* destination, const size_t size, const char* source)
{
  if (destination == nullptr || size == 0
      || source == nullptr || source[0] == '\0') {
    errno = EINVAL;
    return nullptr;
  }

#if X_WINDOWS
  char* path = _fullpath(nullptr, source, size);
#else
  char* path = realpath(source, nullptr);
#endif
  if (path == nullptr) {
    return nullptr;
  }

  const size_t len = std::strlen(path);
  if (len >= size) {
    std::free(path);
    errno = ENOBUFS;
    return nullptr;
  }

  std::memcpy(destination, path, len + 1);
  std::free(path);

  return destination;
}

X_INL int64_t x_fsize(const char* file)
{
  if (file == nullptr) {
    return -1;
  }

#if X_WINDOWS
  struct _stat64 st{};
  return _stat64(file, &st) == 0 ? static_cast<int64_t>(st.st_size) : -1;
#else
  struct stat st{};
  return stat(file, &st) == 0 ? static_cast<int64_t>(st.st_size) : -1;
#endif
}
// IMPL_File_System}}}

// *********************************************************** IMPL_Hardware{{{
X_INL size_t x_ncpu()
{
#if X_WINDOWS
#if defined(_WIN32_WINNT) && _WIN32_WINNT >= 0x0601
  return static_cast<size_t>(
      GetActiveProcessorCount(ALL_PROCESSOR_GROUPS));
#else
  SYSTEM_INFO info{};
  GetSystemInfo(&info);
  return static_cast<size_t>(info.dwNumberOfProcessors);
#endif
#else
  const long cnt = sysconf(_SC_NPROCESSORS_ONLN);
  return cnt < 0 ? 0 : static_cast<size_t>(cnt);
#endif
}
// IMPL_Hardware}}}

// ******************************************************** IMPL_Mathematics{{{
template<typename T>
X_INL constexpr T x_KiB(T scale)
{
  return scale * static_cast<T>(1024);
}

template<typename T>
X_INL constexpr T x_MiB(T scale)
{
  return scale * static_cast<T>(1048576);
}

template<typename T>
X_INL constexpr T x_GiB(T scale)
{
  return scale * static_cast<T>(1073741824);
}

template<typename T>
X_INL constexpr T x_TiB(T scale)
{
  return scale * static_cast<T>(1099511627776);
}

template<typename T>
X_INL constexpr T x_PiB(T scale)
{
  return scale * static_cast<T>(1125899906842624);
}

template<typename T>
X_INL constexpr uint64_t x_bit(T bit)
{
  static_assert(
      std::is_integral_v<T> && !std::is_same_v<T, bool>,
      "x_bit requires an integer index");

  return UINT64_C(1) << bit;
}

template<typename T>
X_INL constexpr bool _x_math_negative(T value)
{
  (void)value;
  static_assert(
      std::is_integral_v<T> && !std::is_same_v<T, bool>,
      "An integral type other than bool is required.");

  if constexpr (std::is_signed_v<T>) {
    return value < 0;
  }

  return false;
}

template<typename T>
X_INL constexpr T x_gcd(T m, T n)
{
  if (_x_math_negative(m) || _x_math_negative(n)) {
    errno = EINVAL;
    return 0;
  }

  while (n != 0) {
    const T rest = m % n;
    m = n;
    n = rest;
  }

  return m;
}

template<typename T>
X_INL constexpr T x_lcm(T m, T n)
{
  if (_x_math_negative(m) || _x_math_negative(n)) {
    errno = EINVAL;
    return 0;
  }

  if (m == 0 || n == 0) {
    return 0;
  }

  const T factor = m / x_gcd(m, n);
  if (factor > (std::numeric_limits<T>::max)() / n) {
    errno = ERANGE;
    return 0;
  }

  return factor * n;
}

template<typename T>
X_INL constexpr T x_next_exp(T base, T source)
{
  if (base < 2 || _x_math_negative(base) || _x_math_negative(source)) {
    errno = EINVAL;
    return 0;
  }

  T power{1};
  while (power < source) {
    if (power > (std::numeric_limits<T>::max)() / base) {
      errno = ERANGE;
      return 0;
    }

    power *= base;
  }

  return power;
}

template<typename T>
X_INL constexpr T x_next_mul(T base, T source)
{
  if (base == 0 || _x_math_negative(base) || _x_math_negative(source)) {
    errno = EINVAL;
    return 0;
  }

  const T step = base - source % base;
  if (source > (std::numeric_limits<T>::max)() - step) {
    errno = ERANGE;
    return 0;
  }

  return source + step;
}

template<typename T>
X_INL constexpr T x_prev_exp(T base, T source)
{
  if (base < 2 || _x_math_negative(base) || _x_math_negative(source)) {
    errno = EINVAL;
    return 0;
  }

  if (source == 0) {
    return 0;
  }

  T power{1};
  const T limit = source / base;
  while (power <= limit) {
    // The quotient bound guarantees power * base <= source, without overflow.
    power *= base;
  }

  return power;
}

template<typename T>
X_INL constexpr T x_prev_mul(T base, T source)
{
  if (base == 0 || _x_math_negative(base) || _x_math_negative(source)) {
    errno = EINVAL;
    return 0;
  }

  return source - source % base;
}
// IMPL_Mathematics}}}

// ************************************************************* IMPL_Memory{{{
template<typename T, size_t N>
X_INL constexpr size_t x_count(const T (&)[N])
{
  return N;
}

template<bool array, typename T>
X_INL void x_delete(T*& pointer)
{
  if constexpr (array) {
    delete[] pointer;
  } else {
    delete pointer;
  }

  pointer = nullptr;
}

template<typename T>
X_INL void x_free(T*& pointer)
{
  if (pointer != nullptr) {
    std::free(const_cast<void*>(static_cast<const volatile void*>(pointer)));
    pointer = nullptr;
  }
}

template<typename T>
X_INL x_error x_malloc(T** pointer, const size_t size)
{
  if (pointer == nullptr || *pointer != nullptr || size == 0) {
    return x_error("posix", EINVAL);
  }

  *pointer = static_cast<T*>(std::malloc(size));

  return *pointer == nullptr ? x_error("posix") : x_ok();
}

X_INL x_error x_memcpy(
    void* destination, const void* source, const size_t size)
{
  if (size == 0) {
    return x_ok();
  }

  if (destination == nullptr || source == nullptr) {
    return x_error("posix", EINVAL);
  }

  const uintptr_t dst{reinterpret_cast<uintptr_t>(destination)};
  const uintptr_t src{reinterpret_cast<uintptr_t>(source)};

  // Subtract addresses to avoid overflowing either range endpoint.
  if ((dst >= src && dst - src < size) || (dst < src && src - dst < size)) {
    return x_error("posix", EINVAL);
  }

  // Reject overlap first. `memmove()` avoids optimizer `restrict` warnings
  // on rejected calls whose integer-address guard is not understood by GCC.
  std::memmove(destination, source, size);

  return x_ok();
}

X_INL x_error _x_meminfo_cpu(size_t* available, size_t* total)
{
  if (available == nullptr && total == nullptr) {
    return x_error("posix", EINVAL);
  }

  size_t avail_size{0};
  size_t total_size{0};

#if X_LINUX
  struct sysinfo info{};
  if (sysinfo(&info) != 0) {
    return x_error("posix");
  }

  if (info.mem_unit == 0
      || (available != nullptr && info.freeram > SIZE_MAX / info.mem_unit)
      || (total != nullptr && info.totalram > SIZE_MAX / info.mem_unit)) {
    return x_error("posix", ERANGE);
  }

  if (available != nullptr) {
    avail_size = static_cast<size_t>(info.freeram) * info.mem_unit;
  }
  if (total != nullptr) {
    total_size = static_cast<size_t>(info.totalram) * info.mem_unit;
  }
#elif X_MACOS
  if (total != nullptr) {
    uint64_t mem{0};
    size_t msz{sizeof(mem)};
    if (sysctlbyname("hw.memsize", &mem, &msz, nullptr, 0) != 0) {
      return x_error("posix");
    }
    if (msz != sizeof(mem)) {
      return x_error("posix", EIO);
    }
    if (mem > SIZE_MAX) {
      return x_error("posix", ERANGE);
    }
    total_size = static_cast<size_t>(mem);
  }

  if (available != nullptr) {
    const host_t host = mach_host_self();
    vm_size_t page_size{0};
    vm_statistics64_data_t info{};
    mach_msg_type_number_t cnt{HOST_VM_INFO64_COUNT};

    kern_return_t ret = host_page_size(host, &page_size);
    if (ret == KERN_SUCCESS) {
      ret = host_statistics64(host, HOST_VM_INFO64,
          reinterpret_cast<host_info64_t>(&info), &cnt);
    }

    const kern_return_t rel_ret = mach_port_deallocate(mach_task_self(), host);
    if (ret == KERN_SUCCESS) {
      ret = rel_ret;
    }
    if (ret != KERN_SUCCESS) {
      return x_error("generic", static_cast<int32_t>(ret));
    }
    if (page_size == 0 || info.free_count > SIZE_MAX / page_size) {
      return x_error("posix", ERANGE);
    }

    // free_count already includes speculative_count; do not count it twice.
    avail_size = static_cast<size_t>(info.free_count)
        * static_cast<size_t>(page_size);
  }
#elif X_WINDOWS
  MEMORYSTATUSEX info{};
  info.dwLength = sizeof(info);
  if (!GlobalMemoryStatusEx(&info)) {
    return x_error("win32");
  }

  if ((available != nullptr && info.ullAvailPhys > SIZE_MAX)
      || (total != nullptr && info.ullTotalPhys > SIZE_MAX)) {
    return x_error("posix", ERANGE);
  }

  if (available != nullptr) {
    avail_size = static_cast<size_t>(info.ullAvailPhys);
  }
  if (total != nullptr) {
    total_size = static_cast<size_t>(info.ullTotalPhys);
  }
#else
  return x_error("posix", ENOTSUP);
#endif

  if (available != nullptr) {
    *available = avail_size;
  }
  if (total != nullptr) {
    *total = total_size;
  }

  return x_ok();
}

#if X_ENABLE_CUDA_DRIVER
X_INL x_error _x_meminfo_cuda_driver(size_t* available, size_t* total)
{
  if (available == nullptr && total == nullptr) {
    return x_error("posix", EINVAL);
  }

  size_t avail_size{0};
  size_t total_size{0};
  const x_error err{"cu", static_cast<int32_t>(
      cuMemGetInfo_v2(&avail_size, &total_size))};
  if (x_fail(err)) {
    return err;
  }

  if (available != nullptr) {
    *available = avail_size;
  }
  if (total != nullptr) {
    *total = total_size;
  }

  return err;
}
#endif

#if X_ENABLE_CUDA_RUNTIME
X_INL x_error _x_meminfo_cuda_runtime(size_t* available, size_t* total)
{
  if (available == nullptr && total == nullptr) {
    return x_error("posix", EINVAL);
  }

  size_t avail_size{0};
  size_t total_size{0};
  const x_error err{"cuda", static_cast<int32_t>(
      cudaMemGetInfo(&avail_size, &total_size))};
  if (x_fail(err)) {
    return err;
  }

  if (available != nullptr) {
    *available = avail_size;
  }
  if (total != nullptr) {
    *total = total_size;
  }

  return err;
}
#endif

X_INL x_error x_meminfo(
    const char* type, size_t* available, size_t* total)
{
  if (type == nullptr) {
    return x_error("posix", EINVAL);
  }

  if (std::strcmp(type, "cpu") == 0) {
    return _x_meminfo_cpu(available, total);
  }

#if X_ENABLE_CUDA_DRIVER
  if (std::strcmp(type, "cu") == 0) {
    return _x_meminfo_cuda_driver(available, total);
  }
#endif

#if X_ENABLE_CUDA_RUNTIME
  if (std::strcmp(type, "cuda") == 0) {
    return _x_meminfo_cuda_runtime(available, total);
  }
#endif

  return x_error("posix", ENOTSUP);
}
// IMPL_Memory}}}

// ************************************************************* IMPL_String{{{
X_INL x_error x_strcpy(
    char* destination, const size_t size, const char* source)
{
  if (destination == nullptr || source == nullptr || size == 0) {
    return x_error("posix", EINVAL);
  }

  const size_t src_size = std::strlen(source);
  const size_t cpy_size = src_size < size - 1 ? src_size : size - 1;
  const uintptr_t dst{reinterpret_cast<uintptr_t>(destination)};
  const uintptr_t src{reinterpret_cast<uintptr_t>(source)};

  // Include both terminators; subtract addresses to avoid endpoint overflow.
  if ((dst >= src && dst - src <= src_size)
      || (dst < src && src - dst <= cpy_size)) {
    return x_error("posix", EINVAL);
  }

  if (cpy_size != 0) {
    // As in `x_memcpy`, `memmove()` avoids spurious `restrict` warnings.
    std::memmove(destination, source, cpy_size);
  }

  destination[cpy_size] = '\0';

  return x_ok();
}

X_INL bool x_strmty(const char* string)
{
  return string == nullptr || string[0] == '\0';
}
// IMPL_String}}}

// *************************************************************** IMPL_CUDA{{{
#if X_ENABLE_CUDA_DRIVER
X_INL const char* _x_memtype_cuda_driver(CUdeviceptr pointer)
{
  unsigned int attr{0};
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
  struct cudaPointerAttributes attr{};
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
template<typename T>
X_INL const char* x_memtype(const char* api, T pointer)
{
  (void)pointer;
  if (api == nullptr || api[0] == '\0') {
    return "N/A";
  }

#if X_ENABLE_CUDA_DRIVER
  if constexpr (std::is_same_v<T, CUdeviceptr>) {
    if (std::strcmp(api, "cu") == 0) {
      return _x_memtype_cuda_driver(pointer);
    }
  }
#endif

#if X_ENABLE_CUDA_RUNTIME
  if constexpr (std::is_same_v<T, std::nullptr_t>
      || (std::is_pointer_v<T>
          && !std::is_function_v<std::remove_pointer_t<T>>)) {
    if (std::strcmp(api, "cuda") == 0) {
      // The API examines the address, not the pointed-to object. Removing
      // volatile for this query does not read or modify volatile storage.
      return _x_memtype_cuda_runtime(
          const_cast<const void*>(static_cast<const volatile void*>(pointer)));
    }
  }
#endif

  return "N/A";
}

X_INL size_t x_ngpu(const char* api)
{
  if (api == nullptr || api[0] == '\0') {
    return 0;
  }

#if X_ENABLE_CUDA_DRIVER
  if (std::strcmp(api, "cu") == 0) {
    int cnt{0};
    const CUresult cres = cuDeviceGetCount(&cnt);
    if (cres != CUDA_SUCCESS) {
      const char* msg{nullptr};
      const CUresult msg_cres = cuGetErrorString(cres, &msg);
      if (msg_cres == CUDA_SUCCESS && msg != nullptr) {
        std::fprintf(stderr, "cuDeviceGetCount: %s\n", msg);
      } else {
        std::fprintf(
            stderr, "cuDeviceGetCount: CUDA driver error %d\n",
            static_cast<int>(cres));
      }
      return 0;
    }

    return cnt < 0 ? 0 : static_cast<size_t>(cnt);
  }
#endif

#if X_ENABLE_CUDA_RUNTIME
  if (std::strcmp(api, "cuda") == 0) {
    int cnt{0};
    const cudaError_t cerr = cudaGetDeviceCount(&cnt);
    if (cerr != cudaSuccess) {
      const char* msg = cudaGetErrorString(cerr);
      std::fprintf(
          stderr, "cudaGetDeviceCount: %s\n",
          msg == nullptr ? "CUDA runtime error" : msg);
      return 0;
    }

    return cnt < 0 ? 0 : static_cast<size_t>(cnt);
  }
#endif

  return 0;
}
#endif  // (X_ENABLE_CUDA_DRIVER || X_ENABLE_CUDA_RUNTIME)
// IMPL_CUDA}}}

#endif  // X_H
