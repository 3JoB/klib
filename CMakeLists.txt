# ---------------------------------------------------------------------------------------
# CMake version
# ---------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.22)
message(STATUS "CMake version: ${CMAKE_VERSION}")

# ---------------------------------------------------------------------------------------
# Start klib project
# ---------------------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(Version)
klib_extract_version()

project(
  klib
  LANGUAGES C CXX
  VERSION ${KLIB_VERSION}
  HOMEPAGE_URL https://github.com/KaiserLancelot/klib)

message(STATUS "${PROJECT_NAME} version: ${KLIB_VERSION}")

# ---------------------------------------------------------------------------------------
# Set default build to release
# ---------------------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      "Release"
      CACHE STRING "Choose Debug, Release, RelWithDebInfo or MinSizeRel" FORCE)
endif()
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

# ---------------------------------------------------------------------------------------
# Set variables
# ---------------------------------------------------------------------------------------
set(KLIB_LIBRARY ${PROJECT_NAME})
set(KLIB_EXECUTABLE kencrypt)
set(KLIB_TEST_EXECUTABLE ktest)
set(KLIB_BENCH_EXECUTABLE kbench)

set(KLIB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(KLIB_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

# https://cmake.org/cmake/help/latest/command/file.html#glob-recurse
file(
  GLOB_RECURSE
  KLIB_LIBRARY_SRC
  CONFIGURE_DEPENDS
  "${KLIB_SOURCE_DIR}/src/*.cpp"
  "${KLIB_SOURCE_DIR}/third_party/fastbase64/*.c"
  "${KLIB_SOURCE_DIR}/third_party/http-parser/*.c")

# ---------------------------------------------------------------------------------------
# Include CMake module
# ---------------------------------------------------------------------------------------
include(CTest)
include(GNUInstallDirs)

include(Options)
include(Check)
include(Doxygen)
include(Format)
include(CompilerOptions)

if(NOT KLIB_SANITIZER)
  message(STATUS "Use mimalloc")
  set(MIMALLOC_OBJECT "/usr/local/lib/mimalloc.o")
endif()

# ---------------------------------------------------------------------------------------
# Find package
# ---------------------------------------------------------------------------------------
find_package(PkgConfig REQUIRED)

pkg_check_modules(zlib REQUIRED IMPORTED_TARGET zlib)
pkg_check_modules(libarchive REQUIRED IMPORTED_TARGET libarchive)
pkg_check_modules(tidy REQUIRED IMPORTED_TARGET tidy)
pkg_check_modules(libcurl REQUIRED IMPORTED_TARGET libcurl)
pkg_check_modules(openssl REQUIRED IMPORTED_TARGET openssl)
pkg_check_modules(sqlcipher REQUIRED IMPORTED_TARGET sqlcipher)
pkg_check_modules(libargon2 REQUIRED IMPORTED_TARGET libargon2)
pkg_check_modules(libzstd REQUIRED IMPORTED_TARGET libzstd)

add_definitions(-DCURL_NO_OLDIES -DSQLITE_HAS_CODEC)

find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(Boost REQUIRED CONFIG)
find_package(scope_guard REQUIRED)
find_package(simdutf REQUIRED)
find_package(xxHash REQUIRED)
find_package(cmark REQUIRED)
find_package(llhttp REQUIRED)
find_package(gsl-lite REQUIRED)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  add_definitions(-Dgsl_CONFIG_CONTRACT_VIOLATION_THROWS)
endif()

add_definitions(-DDBG_MACRO_NO_WARNING)
if(NOT (${CMAKE_BUILD_TYPE} STREQUAL "Debug"))
  add_definitions(-DDBG_MACRO_DISABLE)
endif()

set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

include_directories(third_party)

# ---------------------------------------------------------------------------------------
# Build static library
# ---------------------------------------------------------------------------------------
add_definitions(
  -DLIBARCHIVE_STATIC -DCURL_STATICLIB -DFMT_LOCALE -DSPDLOG_COMPILED_LIB
  -DSPDLOG_FMT_EXTERNAL -Dgsl_CONFIG_DEFAULTS_VERSION=1)

add_library(${KLIB_LIBRARY} STATIC ${KLIB_LIBRARY_SRC})
add_library(${KLIB_LIBRARY}::${KLIB_LIBRARY} ALIAS ${KLIB_LIBRARY})

# https://cmake.org/cmake/help/latest/command/target_include_directories.html
# https://stackoverflow.com/questions/26037954/cmake-target-link-libraries-interface-dependencies
target_include_directories(
  ${KLIB_LIBRARY} PUBLIC "$<BUILD_INTERFACE:${KLIB_SOURCE_DIR}/include>"
                         "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_compile_features(${KLIB_LIBRARY} INTERFACE cxx_std_20)
target_compile_definitions(
  ${KLIB_LIBRARY} INTERFACE FMT_LOCALE SPDLOG_COMPILED_LIB SPDLOG_FMT_EXTERNAL)
target_link_libraries(${KLIB_LIBRARY} PUBLIC ${CMAKE_THREAD_LIBS_INIT})

set_target_properties(
  ${KLIB_LIBRARY} PROPERTIES OUTPUT_NAME ${KLIB_LIBRARY}
                             INTERPROCEDURAL_OPTIMIZATION FALSE)

# https://stackoverflow.com/questions/37924383/combining-several-static-libraries-into-one-using-cmake
file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party)
file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/klib)
file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/zlib)
file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/libarchive)
file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/tidy)
file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/libcurl)
file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/brotli)
file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/openssl)
file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/sqlcipher)
file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/argon2)
file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/zstd)
file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/fmt)
file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/spdlog)
file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/nghttp2)
file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/simdutf)
file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/libidn2)
file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/libpsl)
file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/libunistring)
file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/xxhash)
file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/cmark)
file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/llhttp)
add_custom_command(
  TARGET ${KLIB_LIBRARY}
  POST_BUILD
  COMMAND gcc-ar -x --output=klib $<TARGET_FILE:${KLIB_LIBRARY}>
  COMMAND gcc-ar -x --output=zlib /usr/local/lib/libz.a
  COMMAND gcc-ar -x --output=libarchive /usr/local/lib/libarchive.a
  COMMAND gcc-ar -x --output=tidy /usr/local/lib/libtidy.a
  COMMAND gcc-ar -x --output=libcurl /usr/local/lib/libcurl.a
  COMMAND gcc-ar -x --output=brotli /usr/local/lib/libbrotlicommon.a
  COMMAND gcc-ar -x --output=brotli /usr/local/lib/libbrotlidec.a
  COMMAND gcc-ar -x --output=openssl /usr/local/lib/libcrypto.a
  COMMAND gcc-ar -x --output=openssl /usr/local/lib/libssl.a
  COMMAND gcc-ar -x --output=sqlcipher /usr/local/lib/libsqlcipher.a
  COMMAND gcc-ar -x --output=argon2 /usr/local/lib/libargon2.a
  COMMAND gcc-ar -x --output=zstd /usr/local/lib/libzstd.a
  COMMAND gcc-ar -x --output=fmt /usr/local/lib/libfmt.a
  COMMAND gcc-ar -x --output=spdlog /usr/local/lib/libspdlog.a
  COMMAND gcc-ar -x --output=nghttp2 /usr/local/lib/libnghttp2.a
  COMMAND gcc-ar -x --output=simdutf /usr/local/lib/libsimdutf.a
  COMMAND gcc-ar -x --output=libidn2 /usr/local/lib/libidn2.a
  COMMAND gcc-ar -x --output=libpsl /usr/local/lib/libpsl.a
  COMMAND gcc-ar -x --output=libunistring /usr/local/lib/libunistring.a
  COMMAND gcc-ar -x --output=xxhash /usr/local/lib/libxxhash.a
  COMMAND gcc-ar -x --output=cmark /usr/local/lib/libcmark.a
  COMMAND gcc-ar -x --output=llhttp /usr/local/lib/libllhttp.a
  COMMAND
    gcc-ar -qcs ${KLIB_BINARY_DIR}/lib${KLIB_LIBRARY}.a klib/*.o zlib/*.o
    libarchive/*.o tidy/*.o libcurl/*.o brotli/*.o openssl/*.o sqlcipher/*.o
    argon2/*.o zstd/*.o fmt/*.o spdlog/*.o nghttp2/*.o simdutf/*.o libidn2/*.o
    libpsl/*.o libunistring/*.o xxhash/*.o cmark/*.o llhttp/*.o
  COMMAND gcc-ranlib ${KLIB_BINARY_DIR}/lib${KLIB_LIBRARY}.a
  WORKING_DIRECTORY ${KLIB_BINARY_DIR}/third_party
  COMMENT "Combined static library")

# ---------------------------------------------------------------------------------------
# Build test
# ---------------------------------------------------------------------------------------
if(BUILD_TESTING AND KLIB_BUILD_TEST)
  message(STATUS "Build test")
  add_subdirectory(test)
endif()

# ---------------------------------------------------------------------------------------
# Build benchmark
# ---------------------------------------------------------------------------------------
if(KLIB_BUILD_BENCH)
  message(STATUS "Build benchmark")
  add_subdirectory(bench)
endif()

# ---------------------------------------------------------------------------------------
# Build executable
# ---------------------------------------------------------------------------------------
if(KLIB_BUILD_EXECUTABLE)
  message(STATUS "Build executable program")
  add_subdirectory(tool)
endif()

# ---------------------------------------------------------------------------------------
# Install target
# ---------------------------------------------------------------------------------------
if(KLIB_INSTALL)
  message(STATUS "Generate install target")
  include(Install)

  # https://gitlab.kitware.com/cmake/community/-/wikis/FAQ#can-i-do-make-uninstall-with-cmake
  configure_file("${KLIB_SOURCE_DIR}/cmake/CMakeUninstall.cmake.in"
                 "${KLIB_BINARY_DIR}/CMakeUninstall.cmake" IMMEDIATE @ONLY)
  add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P
                                      ${KLIB_BINARY_DIR}/CMakeUninstall.cmake)
endif()
