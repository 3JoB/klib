#!/bin/bash

set -e

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  curl -L https://github.com/KaiserLancelot/kpkg/releases/download/v0.3.0/kpkg-v0.3.0-ubuntu-20.04.deb \
    -o kpkg.deb
  sudo dpkg -i kpkg.deb

  sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
  sudo apt update
  sudo apt install -y gcc-11 g++-11

  sudo bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"
  sudo apt install -y clang-tidy-12 valgrind

  sudo apt install -y make cmake re2c autoconf automake libtool m4 tcl

  sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 400
  sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 400
  sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-12 400
  sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-12 400
  sudo update-alternatives --install /usr/bin/lld lld /usr/bin/lld-12 400
  sudo update-alternatives --install /usr/bin/ld.lld ld.lld /usr/bin/ld.lld-12 400
  sudo update-alternatives --install /usr/bin/llvm-ar llvm-ar /usr/bin/llvm-ar-12 400
  sudo update-alternatives --install /usr/bin/llvm-nm llvm-nm /usr/bin/llvm-nm-12 400
  sudo update-alternatives --install /usr/bin/llvm-ranlib llvm-ranlib /usr/bin/llvm-ranlib-12 400
  sudo update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-12 400
  sudo update-alternatives --install /usr/bin/llvm-profdata llvm-profdata /usr/bin/llvm-profdata-12 400
  sudo update-alternatives --install /usr/bin/llvm-cov llvm-cov /usr/bin/llvm-cov-12 400
  sudo update-alternatives --install /usr/bin/llvm-symbolizer llvm-symbolizer /usr/bin/llvm-symbolizer-12 400

  if [ ! -d "dependencies" ]; then
    echo "mkdir dependencies"
    mkdir dependencies
  fi
  cd dependencies

  kpkg install lcov doxygen catch2 fmt libarchive curl openssl tidy-html5 -i
  sudo ldconfig
else
  echo "The system does not support: $OSTYPE"
  exit 1
fi
