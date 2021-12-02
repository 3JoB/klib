if(KLIB_TEST_USE_PROXY)
  add_definitions(-DKLIB_TEST_USE_PROXY)
endif()

if(NOT (${CMAKE_BUILD_TYPE} STREQUAL "Debug"))
  add_definitions(-DDBG_MACRO_DISABLE -DDBG_MACRO_NO_WARNING)
endif()

find_package(Boost CONFIG REQUIRED COMPONENTS json)

file(GLOB_RECURSE KLIB_TEST_FILE CONFIGURE_DEPENDS "${KLIB_SOURCE_DIR}/data/*")
file(COPY ${KLIB_TEST_FILE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB_RECURSE KLIB_TEST_SRC CONFIGURE_DEPENDS
     ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

find_package(Catch2 REQUIRED)

add_executable(${KLIB_TEST_EXECUTABLE} ${KLIB_TEST_SRC})
target_link_libraries(
  ${KLIB_TEST_EXECUTABLE} PRIVATE ${KLIB_LIBRARY} Catch2::Catch2WithMain
                                  ${Boost_LIBRARIES})

add_custom_target(
  test_decompress
  COMMAND ${CMAKE_COMMAND} -E tar zxf zlib-v1.2.11.tar.gz
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Unpacking zlib-v1.2.11.tar.gz")
add_dependencies(${KLIB_TEST_EXECUTABLE} test_decompress)

include(Catch)
catch_discover_tests(${KLIB_TEST_EXECUTABLE} REPORTER compact)
