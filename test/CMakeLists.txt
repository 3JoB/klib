enable_testing()

add_subdirectory(unit_test)

if(KLIB_BUILD_EXTRA_TEST)
  message(STATUS "Build extra test")
  add_subdirectory(cmake_add_subdirectory)
  add_subdirectory(cmake_import)
  add_subdirectory(cmake_import_minver)
endif()

include(Coverage)
include(Valgrind)
