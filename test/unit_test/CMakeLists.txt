find_package(Boost CONFIG REQUIRED COMPONENTS json)

file(COPY ${KLIB_SOURCE_DIR}/data/zlib-v1.2.11.tar.gz
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(COPY ${KLIB_SOURCE_DIR}/data/folder1
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(COPY ${KLIB_SOURCE_DIR}/data/folder2
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB_RECURSE TEST_SRC CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

find_package(Catch2 REQUIRED)

add_executable(${TEST_EXECUTABLE} ${TEST_SRC})
target_link_libraries(
  ${TEST_EXECUTABLE} PRIVATE ${LIBRARY} Catch2::Catch2WithMain
                             ${Boost_LIBRARIES})

add_custom_target(
  test_decompress
  COMMAND ${CMAKE_COMMAND} -E tar zxf zlib-v1.2.11.tar.gz
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Unpacking zlib-v1.2.11.tar.gz")
add_dependencies(${TEST_EXECUTABLE} test_decompress)

include(Catch)
catch_discover_tests(${TEST_EXECUTABLE} REPORTER compact)
