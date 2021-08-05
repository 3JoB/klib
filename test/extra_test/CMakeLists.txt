file(
  GLOB CHILDREN
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "${CMAKE_CURRENT_SOURCE_DIR}/*")

set(SUBDIRS "")

foreach(CHILD ${CHILDREN})
  if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${CHILD}")
    list(APPEND SUBDIRS ${CHILD})
  endif()
endforeach()

foreach(SUBDIR ${SUBDIRS})
  add_test(
    NAME ${SUBDIR}_configure
    COMMAND
      ${CMAKE_COMMAND} -S ${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR} -B
      ${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}/build
      -DCMAKE_BUILD_TYPE=RelWithDebInfo
      -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
      -DKLIB_SOURCE_DIR=${KLIB_SOURCE_DIR}
      -DKLIB_TEST_USE_PROXY=${KLIB_TEST_USE_PROXY})

  add_test(
    NAME ${SUBDIR}_build
    COMMAND ${CMAKE_COMMAND} --build
            ${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}/build --config RelWithDebInfo)

  add_test(NAME ${SUBDIR}_clean
           COMMAND rm -rf ${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}/build)

  # https://crascit.com/2016/10/18/test-fixtures-with-cmake-ctest/
  set_tests_properties(${SUBDIR}_configure PROPERTIES FIXTURES_SETUP ${SUBDIR})
  set_tests_properties(${SUBDIR}_build PROPERTIES FIXTURES_REQUIRED ${SUBDIR})
  set_tests_properties(${SUBDIR}_clean PROPERTIES FIXTURES_CLEANUP ${SUBDIR})
endforeach()
