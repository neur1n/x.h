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
  Cross platform C/C++ utilities.
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
See:
- C implementation: [README.md](c/README.md) (WIP)
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
