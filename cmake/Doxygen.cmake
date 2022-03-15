if(KLIB_BUILD_DOC)
  message(STATUS "Build documentation")

  find_program(DOXYGEN_EXECUTABLE doxygen)
  if(NOT DOXYGEN_EXECUTABLE)
    message(FATAL_ERROR "Can not find doxygen")
  endif()

  set(INPUT_DIR "${KLIB_SOURCE_DIR}/include/klib")
  set(OUTPUT_DIR ${KLIB_BINARY_DIR})

  configure_file("${KLIB_SOURCE_DIR}/doc/Doxyfile.in"
                 "${KLIB_BINARY_DIR}/Doxyfile" @ONLY)

  add_custom_target(
    doc
    COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
    WORKING_DIRECTORY ${KLIB_BINARY_DIR}
    COMMENT "Generate documentation ${KLIB_BINARY_DIR}/html/index.html")
endif()
