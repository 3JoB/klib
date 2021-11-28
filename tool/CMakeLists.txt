find_package(CLI11 REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)

file(GLOB_RECURSE KLIB_EXECUTABLE_SRC CONFIGURE_DEPENDS
     "${KLIB_SOURCE_DIR}/tool/*.cpp")

add_executable(${KLIB_EXECUTABLE} ${KLIB_EXECUTABLE_SRC})
target_link_libraries(${KLIB_EXECUTABLE} PRIVATE ${KLIB_LIBRARY} CLI11::CLI11
                                                 fmt::fmt spdlog::spdlog)
