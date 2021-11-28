cmake_minimum_required(VERSION 3.22)

project(dummy_import LANGUAGES CXX)

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

add_executable(without_namespace_target main.cpp)
target_link_libraries(without_namespace_target PRIVATE klib)
target_compile_features(without_namespace_target PRIVATE cxx_std_20)
add_custom_command(
  TARGET without_namespace_target
  POST_BUILD
  COMMAND without_namespace_target)
