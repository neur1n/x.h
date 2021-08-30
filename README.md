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
INCLUDE_DIRECTORIES("path/to/neu.h")
```

### Whole Repository
If the whole repository is downloaded/included into your project, add the
following snippet to your project's main `CMakeLists.txt`:
```cmake
SET(NeuH_DIR "path/to/neu.h")              # Not the neu.h file, but the repository
FIND_PACKAGE(NeuH REQUIRED)                # Notice that the package name is not "neu.h"
INCLUDE_DIRECTORIES(${NeuH_INCLUDE_DIRS})  # Add to inclusion
SET(NeuH_LOG_LEVEL 1)                      # If not specified, 5 will be used by default
```
Notice that when using the whole repository, this library can be found with
`FIND_PACKAGE()` and additionally defined two variables to be used:
- `NeuH_INCLUDE_DIRS`: the include directories of neu.h
- `NeuH_LOG_LEVEL`: the log verbosity level of neu.h, of which the available
values are:
  - \>= 5: enables `NLogP`, `NLogF`, `NLogE`, `NLogW`, `NLogI`, `NLogD` (all log printings) 
  - 4: enables `NLogP`, `NLogF`, `NLogE`, `NLogW`, `NLogI`
  - 3: enables `NLogP`, `NLogF`, `NLogE`, `NLogW`
  - 2: enables `NLogP`, `NLogF`, `NLogE`
  - 1: enables `NLogP`, `NLogF`
  - 0: enables `NLogP`
  - < 0: disables all log printings

  where `P`, `F`, `E`, `W`, `I`, `D` are short for `PUBLIC`, `FATAL`, `ERROR`,
  `WARNING`, `INFO`, `DEBUG` respectively.

<!----------------------------------------------------------- DOCUMENTATION -->
## Documentation
TODO

<!----------------------------------------------------------------- LICENSE -->
## License
Distributed under the MIT license. See [LICENSE](LICENSE) for more information.
