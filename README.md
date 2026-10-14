<pre align="center">
█▀▀███▀▀████▀▀▀█▀▀▀█
█▄  ▀  ▄████   █   █
███   ██████       █
█▀  ▄  ▀█▀▀█   █   █
█▄▄███▄▄█▄▄█▄▄▄█▄▄▄█
</pre>

<p align="center">
  Cross-platform C/C++ utilities (<code>x</code> stands for cross).
</p>

<details open=true>
  <summary>Table of Contents</summary>
  <ul>
    <li><a href="#features">Features</a></li>
    <li><a href="#getting-started">Getting Started</a></li>
    <li><a href="#module-selection">Module Selection</a></li>
    <li><a href="#ai-assistance">AI Assistance</a></li>
    <li><a href="#license">License</a></li>
  </ul>
</details>

## Features

### Core

- **Configuration and portability:** Feature configuration, architecture,
  compiler, OS, and platform detection, symbol visibility, and source
  diagnostics.
- **Error handling:** Assertions, checked calls, and unified error domains for
  POSIX, Win32, HRESULT, NTSTATUS, Mach, sockets, CUDA, cuBLAS, and custom
  domains.

### Optional Modules

- **Logging:** Levelled, colored logging with configurable verbosity.
- **Checksums:** CRC-32, RFC 1071 Internet checksum, and XOR checksum.
- **Date and Time:** Timestamps, millisecond sleep, and CPU/CUDA timing with
  sample statistics.
- **File System:** File queries, stream opening, absolute paths, and file size.
- **Hardware:** Logical CPU count.
- **Mathematics:** Mathematical constants, binary size helpers, bit masks,
  GCD/LCM, and rounding to powers or multiples.
- **Memory:** Array length, allocation/deallocation, checked copying, and
  CPU/GPU memory queries.
- **String:** Null/empty checks and bounded string copying.
- **CUDA:** GPU device count and memory type queries.

API documentation is included directly in [c/x.h](c/x.h) and [cpp/x.h](cpp/x.h).

## Getting Started

Copy the header matching your language into your project as `x.h`:

- [c/x.h](c/x.h): C99 or newer.
- [cpp/x.h](cpp/x.h): C++17 or newer, with classes, templates, and RAII.

### C

```c
#define X_ERROR_DOMAIN_IMPLEMENTATION
#include "x.h"

int main(void)
{
  x_log('i', NULL, "CRC-32: %08x", (unsigned int)x_checksum_crc32("123456789", 9, NULL));
  return 0;
}
```

When using the C error-domain registry, define `X_ERROR_DOMAIN_IMPLEMENTATION`
before including `x.h` in exactly one translation unit.

### C++

```cpp
#include "x.h"

int main()
{
  x_log('i', nullptr, "CRC-32: %08x", static_cast<unsigned int>(x_checksum_crc32("123456789", 9)));
  return 0;
}
```

C++ uses inline variables for the registry and needs no implementation macro.

### Configuration

These feature flags default to `0` (disabled). Before including `x.h`, define a
flag to a nonzero integer constant expression valid in `#if` to enable it:

- `X_ENABLE_CUDA_DRIVER`: CUDA Driver API support.
- `X_ENABLE_CUDA_RUNTIME`: CUDA Runtime API support.
- `X_ENABLE_CUBLAS`: The cuBLAS error domain.
- `X_ENABLE_SOCKET`: The socket error domain; no socket operations.

Enabled CUDA/cuBLAS support requires the corresponding SDK headers and
libraries. On Windows, the socket error domain requires the Winsock library.

Keep feature settings and `X_ERROR_DOMAIN_LIMIT` consistent across translation
units. Register custom error domains before concurrent access. On Linux,
include `x.h` before system headers so its feature-test macros take effect.

## Module Selection

Use Python 3 to extract a standalone header with only the modules you need:

```sh
python3 tool/generate.py --lang c --module minimal --output build/minimal/x.h
python3 tool/generate.py --lang cpp --module math string --output build/custom/x.h
```

Core is always included: configuration and portability helpers, shared
utilities, and error handling. `minimal` (or `core`) selects only Core;
`all` is the default and selects the complete header.

Optional modules are `logging`, `checksum`, `date_time`, `filesystem`,
`hardware`, `math`, `memory`, `string`, and `cuda`. Each depends only on Core.
Module selection does not enable optional backends.

## AI Assistance

The original implementation of x.h was written by me. I now use AI tools to
assist with identifying and fixing issues, refactoring, and writing and running
tests. I review accepted code changes line by line and remain responsible for
design decisions and project maintenance.

## License

Distributed under the [MulanPSL-2.0](http://license.coscl.org.cn/MulanPSL2) license. See [LICENSE](LICENSE) for details.

<details>
  <summary>Why MulanPSL-2.0?</summary>

The Mulan Permissive Software License v2 (MulanPSL-2.0) may be less familiar than more widely used licenses. To provide clarity and context, the following table (cited from [*Choose a License*](https://choosealicense.com/appendix/)) compares key aspects of MulanPSL v2 with popular licenses including *Apache-2.0*, *BSD-3-Clause*, and *MIT*.

| License          | Commercial Use | Distribution | Modification | Patent Use | Private Use | Disclose Source | License and Copyright Notice | Network Use is Distribution | Same License | State Changes | Liability | Trademark Use | Warranty |
|:----------------:|:--------------:|:------------:|:------------:|:----------:|:-----------:|:---------------:|:----------------------------:|:---------------------------:|:------------:|:-------------:|:---------:|:-------------:|:--------:|
| Apache-2.0       | 🟢             | 🟢           | 🟢           | 🟢         | 🟢          |                 | 🔵                           |                             |              | 🔵            | 🔴        | 🔴            | 🔴       |
| BSD-3-Clause     | 🟢             | 🟢           | 🟢           |            | 🟢          |                 | 🔵                           |                             |              |               | 🔴        |               | 🔴       |
| MIT              | 🟢             | 🟢           | 🟢           |            | 🟢          |                 | 🔵                           |                             |              |               | 🔴        |               | 🔴       |
| MulanPSL-2.0     | 🟢             | 🟢           | 🟢           | 🟢         | 🟢          |                 | 🔵                           |                             |              |               | 🔴        | 🔴            | 🔴       |

The drafter of the MulanPSL-2.0 license addressed similar concerns in this
[comment](https://github.com/originjs/vite-plugin-federation/issues/464#issuecomment-1774859600):

> Thank you for raising this issue. Please allow me to explain. (I'm the one responsible for drafting MulanPSL-2.0 and getting it approved by OSI.)
>
> Actually at the beginning we just say in the license, english and chinese version have the same legal effect (because we carefully translated the two versions word by word, sentence by sentence). However, the OSI community suggested that IN CASE, in case there is a conflict between the two languages, we should indicate which language prevails.
>
> However, I must say, there is a tiny chance (close to zero) that this circumstance will happen. On the one hand, many people (including technical experts and lawyers) did careful proofreading between english version and chinese version; on the other hand, MulanPSL-2.0 is such a loose license that really doesn't have constrains, what conflict will you expect? We worry about conflict because we worry about legal risk that may bring, but since the legal terms are so loose we hardly see a risk.
</details>
