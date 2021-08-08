# ---------------------------------------------------------------------------------------
# CMake version
# ---------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.20)
message(STATUS "CMake version: ${CMAKE_VERSION}")

# ---------------------------------------------------------------------------------------
# Start klib project
# ---------------------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(Version)
klib_extract_version()

project(
  klib
  VERSION ${KLIB_VERSION}
  LANGUAGES CXX ASM)

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
set(LIBRARY ${PROJECT_NAME})
set(TEST_EXECUTABLE ${LIBRARY}-test)
set(BENCH_EXECUTABLE ${LIBRARY}-bench)

set(KLIB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(KLIB_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

# https://cmake.org/cmake/help/latest/command/file.html#glob-recurse
file(GLOB_RECURSE LIBRARY_SRC CONFIGURE_DEPENDS "${KLIB_SOURCE_DIR}/src/*.cpp")

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
find_package(LibArchive REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(CURL CONFIG REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(tidy REQUIRED IMPORTED_TARGET tidy)

find_package(fmt REQUIRED)
find_package(pugixml REQUIRED)
find_package(Boost CONFIG REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

if(KLIB_TEST_USE_PROXY)
  add_definitions(-DKLIB_TEST_USE_PROXY)
endif()

# ---------------------------------------------------------------------------------------
# Build style
# ---------------------------------------------------------------------------------------
file(COPY "${KLIB_SOURCE_DIR}/style/style.css" DESTINATION ${KLIB_BINARY_DIR})

add_library(style STATIC ${KLIB_SOURCE_DIR}/src/epub.s)
target_compile_options(style PRIVATE "-Wno-unused-command-line-argument")

# ---------------------------------------------------------------------------------------
# Build static library
# ---------------------------------------------------------------------------------------
if(KLIB_BUILD_STATIC)
  message(STATUS "Build static library")

  add_definitions(-DCURL_STATICLIB -DFMT_LOCALE)

  add_library(${LIBRARY} STATIC ${LIBRARY_SRC})
  add_library(${LIBRARY}::${LIBRARY} ALIAS ${LIBRARY})
  add_dependencies(${LIBRARY} style)

  # https://cmake.org/cmake/help/latest/command/target_include_directories.html
  # https://stackoverflow.com/questions/26037954/cmake-target-link-libraries-interface-dependencies
  target_include_directories(
    ${LIBRARY} PUBLIC "$<BUILD_INTERFACE:${KLIB_SOURCE_DIR}/include>"
                      "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
  target_compile_features(${LIBRARY} INTERFACE cxx_std_20)
  target_link_libraries(${LIBRARY} PUBLIC ${CMAKE_DL_LIBS}
                                          ${CMAKE_THREAD_LIBS_INIT})

  set_target_properties(
    ${LIBRARY}
    PROPERTIES OUTPUT_NAME ${LIBRARY}
               POSITION_INDEPENDENT_CODE ON
               INTERPROCEDURAL_OPTIMIZATION FALSE)

  # https://stackoverflow.com/questions/37924383/combining-several-static-libraries-into-one-using-cmake
  file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party)
  add_custom_command(
    TARGET ${LIBRARY}
    POST_BUILD
    COMMAND ar -x $<TARGET_FILE:${LIBRARY}>
    COMMAND ar -x $<TARGET_FILE:style>
    COMMAND ar -x /usr/local/lib/libz.a
    COMMAND ar -x /usr/local/lib/libarchive.a
    COMMAND ar -x /usr/local/lib/libcrypto.a
    COMMAND ar -x /usr/local/lib/libssl.a
    COMMAND ar -x /usr/local/lib/libnghttp2.a
    COMMAND ar -x /usr/local/lib/libcurl.a
    COMMAND ar -x /usr/local/lib/libtidy.a
    COMMAND ar -x /usr/local/lib/libfmt.a
    COMMAND ar -x /usr/local/lib/libpugixml.a
    COMMAND ar -qcs ${KLIB_BINARY_DIR}/lib${LIBRARY}.a *.o
    WORKING_DIRECTORY ${KLIB_BINARY_DIR}/third_party
    COMMENT "Combined static library")
endif()

# ---------------------------------------------------------------------------------------
# Build shared library
# ---------------------------------------------------------------------------------------
if(KLIB_BUILD_SHARED)
  message(STATUS "Build shared library")

  add_library(${LIBRARY}-shared SHARED ${LIBRARY_SRC})
  add_library(${LIBRARY}::${LIBRARY}-shared ALIAS ${LIBRARY}-shared)

  target_include_directories(
    ${LIBRARY}-shared PUBLIC "$<BUILD_INTERFACE:${KLIB_SOURCE_DIR}/include>"
                             "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
  target_compile_features(${LIBRARY}-shared INTERFACE cxx_std_20)
  target_link_libraries(
    ${LIBRARY}-shared
    PRIVATE style
            ${LibArchive_LIBRARIES}
            OpenSSL::Crypto
            CURL::libcurl
            PkgConfig::tidy
            fmt::fmt
            pugixml::pugixml)

  set_target_properties(
    ${LIBRARY}-shared
    PROPERTIES OUTPUT_NAME ${LIBRARY}
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
