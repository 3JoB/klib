file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/zlib-v1.2.11.tar.gz
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/folder1
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/folder2
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# https://cmake.org/cmake/help/latest/manual/cmake.1.html#run-a-command-line-tool
add_custom_target(
  decompress
  COMMAND ${CMAKE_COMMAND} -E tar xf zlib-v1.2.11.tar.gz
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Unpacking zlib-v1.2.11.tar.gz")

file(GLOB TEST_SRC CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

find_package(Catch2 REQUIRED)

add_executable(${TEST_EXECUTABLE} ${TEST_SRC})
add_dependencies(${TEST_EXECUTABLE} decompress)

target_link_libraries(${TEST_EXECUTABLE} PRIVATE ${LIBRARY}
                                                 Catch2::Catch2WithMain)

# https://github.com/catchorg/Catch2/blob/devel/docs/cmake-integration.md
include(Catch)
catch_discover_tests(${TEST_EXECUTABLE} REPORTER compact)
