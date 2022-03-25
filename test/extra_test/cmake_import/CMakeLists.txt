cmake_minimum_required(VERSION 3.18)

project(dummy_import LANGUAGES CXX)

find_package(klib REQUIRED)

add_executable(with_namespace_target main.cpp)
target_link_libraries(with_namespace_target PRIVATE klib::klib)
add_custom_command(
  TARGET with_namespace_target
  POST_BUILD
  COMMAND with_namespace_target)
