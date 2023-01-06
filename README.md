<!------------------------------------------------------------------ HEADER -->
<pre align="center">
  █▀▀▄ ▄▀▀█    █▀▀▀█▀▀▀█
  ▀▀▄ ▀ ▄▀▀    █   █   █
     █ █       █       █
  ▄▄▀ ▄ ▀▄▄    █   █   █
  █▄▄▀ ▀▄▄█ ██ █▄▄▄█▄▄▄█
</pre>


<p align="center">
  <img src="https://img.shields.io/badge/C-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white" alt="C">
  <img src="https://img.shields.io/badge/C++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++">

  <a href="./LICENSE">
    <img src="https://img.shields.io/badge/license-mulax_psl_v2-green.svg?style=for-the-badge" alt="license-mulan-psl-v2">
  </a>
</p>

<p align="center">
  Cross platform C/C++ utilities.
</p>


<!------------------------------------------------------- TABLE OF CONTENTS -->
<details open="open">
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#documentation">Documentation</a>
      <ul>
        <li><a href="#compiler-detection">Compiler Detection</a></li>
        <li><a href="#os-detection">OS Detection</a></li>
        <li><a href="#platform-detection">Platform Detection</a></li>
        <li><a href="#x_export-x_import">X_EXPORT, X_IMPORT</a></li>
        <li><a href="#x_key">X_KEY</a></li>
        <li><a href="#x_assert">x_assert</a></li>
        <li><a href="#x_bit">x_bit</a></li>
        <li><a href="#x_count">x_count</a></li>
        <li><a href="#x_pi">x_pi</a></li>
        <li><a href="#x_cpu_count">x_cpu_count</a></li>
        <li><a href="#x_duration">x_duration</a></li>
        <li><a href="#x_file_size">x_file_size</a></li>
        <li><a href="#x_full_path">x_full_path</a></li>
        <li><a href="#x_getch">x_getch</a></li>
        <li><a href="#x_now">x_now</a></li>
        <li><a href="#x_path_exists">x_path_exists</a></li>
        <li><a href="#x_splitpath">x_splitpath</a></li>
        <li><a href="#x_sleep">x_sleep</a></li>
        <li><a href="#x_string_empty">x_string_empty</a></li>
        <li><a href="#x_timestamp">x_timestamp</a></li>
        <li><a href="#x_log">x_log</a></li>
      </ul>
    </li>
    <li><a href="#license">LICENSE</a></li>
  </ol>
</details>


<!--------------------------------------------------------- GETTING STARTED -->
## Getting Started
### Installation
Put `x.h` into your project.


