add_test(
  NAME cmake_import_minver_configure
  COMMAND
    ${CMAKE_COMMAND} -S ${CMAKE_CURRENT_SOURCE_DIR}/project -B
    ${CMAKE_CURRENT_SOURCE_DIR}/project/build -DCMAKE_BUILD_TYPE=RelWithDebInfo
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER})

add_test(
  NAME cmake_import_minver_build
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_SOURCE_DIR}/project/build
          --config RelWithDebInfo)

add_test(NAME cmake_import_minver_clean
         COMMAND rm -rf ${CMAKE_CURRENT_SOURCE_DIR}/project/build)

set_tests_properties(cmake_import_minver_configure
                     PROPERTIES FIXTURES_SETUP cmake_import_minver)
set_tests_properties(cmake_import_minver_build PROPERTIES FIXTURES_REQUIRED
                                                          cmake_import_minver)
set_tests_properties(cmake_import_minver_clean PROPERTIES FIXTURES_CLEANUP
                                                          cmake_import_minver)
