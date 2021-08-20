# klib

[![Open in Visual Studio Code](https://open.vscode.dev/badges/open-in-vscode.svg)](https://open.vscode.dev/KaiserLancelot/klib)
[![Build](https://github.com/KaiserLancelot/klib/actions/workflows/build.yml/badge.svg)](https://github.com/KaiserLancelot/klib/actions/workflows/build.yml)
[![Coverage Status](https://coveralls.io/repos/github/KaiserLancelot/klib/badge.svg?branch=main)](https://coveralls.io/github/KaiserLancelot/klib?branch=main)
[![Documentation](https://img.shields.io/badge/docs-doxygen-blue)](https://kaiserlancelot.github.io/klib)
[![GitHub License](https://img.shields.io/github/license/KaiserLancelot/klib)](https://github.com/KaiserLancelot/klib/blob/main/LICENSE)
[![996.icu](https://img.shields.io/badge/link-996.icu-red.svg)](https://996.icu)
[![GitHub Releases](https://img.shields.io/github/release/KaiserLancelot/klib)](https://github.com/KaiserLancelot/klib/releases/latest)
[![GitHub Downloads](https://img.shields.io/github/downloads/KaiserLancelot/klib/total)](https://github.com/KaiserLancelot/klib/releases)
[![Bugs](https://img.shields.io/github/issues/KaiserLancelot/klib/bug)](https://github.com/KaiserLancelot/klib/issues?q=is%3Aopen+is%3Aissue+label%3Abug)

---

C++ Library

## Environment

- Ubuntu 20.04
- GCC 11 or Clang 12

## Dependency

- [zlib](https://github.com/madler/zlib)
- [libarchive](https://github.com/libarchive/libarchive)
- [openssl](https://github.com/openssl/openssl)
- [nghttp2](https://github.com/nghttp2/nghttp2)
- [curl](https://github.com/curl/curl)
- [tidy-html5](https://github.com/htacg/tidy-html5)
- [fmt](https://github.com/fmtlib/fmt)
- [boost](https://www.boost.org/)

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

---

Thanks to [JetBrains](https://www.jetbrains.com/) for donating product licenses to help develop this project <a href="https://www.jetbrains.com/"><img src="logo/jetbrains.svg" width="94" align="center" /></a>
