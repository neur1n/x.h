<!------------------------------------------------------------------ HEADER -->
<pre align="center">
  ▄▀▀▀▀▄▀▀▄ █▀▀▀▀▀▀▀█ █▀▀▀█▀▀▀█    █▀▀▀█▀▀▀█
  █    █  █ █   ▄▄▄▄█ █   █   █    █   █   █
  █  █ █  █ █       █ █   █   █    █       █
  █  █    █ █   ▀▀▀▀█ █   ▀   █    █   █   █
  ▀▄▄▀▄▄▄▄▀ █▄▄▄▄▄▄▄█ ▀▄▄▄▄▄▄▄▀ ██ █▄▄▄█▄▄▄█
</pre>


<p align="center">
  <img src="https://img.shields.io/badge/C11-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white" alt="C11">
  <img src="https://img.shields.io/badge/C++11-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++11">

  <a href="./LICENSE">
    <img src="https://img.shields.io/badge/license-mulan_psl_v2-green.svg?style=for-the-badge" alt="license-mulan-psl-v2">
  </a>
</p>

<p align="center">
  Handy C/C++ utility collections.
</p>

<h3 align="center">
  ⚡ neu.h was heavily refactored after <a href=https://github.com/Neur1n/neu.h/releases/tag/v0.2.0>v0.2.0</a>.
</h3>


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
        <li><a href="#n_export-n_import">N_EXPORT, N_IMPORT</a></li>
        <li><a href="#n_key">N_KEY</a></li>
        <li><a href="#n_log">NLogX</a></li>
        <li><a href="#n_array_size">n_array_size</a></li>
        <li><a href="#n_assert">n_assert</a></li>
        <li><a href="#n_bit">n_bit</a></li>
        <li><a href="#n_clamp">n_clamp</a></li>
        <li><a href="#n_pi">n_pi</a></li>
        <li><a href="#n_succ">n_succ</a></li>
        <li><a href="#n_fail">n_fail</a></li>
        <li><a href="#n_duration">n_duration</a></li>
        <li><a href="#n_full_path">n_full_path</a></li>
        <li><a href="#n_log_to">n_log_to</a></li>
        <li><a href="#n_path_exists">n_path_exists</a></li>
        <li><a href="#n_pressed_key">n_pressed_key</a></li>
        <li><a href="#n_sleep">n_sleep</a></li>
        <li><a href="#n_string_empty">n_string_empty</a></li>
        <li><a href="#n_timestamp">n_timestamp</a></li>
      </ul>
    </li>
    <li><a href="#license">LICENSE</a></li>
  </ol>
</details>


<!--------------------------------------------------------- GETTING STARTED -->
## Getting Started
### Prerequisites
C/C++ compiler that supports C11/C++11.


### Installation
Put `neu.h` into your project.


<!----------------------------------------------------------- DOCUMENTATION -->
## Documentation
### N_EXPORT, N_IMPORT
```cpp
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
```

