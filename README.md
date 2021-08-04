# klib

[![Open in Visual Studio Code](https://open.vscode.dev/badges/open-in-vscode.svg)](https://open.vscode.dev/KaiserLancelot/klib)
[![Build](https://github.com/KaiserLancelot/klib/actions/workflows/build.yml/badge.svg)](https://github.com/KaiserLancelot/klib/actions/workflows/build.yml)
[![Coverage Status](https://coveralls.io/repos/github/KaiserLancelot/klib/badge.svg?branch=master)](https://coveralls.io/github/KaiserLancelot/klib?branch=main)
[![Documentation](https://img.shields.io/badge/docs-doxygen-blue)](https://kaiserlancelot.github.io/klib)
[![GitHub License](https://img.shields.io/github/license/KaiserLancelot/klib)](https://raw.githubusercontent.com/KaiserLancelot/klib/master/LICENSE)
[![996.icu](https://img.shields.io/badge/link-996.icu-red.svg)](https://996.icu)
[![GitHub Releases](https://img.shields.io/github/release/KaiserLancelot/klib)](https://github.com/KaiserLancelot/klib/releases/latest)
[![GitHub Downloads](https://img.shields.io/github/downloads/KaiserLancelot/klib/total)](https://github.com/KaiserLancelot/klib/releases)
[![Bugs](https://img.shields.io/github/issues/KaiserLancelot/klib/bug)](https://github.com/KaiserLancelot/klib/issues?q=is%3Aopen+is%3Aissue+label%3Abug)

---

C++ Library

## Environment:

- Linux(Ubuntu 20.04)
- gcc/clang(Request to support C++20)

## Libraries:

- fmt
- zlib
- libarchive
- openssl
- curl

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
