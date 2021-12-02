cmake_minimum_required(VERSION 3.22)

project(dummy_import LANGUAGES CXX)

# https://stackoverflow.com/questions/7980784/cmake-add-sub-directory-which-is-not-sub-directory-on-real-directory
add_subdirectory(${KLIB_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/klib)

add_executable(with_namespace_target main.cpp)
target_link_libraries(with_namespace_target PRIVATE klib::klib)
add_custom_command(
  TARGET with_namespace_target
  POST_BUILD
  COMMAND with_namespace_target)
