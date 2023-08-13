# SPDX-License-Identifier: MIT
# ~~~
# Copyright (c) 2018 Andrew Gaspar
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training
# ~~~

# Build code using cargo
function(cargo_build)
  cmake_parse_arguments(CARGO "" "NAME" "" ${ARGN})
  string(REPLACE "-" "_" LIB_NAME ${CARGO_NAME})

  set(cargo_target_dir ${CMAKE_CURRENT_BINARY_DIR})

  message("System name: " ${CMAKE_SYSTEM_NAME})
  # Use Kconfig variables to determine the architecture
  if(CONFIG_ARCH STREQUAL "arm")
    if(CONFIG_CPU_CORTEX_M3 STREQUAL "y")
      set(lib_target "thumbv7m-none-eabi")
    endif()
  elseif(CONFIG_BOARD_QEMU_X86_64 STREQUAL "y")
    if(CONFIG_CPU_ATOM STREQUAL "y")
      set(lib_target "x86_64-unknown-none")
    endif()
  elseif(CONFIG_BOARD_NATIVE_POSIX STREQUAL "y")
    if(CONFIG_BOARD_NATIVE_POSIX_32BIT STREQUAL "y")
      set(lib_target "i686-unknown-linux-gnu")
    else()
      set(lib_target "x86_64-unknown-linux-gnu")
    endif()
  else()
    # Note: this is just a default. You should add other architectures above.
    set(lib_target "x86_64-unknown-none")
  endif()

  if(NOT CMAKE_BUILD_TYPE)
    set(lib_build_type "debug")
  elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(lib_build_type "release")
  else()
    set(lib_build_type "debug")
  endif()

  set(lib_file
      "${cargo_target_dir}/${lib_target}/${lib_build_type}/"
      "${CMAKE_STATIC_LIBRARY_PREFIX}${LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}")

  set(cargo_args "build")
  list(APPEND cargo_args "--target" ${lib_target})

  if(${lib_build_type} STREQUAL "release")
    list(APPEND cargo_args "--release")
  endif()

  file(GLOB_RECURSE LIB_SOURCES "*.rs")

  set(cargo_env_command ${CMAKE_COMMAND} -E env
                        "cargo_target_dir=${cargo_target_dir}")

  add_custom_command(
    OUTPUT ${lib_file}
    COMMAND ${cargo_env_command} ${CARGO_EXECUTABLE} ARGS ${cargo_args}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${LIB_SOURCES}
    COMMENT "running cargo")
  add_custom_target(
    ${CARGO_NAME}_target ALL
    COMMENT "Building cargo target"
    DEPENDS ${lib_file})
  add_library(${CARGO_NAME} STATIC IMPORTED GLOBAL)
  add_dependencies(${CARGO_NAME} ${CARGO_NAME}_target)
  set_target_properties(${CARGO_NAME} PROPERTIES IMPORTED_LOCATION ${lib_file})
endfunction()
