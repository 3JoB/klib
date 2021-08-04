file(GLOB_RECURSE BENCHMARK_SRC CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

find_package(Catch2 REQUIRED)
add_definitions(-DCATCH_CONFIG_ENABLE_BENCHMARKING)

add_executable(${BENCHMARK_EXECUTABLE} ${BENCHMARK_SRC})
target_link_libraries(${BENCHMARK_EXECUTABLE} PRIVATE ${LIBRARY}
                                                      Catch2::Catch2WithMain)

add_custom_target(
  run-bench
  COMMAND ${BENCHMARK_EXECUTABLE} --benchmark-no-analysis
  DEPENDS ${BENCHMARK_EXECUTABLE}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Run bench")
