# https://github.com/google/benchmark/blob/master/cmake/AddCXXCompilerFlag.cmake
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

function(mangle_c_compiler_flag FLAG OUTPUT)
  string(TOUPPER "HAVE_C_FLAG_${FLAG}" SANITIZED_FLAG)
  string(REPLACE "+" "X" SANITIZED_FLAG ${SANITIZED_FLAG})
  string(REGEX REPLACE "[^A-Za-z_0-9]" "_" SANITIZED_FLAG ${SANITIZED_FLAG})
  string(REGEX REPLACE "_+" "_" SANITIZED_FLAG ${SANITIZED_FLAG})

  # https://stackoverflow.com/questions/22487215/return-a-list-from-the-function-using-out-parameter
  set(${OUTPUT}
      ${SANITIZED_FLAG}
      PARENT_SCOPE)
endfunction()

function(mangle_cxx_compiler_flag FLAG OUTPUT)
  string(TOUPPER "HAVE_CXX_FLAG_${FLAG}" SANITIZED_FLAG)
  string(REPLACE "+" "X" SANITIZED_FLAG ${SANITIZED_FLAG})
  string(REGEX REPLACE "[^A-Za-z_0-9]" "_" SANITIZED_FLAG ${SANITIZED_FLAG})
  string(REGEX REPLACE "_+" "_" SANITIZED_FLAG ${SANITIZED_FLAG})

  # https://stackoverflow.com/questions/22487215/return-a-list-from-the-function-using-out-parameter
  set(${OUTPUT}
      ${SANITIZED_FLAG}
      PARENT_SCOPE)
endfunction()

function(add_compiler_flag FLAG)
  mangle_c_compiler_flag(${FLAG} MANGLED_C_FLAG)
  mangle_cxx_compiler_flag(${FLAG} MANGLED_CXX_FLAG)

  # https://cmake.org/cmake/help/latest/module/CheckCXXCompilerFlag.html
  # https://cmake.org/cmake/help/latest/module/CheckCXXSourceCompiles.html
  set(OLD_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
  set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} ${FLAG}")
  check_c_compiler_flag(${FLAG} ${MANGLED_C_FLAG})
  check_cxx_compiler_flag(${FLAG} ${MANGLED_CXX_FLAG})
  set(CMAKE_REQUIRED_FLAGS ${OLD_CMAKE_REQUIRED_FLAGS})

  if(${MANGLED_C_FLAG} AND ${MANGLED_CXX_FLAG})
    set(CMAKE_C_FLAGS
        "${CMAKE_C_FLAGS} ${FLAG}"
        PARENT_SCOPE)
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} ${FLAG}"
        PARENT_SCOPE)
  else()
    message(
      FATAL_ERROR "Required flag '${FLAG}' is not supported by the compiler")
  endif()
endfunction()

function(add_linker_flag FLAG)
  mangle_c_compiler_flag(${FLAG} MANGLED_C_FLAG)
  mangle_cxx_compiler_flag(${FLAG} MANGLED_CXX_FLAG)

  # https://cmake.org/cmake/help/latest/module/CheckCXXCompilerFlag.html
  # https://cmake.org/cmake/help/latest/module/CheckCXXSourceCompiles.html
  set(OLD_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
  set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} ${FLAG}")
  check_c_compiler_flag(${FLAG} ${MANGLED_C_FLAG})
  check_cxx_compiler_flag(${FLAG} ${MANGLED_CXX_FLAG})
  set(CMAKE_REQUIRED_FLAGS ${OLD_CMAKE_REQUIRED_FLAGS})

  if(${MANGLED_C_FLAG} AND ${MANGLED_CXX_FLAG})
    set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} ${FLAG}"
        PARENT_SCOPE)
    set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} ${FLAG}"
        PARENT_SCOPE)
    set(CMAKE_MODULE_LINKER_FLAGS
        "${CMAKE_MODULE_LINKER_FLAGS} ${FLAG}"
        PARENT_SCOPE)
  else()
    message(
      FATAL_ERROR "Required flag '${FLAG}' is not supported by the compiler")
  endif()
endfunction()
