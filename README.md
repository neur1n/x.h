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
    <img src="https://img.shields.io/badge/license-mulan_psl_v2-green.svg?style=for-the-badge" alt="license-mulan-psl-v2">
  </a>
</p>

<p align="center">
  Cross-platform C/C++ utilities (<code>x</code> stands for cross).
</p>


<details open=true>
  <summary>Table of Contents</summary>
  <ul>
    <li><a href="#getting-started">Getting Started</a></li>
    <li><a href="#documentation">Documentation</a></li>
    <li><a href="#license">LICENSE</a></li>
  </ul>
</details>


## Getting Started
This project contains several versions of implementations:
- The C implementation in the [c](c) folder, designed to ensure compatibility with C99.
- The C++ implementation in the [cpp](cpp) folder, tailored to harness the latest C++ features (only if they are supported by compilers).

Use the C++ implementation if you can, use the C implementation when you have to.

## Documentation
Feature list:
- **Feature Configuration**: Macros for toggling features.
- **Architecture Detection**: Macros for detecting CPU architecture, e.g., x86, ARM.
- **Compiler Detection**: Macros for identifying the compiler, e.g., Clang, GCC, MSVC.
- **Operating System Detection**: Macros for identifying the operating system, e.g., Linux, macOS, Windows.
- **Platform Detection**: Macros for identifying the platform, e.g., Android, MinGW.
- **Symbol Visibility**: Macros for controlling symbol visibility in shared libraries.
- **Miscellaneous**: Miscellaneous utility macros.
- **Communication**: Utilities such as sockets.
- **Console IO**: Utilities such as console interaction (e.g., get a key press).
- **Date and Time**: Utilities such as current date and time, sleep for milliseconds.
- ✨**Error Handling**: Utilities such as unified error systems (including POSIX, Win32, CUDA and more) and handy error handling macros.
- **File System**: Utilities such as checking file existence, separating file paths.
- **Hardware**: Utilities such as CPU core count.
- **Mathematics**: Utilities such as binary size generators (e.g., KB, MB, GB), greatest common divisor (GCD), least common multiple (LCM).
- **Memory Management**: Utilities such as used and available memory, checking memory type (e.g., host/CPU or device/GPU).
- ✨**Standard IO**: Utilities such as levelled and colored logging.
- **String**: Utilities such as safe string copy.

Read more by searching the feature name in the documentation:
- C implementation: [doxygen-style documentation](c/x.h)
- C++ implementation: [doxygen-style documentation](cpp/x.h)


## License
Distributed under the [MulanPSL-2.0](http://license.coscl.org.cn/MulanPSL2)
license. See [LICENSE](LICENSE) for details.

The MuLan Public Software License v2 (MuLanPSL-2.0) may be less familiar than
more widely used licenses. To provide clarity and context, the following table
(cited from [*Choose a License*](https://choosealicense.com/appendix/))
compares key aspects of MuLanPSL v2 with those popular licenses including
*Apache-2.0*, *BSD-3-Clause*, and *MIT*.

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
