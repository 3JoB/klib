find_package(gsl-lite REQUIRED)
find_package(CLI11 REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  add_definitions(-Dgsl_CONFIG_CONTRACT_VIOLATION_THROWS)
endif()

file(GLOB_RECURSE KLIB_EXECUTABLE_SRC CONFIGURE_DEPENDS
     "${KLIB_SOURCE_DIR}/tool/*.cpp")

add_executable(${KLIB_EXECUTABLE} ${MIMALLOC_OBJECT} ${KLIB_EXECUTABLE_SRC})
target_link_libraries(
  ${KLIB_EXECUTABLE} PRIVATE ${KLIB_LIBRARY} gsl::gsl-lite-v1 CLI11::CLI11
                             fmt::fmt spdlog::spdlog)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  find_package(Backward REQUIRED)
  target_link_libraries(${KLIB_EXECUTABLE} PRIVATE Backward::Backward)
endif()
