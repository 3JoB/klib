# ---------------------------------------------------------------------------------------
# Include files and executable
# ---------------------------------------------------------------------------------------
include(GNUInstallDirs)

# https://stackoverflow.com/questions/30398238/cmake-rpath-not-working-could-not-find-shared-object-file
set_target_properties(
  ${KLIB_EXECUTABLE}
  PROPERTIES INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
             INSTALL_RPATH_USE_LINK_PATH TRUE)

install(DIRECTORY "include/" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
  TARGETS ${KLIB_LIBRARY} ${KLIB_EXECUTABLE} fastbase64
  EXPORT KLIB_EXPORTS
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# ---------------------------------------------------------------------------------------
# Install CMake config files
# ---------------------------------------------------------------------------------------
set(KLIB_EXPORT_DEST_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${KLIB_LIBRARY}")
set(KLIB_CONFIG_TARGETS_FILE "${KLIB_LIBRARY}ConfigTargets.cmake")

install(
  EXPORT KLIB_EXPORTS
  DESTINATION ${KLIB_EXPORT_DEST_DIR}
  NAMESPACE ${KLIB_LIBRARY}::
  FILE ${KLIB_CONFIG_TARGETS_FILE})

set(KLIB_PROJECT_CONFIG_IN
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${KLIB_LIBRARY}Config.cmake.in")
set(KLIB_PROJECT_CONFIG_OUT
    "${CMAKE_CURRENT_BINARY_DIR}/${KLIB_LIBRARY}Config.cmake")
set(KLIB_VERSION_CONFIG_FILE
    "${CMAKE_CURRENT_BINARY_DIR}/${KLIB_LIBRARY}ConfigVersion.cmake")

# https://cmake.org/cmake/help/latest/module/CMakePackageConfigHelpers.html#example-generating-package-files
include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${KLIB_PROJECT_CONFIG_IN} ${KLIB_PROJECT_CONFIG_OUT}
  INSTALL_DESTINATION ${KLIB_EXPORT_DEST_DIR})
write_basic_package_version_file(${KLIB_VERSION_CONFIG_FILE}
                                 COMPATIBILITY SameMajorVersion)

install(FILES ${KLIB_PROJECT_CONFIG_OUT} ${KLIB_VERSION_CONFIG_FILE}
        DESTINATION ${KLIB_EXPORT_DEST_DIR})

# ---------------------------------------------------------------------------------------
# Support creation of installable packages
# ---------------------------------------------------------------------------------------
# https://cmake.org/cmake/help/latest/module/CPack.html
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_INSTALL_CMAKE_PROJECTS ${KLIB_BINARY_DIR} ${PROJECT_NAME} ALL .)

# https://cmake.org/cmake/help/latest/cpack_gen/deb.html
set(CPACK_PACKAGE_CONTACT "Kaiser <KaiserLancelot123@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "A C++ library, which wraps several important C libraries and provides some convenient functions"
)
set(CPACK_PACKAGE_VERSION
    ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

# https://cmake.org/cmake/help/latest/cpack_gen/archive.html
# https://cmake.org/cmake/help/latest/cpack_gen/deb.html
set(CPACK_GENERATOR "TZST;DEB")
# FIXME set(CPACK_DEBIAN_COMPRESSION_TYPE "zstd")

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)
