<!------------------------------------------------------------------ HEADER -->
<pre align="center">
  ██╗   ██╗ ████████╗ ██╗   ██╗     ██╗   ██╗
  ████╗ ██║ ██╔═════╝ ██║   ██║     ██║   ██║
  ██╔██╗██║ ██████╗   ██║   ██║     ████████║
  ██║╚████║ ██╔═══╝   ██║   ██║     ██╔═══██║
  ██║ ╚═██║ ████████╗ ╚██████╔╝ ██╗ ██║   ██║
  ╚═╝   ╚═╝ ╚═══════╝  ╚═════╝  ╚═╝ ╚═╝   ╚═╝
</pre>

<p align="center">
  <img src="https://img.shields.io/badge/C++11-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++11">

  <a href="./LICENSE">
    <img src="https://img.shields.io/badge/license-mit-green.svg?style=for-the-badge" alt="license-mit">
  </a>
</p>

<p align="center">
  Handy C++ utility collections.
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
    <li><a href="#usage">Usage</a></li>
    <li><a href="#license">License</a></li>
  </ol>
</details>

<!--------------------------------------------------------- GETTING STARTED -->
## Getting Started
### Prerequisites
- C++ compiler: >= C++11
- CMake: >= 3.5

### Installation
#### Manually
1. Clone the repository
```bash
git clone https://github.com/Neur1n/neu.h.git
```

2. Add the following contents to your project's `CMakeLists.txt`
```cmake
INCLUDE(${PATH_TO_NEU_H}/cmake/neu.h.cmake)  # this provides ${NEU_H_DIR}
INCLUDE_DIRECTORIES(${NEU_H_DIR}/include)
```

#### Via CMake
CMake's [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html)
module can be used to automate the inclusion of neu.h by adding the following
contents to your project's `CMakeLists.txt`:
```cmake
INCLUDE(FetchContent)

FetchContent_Declare(neu_h
  GIT_REPOSITORY https://github.com/Neur1n/neu.h.git
  GIT_TAG origin/master
  GIT_PROGRESS ON

  FETCHCONTENT_BASE_DIR ${PROJECT_SOURCE_DIR}/build/neu.h
  FETCHCONTENT_SOURCE_DIR_NEU_H ${PROJECT_SOURCE_DIR}/thirdparty/neu.h
  FETCHCONTENT_QUIET OFF
)
FetchContent_MakeAvailable(neu_h)

INCLUDE(${neu_h_SOURCE_DIR}/cmake/neu.h.cmake)
```

<!------------------------------------------------------------------- USAGE -->
## Usage
Include the single header into a C++ file then it is good to go:
```cpp
#include "neu.h"

// rest of your code
```

<!----------------------------------------------------------------- LICENSE -->
## License
Distributed under the MIT license. See [LICENSE](LICENSE) for more information.
