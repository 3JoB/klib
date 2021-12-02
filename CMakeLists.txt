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
  LANGUAGES CXX
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
set(KLIB_TEST_EXECUTABLE ${KLIB_LIBRARY}-test)
set(KLIB_BENCH_EXECUTABLE ${KLIB_LIBRARY}-bench)

set(KLIB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(KLIB_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

# https://cmake.org/cmake/help/latest/command/file.html#glob-recurse
file(GLOB_RECURSE KLIB_LIBRARY_SRC CONFIGURE_DEPENDS
     "${KLIB_SOURCE_DIR}/src/*.cpp")

# ---------------------------------------------------------------------------------------
# Include CMake module
# ---------------------------------------------------------------------------------------
include(CTest)
include(GNUInstallDirs)

include(Options)
include(Check)
include(ClangTidy)
include(Doxygen)
include(Format)
include(CompilerOptions)

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
pkg_check_modules(libbrotlicommon REQUIRED IMPORTED_TARGET libbrotlicommon)

add_definitions(-DOPENSSL_NO_DEPRECATED -DCURL_NO_OLDIES -DSQLITE_HAS_CODEC)

find_package(zstd REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(Boost REQUIRED CONFIG)
find_package(scope_guard REQUIRED)
find_package(simdutf REQUIRED)
find_package(xxHash REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

# ---------------------------------------------------------------------------------------
# Build static library
# ---------------------------------------------------------------------------------------
if(KLIB_BUILD_STATIC)
  message(STATUS "Build static library")

  add_definitions(-DLIBARCHIVE_STATIC -DCURL_STATICLIB -DFMT_LOCALE
                  -DSPDLOG_COMPILED_LIB -DSPDLOG_FMT_EXTERNAL)

  add_library(${KLIB_LIBRARY} STATIC ${KLIB_LIBRARY_SRC})
  add_library(${KLIB_LIBRARY}::${KLIB_LIBRARY} ALIAS ${KLIB_LIBRARY})

  # https://cmake.org/cmake/help/latest/command/target_include_directories.html
  # https://stackoverflow.com/questions/26037954/cmake-target-link-libraries-interface-dependencies
  target_include_directories(
    ${KLIB_LIBRARY} PUBLIC "$<BUILD_INTERFACE:${KLIB_SOURCE_DIR}/include>"
                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
  target_compile_features(${KLIB_LIBRARY} INTERFACE cxx_std_20)
  target_compile_definitions(
    ${KLIB_LIBRARY} INTERFACE FMT_LOCALE SPDLOG_COMPILED_LIB
                              SPDLOG_FMT_EXTERNAL)
  target_link_libraries(${KLIB_LIBRARY} PUBLIC ${CMAKE_THREAD_LIBS_INIT})

  set_target_properties(
    ${KLIB_LIBRARY}
    PROPERTIES OUTPUT_NAME ${KLIB_LIBRARY}
               POSITION_INDEPENDENT_CODE ON
               INTERPROCEDURAL_OPTIMIZATION FALSE)

  # https://stackoverflow.com/questions/37924383/combining-several-static-libraries-into-one-using-cmake
  file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party)
  file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/xxhash)
  add_custom_command(
    TARGET ${KLIB_LIBRARY}
    POST_BUILD
    COMMAND ar -x $<TARGET_FILE:${KLIB_LIBRARY}>
    COMMAND ar -x /usr/local/lib/libz.a
    COMMAND ar -x /usr/local/lib/libarchive.a
    COMMAND ar -x /usr/local/lib/libtidy.a
    COMMAND ar -x /usr/local/lib/libbrotlicommon.a
    COMMAND ar -x /usr/local/lib/libbrotlidec.a
    COMMAND ar -x /usr/local/lib/libcurl.a
    COMMAND ar -x /usr/local/lib/libcrypto.a
    COMMAND ar -x /usr/local/lib/libssl.a
    COMMAND ar -x /usr/local/lib/libsqlcipher.a
    COMMAND ar -x /usr/local/lib/libargon2.a
    COMMAND ar -x /usr/local/lib/libzstd.a
    COMMAND ar -x /usr/local/lib/libfmt.a
    COMMAND ar -x /usr/local/lib/libspdlog.a
    COMMAND ar -x /usr/local/lib/libnghttp2.a
    COMMAND ar -x /usr/local/lib/libsimdutf.a
    COMMAND ar -x --output=xxhash /usr/local/lib/libxxhash.a
    COMMAND ar -qcs ${KLIB_BINARY_DIR}/lib${KLIB_LIBRARY}.a *.o xxhash/*.o
    COMMAND ranlib ${KLIB_BINARY_DIR}/lib${KLIB_LIBRARY}.a
    WORKING_DIRECTORY ${KLIB_BINARY_DIR}/third_party
    COMMENT "Combined static library")
endif()

# ---------------------------------------------------------------------------------------
# Build shared library
# ---------------------------------------------------------------------------------------
if(KLIB_BUILD_SHARED)
  message(STATUS "Build shared library")

  add_library(${KLIB_LIBRARY}-shared SHARED ${KLIB_LIBRARY_SRC})
  add_library(${KLIB_LIBRARY}::${KLIB_LIBRARY}-shared ALIAS
              ${KLIB_LIBRARY}-shared)

  target_include_directories(
    ${KLIB_LIBRARY}-shared
    PUBLIC "$<BUILD_INTERFACE:${KLIB_SOURCE_DIR}/include>"
           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
  target_compile_features(${KLIB_LIBRARY}-shared INTERFACE cxx_std_20)
  target_link_libraries(
    ${KLIB_LIBRARY}-shared
    PRIVATE PkgConfig::zlib
            PkgConfig::libarchive
            PkgConfig::tidy
            PkgConfig::libcurl
            PkgConfig::openssl
            PkgConfig::sqlcipher
            PkgConfig::libargon2
            PkgConfig::libbrotlicommon
            zstd::libzstd_static
            scope_guard::scope_guard
            simdutf::simdutf
            xxHash::xxhash
            ${Boost_LIBRARIES})
  target_link_libraries(${KLIB_LIBRARY}-shared PUBLIC fmt::fmt spdlog::spdlog)

  set_target_properties(
    ${KLIB_LIBRARY}-shared
    PROPERTIES OUTPUT_NAME ${KLIB_LIBRARY}
               VERSION ${KLIB_VERSION}
               SOVERSION ${KLIB_VERSION_MAJOR})
endif()

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
