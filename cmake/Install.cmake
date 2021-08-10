# ---------------------------------------------------------------------------------------
# Include files
# ---------------------------------------------------------------------------------------
include(GNUInstallDirs)

install(DIRECTORY "include/" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
  TARGETS ${LIBRARY} ${LIBRARY}-shared
  EXPORT KLIB_EXPORTS
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# ---------------------------------------------------------------------------------------
# Install CMake config files
# ---------------------------------------------------------------------------------------
set(KLIB_EXPORT_DEST_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY}")
set(KLIB_CONFIG_TARGETS_FILE "${LIBRARY}ConfigTargets.cmake")

install(
  EXPORT KLIB_EXPORTS
  DESTINATION ${KLIB_EXPORT_DEST_DIR}
  NAMESPACE ${LIBRARY}::
  FILE ${KLIB_CONFIG_TARGETS_FILE})

set(KLIB_PROJECT_CONFIG_IN
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${LIBRARY}Config.cmake.in")
set(KLIB_PROJECT_CONFIG_OUT
    "${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY}Config.cmake")
set(KLIB_VERSION_CONFIG_FILE
    "${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY}ConfigVersion.cmake")

configure_file(${KLIB_PROJECT_CONFIG_IN} ${KLIB_PROJECT_CONFIG_OUT} @ONLY)

include(CMakePackageConfigHelpers)
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
set(CPACK_PACKAGE_CONTACT "kaiser <KaiserLancelot123@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++ Library")
set(CPACK_PACKAGE_VERSION
    ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

# https://cmake.org/cmake/help/latest/manual/cpack-generators.7.html
set(CPACK_GENERATOR "TGZ;DEB")

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)
