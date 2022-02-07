if(KLIB_TEST_USE_PROXY)
  add_definitions(-DKLIB_TEST_USE_PROXY)
endif()

file(GLOB KLIB_TEST_FILE CONFIGURE_DEPENDS "${KLIB_SOURCE_DIR}/data/*")
file(COPY ${KLIB_TEST_FILE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB_RECURSE KLIB_TEST_SRC CONFIGURE_DEPENDS
     ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

find_package(Catch2 REQUIRED)
find_package(Boost CONFIG REQUIRED COMPONENTS json)
add_definitions(-DDBG_MACRO_NO_WARNING)
if(NOT (${CMAKE_BUILD_TYPE} STREQUAL "Debug"))
  add_definitions(-DDBG_MACRO_DISABLE)
endif()

add_executable(${KLIB_TEST_EXECUTABLE} ${MIMALLOC_OBJECT} ${KLIB_TEST_SRC})
target_link_libraries(
  ${KLIB_TEST_EXECUTABLE} PRIVATE ${KLIB_LIBRARY} Catch2::Catch2WithMain
                                  ${Boost_LIBRARIES})

include(Catch)
catch_discover_tests(${KLIB_TEST_EXECUTABLE} REPORTER compact)
