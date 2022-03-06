# klib

[![Build](https://github.com/KaiserLancelot/klib/actions/workflows/build.yml/badge.svg)](https://github.com/KaiserLancelot/klib/actions/workflows/build.yml)
[![Platform](https://img.shields.io/badge/Platform-Debian%2011-brightgreen)](https://www.debian.org/)
[![Coverage Status](https://coveralls.io/repos/github/KaiserLancelot/klib/badge.svg?branch=main)](https://coveralls.io/github/KaiserLancelot/klib?branch=main)
[![GitHub Releases](https://img.shields.io/github/release/KaiserLancelot/klib)](https://github.com/KaiserLancelot/klib/releases/latest)
[![Documentation](https://img.shields.io/badge/docs-doxygen-blue)](https://kaiserlancelot.github.io/klib)
[![GitHub Downloads](https://img.shields.io/github/downloads/KaiserLancelot/klib/total)](https://github.com/KaiserLancelot/klib/releases)
[![GitHub License](https://img.shields.io/github/license/KaiserLancelot/klib)](https://github.com/KaiserLancelot/klib/blob/main/LICENSE)
[![996.icu](https://img.shields.io/badge/link-996.icu-red.svg)](https://996.icu)

---

A C++ library, which wraps several important C libraries and provides some convenient functions

## Environment

- [kenv](https://github.com/KaiserLancelot/kenv)

## Build

```bash
cmake -S . -B build
cmake --build build --config Release -j"$(nproc)"
```

## Install

```bash
sudo cmake --build build --config Release --target install
```

## Uninstall

```bash
sudo cmake --build build --config Release --target uninstall
```

## Usage with CMake

```cmake
add_subdirectory(klib)
```

or

```cmake
find_package(klib)
target_link_libraries(<your-target> PRIVATE klib::klib)
```

## Roadmap

- Use get headers
  - https://github.com/curl/curl/wiki/get-headers-v2
- HTTP/3
  - https://github.com/curl/curl/wiki/HTTP3

---

Thanks to [JetBrains](https://www.jetbrains.com/) for donating product licenses to help develop this project <a href="https://www.jetbrains.com/"><img src="logo/jetbrains.svg" width="94" align="center" /></a>
