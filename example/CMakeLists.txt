cmake_minimum_required(VERSION 3.20)

project(examples LANGUAGES CXX)

find_package(klib REQUIRED)

add_executable(with_namespace_target main.cpp)
target_link_libraries(with_namespace_target PRIVATE klib::klib)
add_custom_command(
  TARGET with_namespace_target
  POST_BUILD
  COMMAND with_namespace_target)

add_executable(with_namespace_target_shared main.cpp)
target_link_libraries(with_namespace_target_shared PRIVATE klib::klib-shared)
add_custom_command(
  TARGET with_namespace_target_shared
  POST_BUILD
  COMMAND with_namespace_target_shared)
