file(GLOB KLIB_BENCH_FILE CONFIGURE_DEPENDS "${KLIB_SOURCE_DIR}/data/*")
file(COPY ${KLIB_BENCH_FILE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB_RECURSE KLIB_BENCH_SRC CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

find_package(Catch2 REQUIRED)
find_package(re2 REQUIRED)
find_package(TBB REQUIRED)
find_package(simdjson REQUIRED)
find_package(Boost REQUIRED COMPONENTS json CONFIG)
add_definitions(-DCATCH_CONFIG_ENABLE_BENCHMARKING)

find_package(PkgConfig REQUIRED)
pkg_check_modules(icu-i18n REQUIRED IMPORTED_TARGET icu-i18n)
add_definitions(-DU_HIDE_INTERNAL_API -DU_HIDE_DRAFT_API -DU_HIDE_OBSOLETE_API
                -DU_HIDE_DEPRECATED_API)

add_executable(${KLIB_BENCH_EXECUTABLE} ${MIMALLOC_OBJECT} ${KLIB_BENCH_SRC})
target_link_libraries(
  ${KLIB_BENCH_EXECUTABLE}
  PRIVATE ${KLIB_LIBRARY}
          Catch2::Catch2WithMain
          TBB::tbb
          simdjson::simdjson
          ${Boost_LIBRARIES}
          PkgConfig::icu-i18n
          re2::re2)

add_custom_target(
  run-bench
  COMMAND ${KLIB_BENCH_EXECUTABLE} --benchmark-no-analysis --benchmark-samples=3
          --use-colour=no -o ${KLIB_SOURCE_DIR}/bench/result.md
  DEPENDS ${KLIB_BENCH_EXECUTABLE}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Run bench")
