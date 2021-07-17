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
    <li><a href="#documentation">Documentation</a></li>
    <li><a href="#license">License</a></li>
  </ol>
</details>

<!--------------------------------------------------------- GETTING STARTED -->
## Getting Started
### Prerequisites
- C++ compiler: >= C++11
- CMake: >= 3.5

### Installation
This library contains only one header, `neu.h`. So you may download this single
file and put it into your project, or the whole repository via Git:
```bash
git clone https://github.com/Neur1n/neu.h.git
```

<!------------------------------------------------------------------- USAGE -->
## Usage
### Single Header
If you want to use only the single `neu.h` file, add the directory containing
`neu.h` to `INCLUDE_DIRECTORIES()` in your project's main `CMakeLists.txt`:
```cmake
INCLUDE_DIRECTORIES(${PATH_TO_NEU_H_DIR})
```

### Whole Repository
If the whole repository is downloaded/included into your project, add the
following snippet to your project's main `CMakeLists.txt`:
```cmake
SET(NeuH_DIR "path/to/NeuH")
FIND_PACKAGE(NeuH REQUIRED)
INCLUDE_DIRECTORIES(${NeuH_INCLUDE_DIRS})
SET(NeuH_LOG_LEVEL 1)
```
Notice that when using the whole repository, this library can be found with
`FIND_PACKAGE()` and additionally defined two variables to be used:
- `NeuH_INCLUDE_DIRS`: the include directories of neu.h
- `NeuH_LOG_LEVEL`: the log verbosity level of neu.h, of which the available
values are:
  - 4: enables all log printings (`NOneLine`, `NErr`, `NWarn`, `NHint`,
  `NInfo`), recommended for **RELEASE** build
  - 3: enables `NOneLine`, `NErr`, `NWarn`, `NHint`
  - 2: enables `NOneLine`, `NErr`, `NWarn`
  - 1: enables `NOneLine`, `NErr`
  - 0: disable all log printings
  - -1: enables all log printings, recommended for **DEBUG** build

<!----------------------------------------------------------- DOCUMENTATION -->
## Documentation
TODO

<!----------------------------------------------------------------- LICENSE -->
## License
Distributed under the MIT license. See [LICENSE](LICENSE) for more information.
