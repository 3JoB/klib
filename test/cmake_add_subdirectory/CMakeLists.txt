add_test(
  NAME cmake_add_subdirectory_configure
  COMMAND
    ${CMAKE_COMMAND} -S ${CMAKE_CURRENT_SOURCE_DIR}/project -B
    ${CMAKE_CURRENT_SOURCE_DIR}/project/build -DCMAKE_BUILD_TYPE=RelWithDebInfo
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DKLIB_SOURCE=${KLIB_SOURCE_DIR})

add_test(
  NAME cmake_add_subdirectory_build
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_SOURCE_DIR}/project/build
          --config RelWithDebInfo)

add_test(NAME cmake_add_subdirectory_clean
         COMMAND rm -rf ${CMAKE_CURRENT_SOURCE_DIR}/project/build)

# https://crascit.com/2016/10/18/test-fixtures-with-cmake-ctest/
set_tests_properties(cmake_add_subdirectory_configure
                     PROPERTIES FIXTURES_SETUP cmake_add_subdirectory)
set_tests_properties(cmake_add_subdirectory_build
                     PROPERTIES FIXTURES_REQUIRED cmake_add_subdirectory)
set_tests_properties(cmake_add_subdirectory_clean
                     PROPERTIES FIXTURES_CLEANUP cmake_add_subdirectory)