See [Microsoft Docs](https://docs.microsoft.com/en-us/cpp/cpp/dllexport-dllimport?view=msvc-170)
and [GCC Wiki](https://gcc.gnu.org/wiki/Visibility) for more details.


### N_KEY
```cpp
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
```

Several virtual key macros, most of which are defined as their corresponding
[ASCII code](https://www.asciitable.com). However, the arrow keys are handled
differently among OSs (please tell me if this statement is not accurate), they
are defined as negative integers to work around. Please checkout
<a href="#n_pressed_key">n_pressed_key</a> for their usage.


### n_log
```cpp
void n_log(const char* level, const char* file, const char* format, ...)
```

#### Parameters
- `level`: Logging level specification, available values are `P` (Public),
`F` (Fatal), `E` (Error), `W` (Warning), `I` (Info), `D` (Debug).
- `file`: File for saving the loggings. If `NULL` provided, no loggings will be
saved.
- `format`: Same as that in [printf](https://www.cplusplus.com/reference/cstdio/printf/).
- `...`: Same as that in [printf](https://www.cplusplus.com/reference/cstdio/printf/).

#### Examples
```cpp
int a = 0;

n_log("P", nullptr, "hello");
n_log("D", nullptr, "%s", "world");
n_log("I", nullptr, "%d", 10);
n_log("W", nullptr, "%d", ++a);
n_log("E", "./log.txt", "%d", ++a);
n_log("F", "./log.txt", "%s", "fatal");
```

The formats of each level are almost identical. What differentiate them are the
level prefixes and the colors. Additionally, fatal level will call
`exit(EXIT_FAILURE)` to make the program exits.

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
    <td><img src="https://via.placeholder.com/20x10/FFFFFF/FFFFFF"></a></td>
    <td rowspan="6">[level timestamp | file | function | line] message</td>
    <td rowspan="6">[level timestamp] message</td>
  </tr>
  <tr>
    <td>F</td>
    <td><img src="https://via.placeholder.com/20x10/FF00FF/FF00FF"></a></td>
  </tr>
  <tr>
    <td>E</td>
    <td><img src="https://via.placeholder.com/20x10/FF0000/FF0000"></a></td>
  </tr>
  <tr>
    <td>W</td>
    <td><img src="https://via.placeholder.com/20x10/FFFF00/FFFF00"></a></td>
  </tr>
  <tr>
    <td>I</td>
    <td><img src="https://via.placeholder.com/20x10/00FF00/00FF00"></a></td>
  </tr>
  <tr>
    <td>D</td>
    <td><img src="https://via.placeholder.com/20x10/0000FF/0000FF"></a></td>
  </tr>
</tbody>
</table>


### n_array_size
```cpp
T n_array_size(array)  // In this document, T indicats a return type from a macro.
```

#### Parameters
- `array`: The array of which the size to be queried.

#### Returns
- The length (count of elements) of the input array.

#### Examples
```cpp
int a[5] = {1, 2, 3, 4, 5};

for (size_t i = 0; i < n_array_size(a); ++i)
{
  printf("%d", a[i]);
}
```

Get length of an array. This macro does not work with those arrays declared
with `malloc` or `new`, nor throw any exceptions. For C++, some suggest using
the following template function:

```cpp
template<class T, size_t N>
size_t n_array_size(const T (&array)[N])
{
  return N;
}
```


### n_assert
```cpp
n_assert(expr)
```

#### Parameters
- `expr`: The assertion expression.

#### Examples
```cpp
n_assert(1 == 1);
n_assert(1 != 1);
```

An macro enables the assertion functionality for both debug and release builds.
For debug build, it is the same as the assertion macro in assert.h/cassert
header.


### n_bit
```cpp
T n_bit(bit)
```

#### Parameters
- `bit`: Specification for which bit of the integer should be set to 1.

#### Examples
```cpp
enum
{
  FLAG_NONE   = 0,
  FLAG_READ   = n_bit(1),
  FLAG_WRITE  = n_bit(2),
};

int read_and_write = FLAG_READ | FLAG_WRITE;
```

A macro used to generate an integer with only the n-th bit set to 1. This is
useful when one needs enumerations like `0b0001`, `0b0010`, `0b0100` to perform
the `&`, `|`, `~` operations.

#### Returns
- The generated integer, may be `int`, `long`, etc.


### n_clamp
```cpp
T n_clamp(x, lower, upper)
```

#### Parameters
- `x`: Variable or value to be clamped.
- `lower`: Lower bound.
- `upper`: Upper bound.

#### Returns
- The clamped value.


### n_max
```cpp
T n_max(a, b)
```

#### Parameters
- `a`: Some variable or value.
- `b`: Another variable or value.

#### Returns
- The bigger value.


### n_min
```cpp
T n_min(a, b)
```

#### Parameters
- `a`: Some variable or value.
- `b`: Another variable or value.

#### Returns
- The smaller value.


### n_pi
```cpp
T n_pi(T)
```

#### Parameters
- `T`: Target type, e.g. `float`, `double`, `long`.

#### Returns
- Pi of specified type.

#### Examples
```cpp
float a = n_pi(float);
double b = n_pi(double);
```

A macro for retrieving the constant [Pi](https://en.wikipedia.org/wiki/Pi). The
input argument is used to specify the target data type. In C++, one may use the
following template:

```cpp
template<class T>
static constexpr T n_pi = (T)3.141592653589793238462643383279502884197169399375;
// Usage: n_pi<float>, n_pi<double>, etc.
```


### n_succ
```cpp
bool n_succ(const errno_t err)
```

#### Parameters
- code: The error code returned by some function.

#### Returns
- `true` if the error code indicate no failure.

#### Examples
```cpp
FILE* stream = NULL;
errno_t err = 0;
err = fopen_s(&stream, "some/file", "a");
if (n_succ(err))
{
  // do something...
}
```


### n_fail
```cpp
bool n_fail(const errno_t err)
```

Check if the error code indicates any failure.

#### Parameters
- code: The error code returned by some function.

#### Returns
- `true` if the code indicates a failure.


### n_duration
```cpp
double n_duration(
    const struct timespec start, const struct timespec end, const char* unit)
```

#### Parameters
- `start`: The start time point.
- `end`: The end time point.
- `unit`: The unit of the outcome.

#### Returns
- The duration of the two time points in specified unit.


### n_full_path
```cpp
const char* n_full_path(const char* src, char* dst)
```

#### Parameters
- `src`: The (maybe relative) path to a file.
- `dst`: The full/absolute path to the input file. It should be allocated with
sufficient memory before passed into this function, otherwise `NULL` is returned.

#### Returns
- The same value contained in `dst`, or `NULL` when the operation failed.

#### Examples
```cpp
const char* file = "./foo/bar.txt";
char buf[MAX_PATH] = {0};
n_full_path(file, buf);
```


### n_log_to
```cpp
errno_t n_log_to(const char* file, const char* format, ...)
```

#### Parameters
- `file`: The path to a file.
- `format`: Same as that in [printf](https://www.cplusplus.com/reference/cstdio/printf/).
- `...`: Same as that in [printf](https://www.cplusplus.com/reference/cstdio/printf/).

#### Returns
- The resulting error code of the operation.

#### Examples
```cpp
char ts[26] = {0};
errno_t err = n_log_to("./foo/bar.txt", "%s: %s", n_timestamp(ts, 26), "hello world");
if (n_succ(err))
{
  // do something...
}
```

Additionally, see definition of <a href="#n_log">n_log</a> for another example.


### n_path_exists
```cpp
bool n_path_exists(const char* path)
```

#### Parameters
- `path`: The path to a querying file/directory.

#### Returns
- `true` if the file/directory exists.


### n_pressed_key
```cpp
int n_pressed_key()
```

#### Returns
- The pressed key.

#### Examples
This function only works with console programs. One may use it to break a
infinity loop manually:

```cpp
int main(int argc, char** argv)
{
  while (true)
  {
    if (n_pressed_key() == N_KEY_ESC)
    {
      break;  // When the 'Esc' key is pressed, the loop breaks.
    }

    n_sleep(10);
  }

  return 0;
}
```


### n_sleep
```cpp
void n_sleep(const unsigned long long ms)
```

#### Parameters
- `ms`: The length of the sleep duration in milliseconds.


### n_string_empty
```cpp
bool n_string_empty(const char* string)
```

#### Parameters
- `string`: The string.

#### Returns
- `true` if the string is NULL or '\0' is the only thing it contains.


### n_timestamp
```cpp
const char* n_timestamp(char* buffer, const size_t size)
```

#### Parameters
- `buffer`: Buffer to store timestamp string.
- `size`: Size of the buffer.

#### Returns
- Same content of the input buffer. A empty string ("") will be returned if the
operation failed.

#### Examples
See <a href="#n_log_to">n_log_to</a>.


<!----------------------------------------------------------------- LICENSE -->
## License
Distributed under the [Mulan PSL v2](http://license.coscl.org.cn/MulanPSL2/)
license. See [LICENSE](LICENSE) for more information.
