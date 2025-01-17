# klib

[![Build](https://github.com/KaiserLancelot/klib/actions/workflows/build.yml/badge.svg)](https://github.com/KaiserLancelot/klib/actions/workflows/build.yml)
[![Platform](https://img.shields.io/badge/Platform-Debian%2011-brightgreen)](https://www.debian.org/)
[![Coverage Status](https://codecov.io/gh/KaiserLancelot/klib/branch/main/graph/badge.svg?token=Adu3iF5b6t)](https://codecov.io/gh/KaiserLancelot/klib)
[![GitHub Releases](https://img.shields.io/github/release/KaiserLancelot/klib)](https://github.com/KaiserLancelot/klib/releases/latest)
[![Documentation](https://img.shields.io/badge/docs-doxygen-blue)](https://kaiserlancelot.github.io/klib)
[![GitHub Downloads](https://img.shields.io/github/downloads/KaiserLancelot/klib/total)](https://github.com/KaiserLancelot/klib/releases)
[![GitHub License](https://img.shields.io/github/license/KaiserLancelot/klib)](https://github.com/KaiserLancelot/klib/blob/main/LICENSE)
[![996.icu](https://img.shields.io/badge/link-996.icu-red.svg)](https://996.icu)

---

A C++ library, which wraps several important C libraries and provides some convenient functions

## Third party

- zlib-ng ([zlib License](https://github.com/zlib-ng/zlib-ng/blob/develop/LICENSE.md))
- libarchive ([License](https://github.com/libarchive/libarchive/blob/master/COPYING))
- tidy-html5 ([License](https://github.com/htacg/tidy-html5/blob/next/README/LICENSE.md))
- curl ([License](https://github.com/curl/curl/blob/master/COPYING))
- brotli ([MIT License](https://github.com/google/brotli/blob/master/LICENSE))
- BoringSSL ([License](https://boringssl.googlesource.com/boringssl/+/refs/heads/master-with-bazel/LICENSE))
- sqlcipher ([License](https://github.com/sqlcipher/sqlcipher/blob/master/LICENSE))
- Argon2 ([License](https://github.com/P-H-C/phc-winner-argon2/blob/master/LICENSE))
- zstd ([BSD License](https://github.com/facebook/zstd/blob/dev/LICENSE))
- nghttp2 ([MIT License](https://github.com/nghttp2/nghttp2/blob/master/COPYING))
- simdutf ([MIT License](https://github.com/simdutf/simdutf/blob/master/LICENSE-MIT))
- xxHash ([BSD License](https://github.com/Cyan4973/xxHash/blob/dev/LICENSE))
- cmark ([License](https://github.com/commonmark/cmark/blob/master/COPYING))
- woff2 ([MIT License](https://github.com/google/woff2/blob/master/LICENSE))
- libvips ([GNU Lesser General Public License v2.1](https://github.com/libvips/libvips/blob/master/COPYING))
- glib ([GNU Lesser General Public License v2.1](https://gitlab.gnome.org/GNOME/glib/-/blob/main/COPYING))
- libexpat ([License](https://github.com/libexpat/libexpat/blob/master/expat/COPYING))
- libffi ([License](https://github.com/libffi/libffi/blob/master/LICENSE))
- util-linux ([GNU General Public License v2.0](https://github.com/util-linux/util-linux/blob/master/COPYING))
- pcre ([License](https://github.com/luvit/pcre/blob/master/LICENCE))
- libspng ([BSD 2-Clause "Simplified" License](https://github.com/randy408/libspng/blob/master/LICENSE))
- mozjpeg ([License](https://github.com/mozilla/mozjpeg/blob/master/LICENSE.md))
- libwebp ([License](https://chromium.googlesource.com/webm/libwebp/+/refs/heads/main/COPYING))
- XZ Utils ([License](https://git.tukaani.org/?p=xz.git;a=blob;f=COPYING;h=20e60d5b2427334e1fec6701e7c5ad0da0bc8a5d;hb=HEAD))
- libqrencode ([GNU Lesser General Public License v2.1](https://github.com/fukuchi/libqrencode/blob/master/COPYING))
- file ([License](https://github.com/file/file/blob/master/COPYING))
- Scope Guard ([MIT License](https://github.com/Neargye/scope_guard/blob/master/LICENSE))
- fmt ([License](https://github.com/fmtlib/fmt/blob/master/LICENSE.rst))
- spdlog ([MIT License](https://github.com/gabime/spdlog/blob/v1.x/LICENSE))
- parallel-hashmap ([Apache License 2.0](https://github.com/greg7mdp/parallel-hashmap/blob/master/LICENSE))
- Boost ([Boost License](https://www.boost.org/users/license.html))
- GSL ([MIT License](https://github.com/Microsoft/GSL/blob/master/LICENSE))
- utfcpp ([Boost Software License 1.0](https://github.com/nemtrif/utfcpp/blob/master/LICENSE))
- fastbase64 ([License](https://github.com/lemire/fastbase64/blob/master/LICENSE))
- sfntly ([Apache License 2.0](https://github.com/rillig/sfntly/blob/master/cpp/COPYING.txt))

## Build environment

- [kenv](https://github.com/KaiserLancelot/kenv)

## Usage

Install dependencies first using [kpkg](https://github.com/KaiserLancelot/kpkg)

```bash
kpkg install fmt spdlog parallel-hashmap
```

Then use with CMake

```cmake
find_package(klib REQUIRED)
target_link_libraries(<your-target> PRIVATE klib::klib)
```

---

Thanks to [JetBrains](https://www.jetbrains.com/) for donating product licenses to help develop this project <a href="https://www.jetbrains.com/"><img src="logo/jetbrains.svg" width="94" align="center" /></a>