### Usage
`x.h` is a single header file following the style of the
[stb libraries](https://github.com/nothings/stb), which means users must define
the macro `X_H_IMPLEMENTATION` in one and only one of their source files:

```c
// in some foo.c file
#define X_H_IMPLEMENTATION
#include "x.h"

// in some bar.h or bar.c file
#include "x.h"
```


<!----------------------------------------------------------- DOCUMENTATION -->
## Documentation
### Compiler Detection
- X_IS_CLANG: The compiler being used is Clang.
- X_IS_GCC: The compiler being used is GCC.
- X_IS_MSVC: The compiler being used is MSVC.


### OS Detection
- X_IS_CYGWIN: The OS is Cygwin.
- X_IS_LINUX: The OS is Linux.
- X_IS_WINDOWS: The OS is Windows (both 32 bit and 64 bit).


### Platform Detection
- X_IS_ANDROID: The platform is Android.
- X_IS_MINGW: The platform is Mingw (both 32 bit and 64 bit).
- X_IS_MINGW64: The platform is Mingw64.


### X_EXPORT, X_IMPORT
```c
#ifndef X_EXPORT
#if X_IS_WINDOWS
#define X_EXPORT __declspec(dllexport)
#else
#define X_EXPORT __attribute__ ((visibility("default")))
#endif
#endif

#ifndef X_IMPORT
#if X_IS_WINDOWS
#define X_IMPORT __declspec(dllimport)
#else
#define X_IMPORT __attribute__ ((visibility("hidden")))
#endif
#endif
```

See [Microsoft Docs](https://docs.microsoft.com/en-us/cpp/cpp/dllexport-dllimport?view=msvc-170)
and [GCC Wiki](https://gcc.gnu.org/wiki/Visibility) for more details.


### X_KEY
```cpp
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
```

Several virtual key macros, most of which are defined as their corresponding
[ASCII code](https://www.asciitable.com). However, the arrow keys are handled
differently among OSs (please tell me if this statement is not accurate), they
are defined as negative integers to work around. Please checkout
<a href="#x_getch">x_getch</a> for their usage.


### x_assert
An macro enables the assertion functionality for both debug and release builds.
For debug build, it is the same as the assertion macro in assert.h/cassert
header.

```c
x_assert(expr)
```

#### Parameters
- `expr`: The assertion expression.

#### Examples
```c
x_assert(1 == 1);
x_assert(1 != 1);
```


### x_bit
A macro used to generate an integer with only the n-th bit set to 1. This is
useful when one needs enumerations like `0b0001`, `0b0010`, `0b0100` to perform
the `&`, `|`, `~` operations.

```c
T x_bit(bit)  // In this document, T indicats a return type from a macro.
```

#### Parameters
- `bit`: Specification for which bit of the integer should be set to 1.

#### Examples
```c
enum
{
  FLAG_NONE   = 0,
  FLAG_READ   = x_bit(1),
  FLAG_WRITE  = x_bit(2),
};

int read_and_write = FLAG_READ | FLAG_WRITE;
```

#### Returns
- The generated integer, may be `int`, `long`, etc.


### x_count
Get length of an array. This macro does not work with those arrays declared
with `malloc` or `new`, nor throw any exceptions.

```c
T x_count(array)
```

For C++, it is implemented as a templated function:

```cpp
template<class T, size_t N>
size_t x_count(const T (&array)[N])
{
  return N;
}
```

#### Parameters
- `array`: The array of which the size to be queried.

#### Returns
- The length (count of elements) of the input array.

#### Examples
```c
int a[5] = {1, 2, 3, 4, 5};

for (size_t i = 0; i < x_count(a); ++i)
{
  printf("%d", a[i]);
}
```


### x_pi
A macro for retrieving the constant [Pi](https://en.wikipedia.org/wiki/Pi). The
input argument is used to specify the target data type.

```c
T x_pi(T)
```

In C++, one may use the following template:

```cpp
template<class T>
static constexpr T x_pi = (T)3.141592653589793238462643383279502884197169399375;
// Usage: x_pi<float>, x_pi<double>, etc.
```

#### Parameters
- `T`: Target type, e.g. `float`, `double`, `long`.

#### Returns
- Pi of specified type.

#### Examples
```c
float a = x_pi(float);
double b = x_pi(double);
```


### x_cpu_count
```c
size_t x_cpu_count()
```

#### Returns
- Count of CPU of current host.


### x_duration
```c
double x_duration(const struct timespec start, const struct timespec end, const char* unit)
```

#### Parameters
- `start`: The start time point.
- `end`: The end time point.
- `unit`: The unit of the outcome.

#### Returns
- The duration of the two time points in specified unit.


### x_file_size
```c
long long x_file_size(const char* file)
```

#### Parameters
- `file`: File name of which the size to be exam.

#### Returns
- Size of the file in bytes.


### x_full_path
```c
const char* x_full_path(char* dst, const char* src)
```

#### Parameters
- `dst`: The full/absolute path to the input file. It should be allocated with
sufficient memory before passed into this function, otherwise `NULL` is returned.
- `src`: The (maybe relative) path to a file.

#### Returns
- The same value contained in `dst`, or `NULL` when the operation failed.

#### Examples
```c
const char* file = "./foo/bar.txt";
char buf[X_PATH_MAX] = {0};
x_full_path(buf, file);
```


### x_getch
A cross-platform version of Win32's
[\_getch](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/getch-getwch?view=msvc-170) function.

```c
int x_getch()
```

#### Returns
- The pressed key.

#### Examples
This function only works with console programs. One may use it to break a
infinity loop manually:

```c
int main(int argc, char** argv)
{
  while (true)
  {
    if (x_getch() == X_KEY_ESC)
    {
      break;  // When the 'Esc' key is pressed, the loop breaks.
    }

    x_sleep(10);
  }

  return 0;
}
```


### x_now
```c
struct timespec x_now();
```

#### Returns
- A `struct timespec` instance indicating the current time.


### x_path_exists
```c
bool x_path_exists(const char* path)
```

#### Parameters
- `path`: The path to a querying file/directory.

#### Returns
- `true` if the file/directory exists.


### x_splitpath
A cross-platform version of Win32's
[\_splitpath](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/splitpath-s-wsplitpath-s?view=msvc-170) function.

```c
int x_splitpath(
    const char *path,
    char *root, const size_t rsz, char *dir, const size_t dsz,
    char *file, const size_t fsz, char *ext, const size_t esz)
```


### x_sleep
```c
void x_sleep(const unsigned long ms)
```

#### Parameters
- `ms`: The length of the sleep duration in milliseconds.


### x_string_empty
```c
bool x_string_empty(const char* string)
```

#### Parameters
- `string`: The string.

#### Returns
- `true` if the string is NULL or '\0' is the only thing it contains.


### x_timestamp
```c
const char* x_timestamp(char* buffer, const size_t size)
```

#### Parameters
- `buffer`: Buffer to store timestamp string.
- `size`: Size of the buffer.

#### Returns
- Same content of the input buffer. A empty string ("") will be returned if the
operation failed.


### x_log
```c
x_log(char level, FILE* file, const char* format, ...)
```

#### Parameters
- `level`: Logging level specification, available values are `P` (Public),
`F` (Fatal), `E` (Error), `W` (Warning), `I` (Info), `D` (Debug) and they are
**case insensitive**.
- `file`: File stream for saving the loggings. If `NULL` provided, no loggings
will be saved.
- `format`: Same as that in [printf](https://www.cplusplus.com/reference/cstdio/printf/).
- `...`: Same as that in [printf](https://www.cplusplus.com/reference/cstdio/printf/).

#### Examples
```c
int a = 0;

x_log('P', NULL "hello");
x_log('D', NULL "%s", "world");
x_log('I', NULL "%d", 10);
x_log('W', NULL "%d", ++a);
x_log('E', pf, "%d", ++a);  // e.g. FILE* pf = fopen("log.txt", "w")
x_log('F', pf, "%s", "fatal");
```

The formats of each level are almost identical. What differentiate them are the
level prefixes and the colors.

<table class="center" style="text-align:center">
<thead>
  <tr>
    <th>Level</th>
    <th>Color</th>
    <th>Debug Build Format</th>
    <th>Release Build Format</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td>P</td>
    <td><div><span style="background-color:white;">&emsp;</span></div></td>
    <td rowspan="6">[level timestamp | file | function | line] message</td>
    <td rowspan="6">[level timestamp] message</td>
  </tr>
  <tr>
    <td>F</td>
    <td><div><span style="background-color:purple;">&emsp;</span></div></td>
  </tr>
  <tr>
    <td>E</td>
    <td><div><span style="background-color:red;">&emsp;</span></div></td>
  </tr>
  <tr>
    <td>W</td>
    <td><div><span style="background-color:orange;">&emsp;</span></div></td>
  </tr>
  <tr>
    <td>I</td>
    <td><div><span style="background-color:green;">&emsp;</span></div></td>
  </tr>
  <tr>
    <td>D</td>
    <td><div><span style="background-color:cyan;">&emsp;</span></div></td>
  </tr>
</tbody>
</table>


<!----------------------------------------------------------------- LICENSE -->
## License
Distributed under the [Mulan PSL v2](http://license.coscl.org.cn/MulanPSL2/)
license. See [LICENSE](LICENSE) for more information.
