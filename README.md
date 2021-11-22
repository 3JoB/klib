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

- Debian 11
- GCC 11 or Clang 13

## Dependency

- [tidy-html5](https://github.com/htacg/tidy-html5)
- [zlib](https://github.com/madler/zlib)
- [libarchive](https://github.com/libarchive/libarchive)
- [openssl](https://github.com/openssl/openssl)
- [nghttp2](https://github.com/nghttp2/nghttp2)
- [curl](https://github.com/curl/curl)
- [zstd](https://github.com/facebook/zstd)
- [sqlcipher](https://github.com/sqlcipher/sqlcipher)
- [argon2](https://github.com/P-H-C/phc-winner-argon2)
- [Boost](https://www.boost.org/)
- [fmt](https://github.com/fmtlib/fmt)
- [scope_guard](https://github.com/Neargye/scope_guard)

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
