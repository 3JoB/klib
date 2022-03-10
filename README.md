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

## Third party

- zlib-ng ([zlib License](https://github.com/zlib-ng/zlib-ng/blob/develop/LICENSE.md))
- libarchive ([License](https://github.com/libarchive/libarchive/blob/master/COPYING))
- tidy-html5 ([License](https://github.com/htacg/tidy-html5/blob/next/README/LICENSE.md))
- curl ([License](https://github.com/curl/curl/blob/master/COPYING))
- BoringSSL ([License](https://boringssl.googlesource.com/boringssl/+/refs/heads/master-with-bazel/LICENSE))
- sqlcipher ([License](https://github.com/sqlcipher/sqlcipher/blob/master/LICENSE))
- Argon2 ([License](https://github.com/P-H-C/phc-winner-argon2/blob/master/LICENSE))
- zstd ([BSD License](https://github.com/facebook/zstd/blob/dev/LICENSE))
- simdutf ([MIT License](https://github.com/simdutf/simdutf/blob/master/LICENSE-MIT))
- xxHash ([BSD License](https://github.com/Cyan4973/xxHash/blob/dev/LICENSE))
- cmark ([License](https://github.com/commonmark/cmark/blob/master/COPYING))
- woff2 ([MIT License](https://github.com/google/woff2/blob/master/LICENSE))
- libwebp ([License](https://chromium.googlesource.com/webm/libwebp/+/refs/heads/main/COPYING))
- Scope Guard ([MIT License](https://github.com/Neargye/scope_guard/blob/master/LICENSE))
- fmt ([License](https://github.com/fmtlib/fmt/blob/master/LICENSE.rst))
- spdlog ([MIT License](https://github.com/gabime/spdlog/blob/v1.x/LICENSE))
- fastbase64 ([License](https://github.com/lemire/fastbase64/blob/master/LICENSE))
- http-parser ([MIT License](https://github.com/nodejs/http-parser/blob/main/LICENSE-MIT))
- sfntly ([Apache License 2.0](https://github.com/rillig/sfntly/blob/master/cpp/COPYING.txt))
- utf_utils ([License](https://github.com/BobSteagall/utf_utils/blob/master/LICENSE.txt))

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
