# ---------------------------------------------------------------------------------------
# CMake version
# ---------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.18)
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
  "${KLIB_SOURCE_DIR}/third_party/http-parser/*.c"
  "${KLIB_SOURCE_DIR}/third_party/sfntly/*.cc"
  "${KLIB_SOURCE_DIR}/third_party/subtly/*.cc"
  "${KLIB_SOURCE_DIR}/third_party/utf_utils/*.cpp")

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
pkg_check_modules(libwoff2dec REQUIRED IMPORTED_TARGET libwoff2dec)
pkg_check_modules(libwoff2enc REQUIRED IMPORTED_TARGET libwoff2enc)
pkg_check_modules(libwoff2common REQUIRED IMPORTED_TARGET libwoff2common)
pkg_check_modules(libbrotlidec REQUIRED IMPORTED_TARGET libbrotlidec)
pkg_check_modules(libbrotlienc REQUIRED IMPORTED_TARGET libbrotlienc)
pkg_check_modules(libbrotlicommon REQUIRED IMPORTED_TARGET libbrotlicommon)
pkg_check_modules(vips REQUIRED IMPORTED_TARGET vips)
pkg_check_modules(vips-cpp REQUIRED IMPORTED_TARGET vips-cpp)
pkg_check_modules(gio-2.0 REQUIRED IMPORTED_TARGET gio-2.0)
pkg_check_modules(gobject-2.0 REQUIRED IMPORTED_TARGET gobject-2.0)
pkg_check_modules(gmodule-2.0 REQUIRED IMPORTED_TARGET gmodule-2.0)
pkg_check_modules(glib-2.0 REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(libpcre REQUIRED IMPORTED_TARGET libpcre)
pkg_check_modules(expat REQUIRED IMPORTED_TARGET expat)
pkg_check_modules(libffi REQUIRED IMPORTED_TARGET libffi)
pkg_check_modules(mount REQUIRED IMPORTED_TARGET mount)
pkg_check_modules(blkid REQUIRED IMPORTED_TARGET blkid)
pkg_check_modules(libspng REQUIRED IMPORTED_TARGET libspng)
pkg_check_modules(libjpeg REQUIRED IMPORTED_TARGET libjpeg)
pkg_check_modules(libwebp REQUIRED IMPORTED_TARGET libwebp)
pkg_check_modules(libwebpmux REQUIRED IMPORTED_TARGET libwebpmux)
pkg_check_modules(libwebpdemux REQUIRED IMPORTED_TARGET libwebpdemux)

find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(Boost REQUIRED CONFIG)
find_package(scope_guard REQUIRED)
find_package(simdutf REQUIRED)
find_package(xxHash REQUIRED)
find_package(cmark REQUIRED)
find_package(utf8cpp REQUIRED)
find_package(Microsoft.GSL REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_definitions(-DDBG_MACRO_NO_WARNING)
if(NOT (${CMAKE_BUILD_TYPE} STREQUAL "Debug"))
  add_definitions(-DDBG_MACRO_DISABLE)
endif()

include_directories(${gio-2.0_INCLUDE_DIRS})
include_directories(third_party)

# ---------------------------------------------------------------------------------------
# Build static library
# ---------------------------------------------------------------------------------------
if(KLIB_BUILD_STATIC)
  message(STATUS "Build status library")

  add_library(${KLIB_LIBRARY} STATIC ${KLIB_LIBRARY_SRC})
  add_library(${KLIB_LIBRARY}::${KLIB_LIBRARY} ALIAS ${KLIB_LIBRARY})

  # https://cmake.org/cmake/help/latest/command/target_include_directories.html
  # https://stackoverflow.com/questions/26037954/cmake-target-link-libraries-interface-dependencies
  target_include_directories(
    ${KLIB_LIBRARY} PUBLIC "$<BUILD_INTERFACE:${KLIB_SOURCE_DIR}/include>"
                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
  target_compile_features(${KLIB_LIBRARY} INTERFACE cxx_std_20)
  target_compile_definitions(
    ${KLIB_LIBRARY} PUBLIC FMT_LOCALE SPDLOG_COMPILED_LIB SPDLOG_FMT_EXTERNAL)
  target_compile_definitions(
    ${KLIB_LIBRARY}
    PRIVATE SFNTLY_EXPERIMENTAL
            SFNTLY_NO_EXCEPTION
            PCRE_STATIC
            LIBARCHIVE_STATIC
            CURL_STATICLIB
            CURL_NO_OLDIES
            SQLITE_HAS_CODEC)
  target_link_libraries(${KLIB_LIBRARY} INTERFACE ${CMAKE_THREAD_LIBS_INIT}
                                                  ${CMAKE_DL_LIBS})

  set_target_properties(
    ${KLIB_LIBRARY} PROPERTIES OUTPUT_NAME ${KLIB_LIBRARY}
                               INTERPROCEDURAL_OPTIMIZATION FALSE)

  # https://stackoverflow.com/questions/37924383/combining-several-static-libraries-into-one-using-cmake
  file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party)
  file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/klib)
  file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/zlib-ng)
  file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/libarchive)
  file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/tidy-html5)
  file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/libcurl)
  file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/brotli)
  file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/boringssl)
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
  file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/woff2)
  file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/libvips)
  file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/glib)
  file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/libexpat)
  file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/libffi)
  file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/blkid)
  file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/mount)
  file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/libpcre)
  file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/libspng)
  file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/mozjpeg)
  file(MAKE_DIRECTORY ${KLIB_BINARY_DIR}/third_party/libwebp)
  add_custom_command(
    TARGET ${KLIB_LIBRARY}
    POST_BUILD
    COMMAND gcc-ar -x --output=klib $<TARGET_FILE:${KLIB_LIBRARY}>
    COMMAND gcc-ar -x --output=zlib-ng /usr/local/lib/libz.a
    COMMAND gcc-ar -x --output=libarchive /usr/local/lib/libarchive.a
    COMMAND gcc-ar -x --output=tidy-html5 /usr/local/lib/libtidy.a
    COMMAND gcc-ar -x --output=libcurl /usr/local/lib/libcurl.a
    COMMAND gcc-ar -x --output=brotli /usr/local/lib/libbrotlicommon.a
    COMMAND gcc-ar -x --output=brotli /usr/local/lib/libbrotlidec.a
    COMMAND gcc-ar -x --output=brotli /usr/local/lib/libbrotlienc.a
    COMMAND gcc-ar -x --output=boringssl /usr/local/lib/libcrypto.a
    COMMAND gcc-ar -x --output=boringssl /usr/local/lib/libssl.a
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
    COMMAND gcc-ar -x --output=woff2 /usr/local/lib/libwoff2common.a
    COMMAND gcc-ar -x --output=woff2 /usr/local/lib/libwoff2dec.a
    COMMAND gcc-ar -x --output=woff2 /usr/local/lib/libwoff2enc.a
    COMMAND gcc-ar -x --output=libvips /usr/local/lib/libvips.a
    COMMAND gcc-ar -x --output=libvips /usr/local/lib/libvips-cpp.a
    COMMAND gcc-ar -x --output=glib /usr/local/lib/libgio-2.0.a
    COMMAND gcc-ar -x --output=glib /usr/local/lib/libglib-2.0.a
    COMMAND gcc-ar -x --output=glib /usr/local/lib/libgmodule-2.0.a
    COMMAND gcc-ar -x --output=glib /usr/local/lib/libgobject-2.0.a
    COMMAND gcc-ar -x --output=libexpat /usr/local/lib/libexpat.a
    COMMAND gcc-ar -x --output=libffi /usr/local/lib/libffi.a
    COMMAND gcc-ar -x --output=mount /usr/local/lib/libmount.a
    COMMAND gcc-ar -x --output=blkid /usr/local/lib/libblkid.a
    COMMAND gcc-ar -x --output=libpcre /usr/local/lib/libpcre.a
    COMMAND gcc-ar -x --output=libspng /usr/local/lib/libspng_static.a
    COMMAND gcc-ar -x --output=mozjpeg /usr/local/lib/libjpeg.a
    COMMAND gcc-ar -x --output=libwebp /usr/local/lib/libwebp.a
    COMMAND gcc-ar -x --output=libwebp /usr/local/lib/libwebpmux.a
    COMMAND gcc-ar -x --output=libwebp /usr/local/lib/libwebpdemux.a
    COMMAND
      gcc-ar -qcs ${KLIB_BINARY_DIR}/lib${KLIB_LIBRARY}.a klib/*.o zlib-ng/*.o
      libarchive/*.o tidy-html5/*.o libcurl/*.o brotli/*.o boringssl/*.o
      sqlcipher/*.o argon2/*.o zstd/*.o fmt/*.o spdlog/*.o nghttp2/*.o
      simdutf/*.o libidn2/*.o libpsl/*.o libunistring/*.o xxhash/*.o cmark/*.o
      woff2/*.o libvips/*.o glib/*.o libexpat/*.o libffi/*.o mount/*.o blkid/*.o
      libpcre/*.o libspng/*.o mozjpeg/*.o libwebp/*.o
    COMMAND gcc-ranlib ${KLIB_BINARY_DIR}/lib${KLIB_LIBRARY}.a
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
  target_compile_definitions(
    ${KLIB_LIBRARY}-shared PRIVATE SFNTLY_EXPERIMENTAL SFNTLY_NO_EXCEPTION
                                   CURL_NO_OLDIES SQLITE_HAS_CODEC)
  target_link_libraries(${KLIB_LIBRARY}-shared PUBLIC fmt::fmt spdlog::spdlog)
  target_link_libraries(
    ${KLIB_LIBRARY}-shared
    PRIVATE PkgConfig::zlib
            PkgConfig::libarchive
            PkgConfig::tidy
            PkgConfig::libcurl
            PkgConfig::openssl
            PkgConfig::sqlcipher
            PkgConfig::libargon2
            PkgConfig::libzstd
            PkgConfig::libwoff2dec
            PkgConfig::libwoff2enc
            PkgConfig::libwoff2common
            PkgConfig::libbrotlidec
            PkgConfig::libbrotlienc
            PkgConfig::libbrotlicommon
            PkgConfig::vips
            PkgConfig::vips-cpp
            PkgConfig::gio-2.0
            PkgConfig::expat
            PkgConfig::libffi
            PkgConfig::mount
            PkgConfig::blkid
            PkgConfig::libspng
            PkgConfig::libjpeg
            PkgConfig::libwebp
            PkgConfig::libwebpmux
            PkgConfig::libwebpdemux
            ${Boost_LIBRARIES}
            scope_guard::scope_guard
            simdutf::simdutf
            xxHash::xxhash
            cmark::cmark_static
            utf8cpp
            Microsoft.GSL::GSL
            ${CMAKE_THREAD_LIBS_INIT}
            ${CMAKE_DL_LIBS})

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
