enable_testing()

if(KLIB_TEST_USE_PROXY)
  add_definitions(-DKLIB_TEST_USE_PROXY)
endif()

add_subdirectory(unit_test)

if(KLIB_BUILD_EXTRA_TEST)
  message(STATUS "Build extra test")
  add_subdirectory(extra_test)
endif()

include(Coverage)
include(Valgrind)
