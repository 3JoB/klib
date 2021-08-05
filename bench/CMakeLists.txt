file(COPY ${KLIB_SOURCE_DIR}/data/zlib-v1.2.11.tar.gz
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB_RECURSE BENCHMARK_SRC CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

find_package(Catch2 REQUIRED)
add_definitions(-DCATCH_CONFIG_ENABLE_BENCHMARKING)

add_executable(${BENCH_EXECUTABLE} ${BENCHMARK_SRC})
target_link_libraries(${BENCH_EXECUTABLE} PRIVATE ${LIBRARY}
                                                  Catch2::Catch2WithMain)

add_custom_target(
  bench_decompress
  COMMAND ${CMAKE_COMMAND} -E tar zxf zlib-v1.2.11.tar.gz
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Unpacking zlib-v1.2.11.tar.gz")
add_dependencies(${BENCH_EXECUTABLE} bench_decompress)

add_custom_target(
  run-bench
  COMMAND ${BENCH_EXECUTABLE} --benchmark-no-analysis
  DEPENDS ${BENCH_EXECUTABLE}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Run bench")
