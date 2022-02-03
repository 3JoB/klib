include_directories(include)
file(GLOB_RECURSE FASTBASE64_SRC CONFIGURE_DEPENDS "src/*.c")
add_library(fastbase64 STATIC ${FASTBASE64_SRC})
target_compile_options(fastbase64 PRIVATE "-mavx2")
