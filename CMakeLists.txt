# https://github.com/gabime/spdlog/blob/v1.x/CMakeLists.txt
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
  LANGUAGES CXX)

message(STATUS "${PROJECT_NAME} version: ${KLIB_VERSION}")

# ---------------------------------------------------------------------------------------
# Set default build to release
# ---------------------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      "Release"
      CACHE STRING "Choose Debug, Release, MinSizeRel or RelWithDebInfo" FORCE)
endif()
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

# ---------------------------------------------------------------------------------------
# Set variables
# ---------------------------------------------------------------------------------------
set(LIBRARY ${PROJECT_NAME})
set(EXECUTABLE tool)
set(TEST_EXECUTABLE catch2)
set(BENCHMARK_EXECUTABLE bench)

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
find_package(fmt REQUIRED)
find_package(ZLIB REQUIRED)
find_package(LibArchive REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

if(KLIB_TEST_USE_PROXY)
  add_definitions(-DKLIB_TEST_USE_PROXY)
endif()

# ---------------------------------------------------------------------------------------
# Build static library
# ---------------------------------------------------------------------------------------
if(KLIB_BUILD_STATIC)
  message(STATUS "Build static library")

  add_library(${LIBRARY} STATIC ${LIBRARY_SRC})
  add_library(${LIBRARY}::${LIBRARY} ALIAS ${LIBRARY})

  # https://cmake.org/cmake/help/latest/command/target_include_directories.html
  # https://stackoverflow.com/questions/26037954/cmake-target-link-libraries-interface-dependencies
  target_include_directories(
    ${LIBRARY} PUBLIC "$<BUILD_INTERFACE:${KLIB_SOURCE_DIR}/include>"
                      "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
  target_compile_features(${LIBRARY} INTERFACE cxx_std_20)

  set_target_properties(
    ${LIBRARY}
    PROPERTIES OUTPUT_NAME ${LIBRARY}
               POSITION_INDEPENDENT_CODE ON
               INTERPROCEDURAL_OPTIMIZATION FALSE)

  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/third-party)
  add_custom_command(
    TARGET ${LIBRARY}
    POST_BUILD
    COMMAND ar -x $<TARGET_FILE:${LIBRARY}>
    COMMAND ar -x /usr/local/lib/libcurl.a
    COMMAND ar -x /usr/local/lib/libfmt.a
    COMMAND ar -x /usr/local/lib/libcrypto.a
    COMMAND ar -x /usr/local/lib/libssl.a
    COMMAND ar -x /usr/local/lib/libarchive.a
    COMMAND ar -x /usr/local/lib/libz.a
    COMMAND ar -qcs ../libklib.a *.o
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/third-party
    COMMENT "combined static library")
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
    ${LIBRARY}-shared PRIVATE CURL::libcurl fmt::fmt OpenSSL::Crypto
                              ${LibArchive_LIBRARIES} ${ZLIB_LIBRARIES})

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
if(KLIB_BUILD_BENCHMARK)
  message(STATUS "Build benchmark")
  add_subdirectory(bench)
endif()

# ---------------------------------------------------------------------------------------
# Install target
# ---------------------------------------------------------------------------------------
if(KLIB_INSTALL)
  message(STATUS "Generate install")
  include(Install)

  # https://gitlab.kitware.com/cmake/community/-/wikis/FAQ#can-i-do-make-uninstall-with-cmake
  configure_file("${KLIB_SOURCE_DIR}/cmake/CMakeUninstall.cmake.in"
                 "${KLIB_BINARY_DIR}/CMakeUninstall.cmake" @ONLY)
  add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P
                                      ${KLIB_BINARY_DIR}/CMakeUninstall.cmake)
endif()
