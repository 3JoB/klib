file(COPY ${KLIB_SOURCE_DIR}/data/zlib-v1.2.11.tar.gz
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB_RECURSE BENCH_SRC CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

find_package(Catch2 REQUIRED)
add_definitions(-DCATCH_CONFIG_ENABLE_BENCHMARKING)

add_executable(${KLIB_BENCH_EXECUTABLE} ${BENCH_SRC})
target_link_libraries(${KLIB_BENCH_EXECUTABLE} PRIVATE ${KLIB_LIBRARY}
                                                       Catch2::Catch2WithMain)

add_custom_target(
  bench_decompress
  COMMAND ${CMAKE_COMMAND} -E tar zxf zlib-v1.2.11.tar.gz
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Unpacking zlib-v1.2.11.tar.gz")
add_dependencies(${KLIB_BENCH_EXECUTABLE} bench_decompress)

add_custom_target(
  run-bench
  COMMAND ${KLIB_BENCH_EXECUTABLE} --benchmark-no-analysis
  DEPENDS ${KLIB_BENCH_EXECUTABLE}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Run bench")
