# SPDX-License-Identifier: MIT
# Copyright (c) 2018 Andrew Gaspar
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

function(cargo_build)
  cmake_parse_arguments(CARGO "" "NAME" "" ${ARGN})
  string(REPLACE "-" "_" LIB_NAME ${CARGO_NAME})

  set(CARGO_TARGET_DIR ${CMAKE_CURRENT_BINARY_DIR})

  message("System name: " ${CMAKE_SYSTEM_NAME})
  # Use Kconfig variables to determine the architecture
  if(CONFIG_ARCH STREQUAL "arm")
    if(CONFIG_CPU_CORTEX_M3 STREQUAL "y")
      set(LIB_TARGET "thumbv7m-none-eabi")
    endif()
  else()
    # Note: this is just a default. You should add other architectures above.
    set(LIB_TARGET "x86_64-unknown-none")
  endif()

  if(NOT CMAKE_BUILD_TYPE)
    set(LIB_BUILD_TYPE "debug")
  elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(LIB_BUILD_TYPE "release")
  else()
    set(LIB_BUILD_TYPE "debug")
  endif()

  set(LIB_FILE
      "${CARGO_TARGET_DIR}/${LIB_TARGET}/${LIB_BUILD_TYPE}/${CMAKE_STATIC_LIBRARY_PREFIX}${LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}"
  )

  set(CARGO_ARGS "build")
  list(APPEND CARGO_ARGS "--target" ${LIB_TARGET})

  if(${LIB_BUILD_TYPE} STREQUAL "release")
    list(APPEND CARGO_ARGS "--release")
  endif()

  file(GLOB_RECURSE LIB_SOURCES "*.rs")

  set(CARGO_ENV_COMMAND ${CMAKE_COMMAND} -E env
                        "CARGO_TARGET_DIR=${CARGO_TARGET_DIR}")

  add_custom_command(
    OUTPUT ${LIB_FILE}
    COMMAND ${CARGO_ENV_COMMAND} ${CARGO_EXECUTABLE} ARGS ${CARGO_ARGS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${LIB_SOURCES}
    COMMENT "running cargo")
  add_custom_target(${CARGO_NAME}_target ALL DEPENDS ${LIB_FILE})
  add_library(${CARGO_NAME} STATIC IMPORTED GLOBAL)
  add_dependencies(${CARGO_NAME} ${CARGO_NAME}_target)
  set_target_properties(${CARGO_NAME} PROPERTIES IMPORTED_LOCATION ${LIB_FILE})
endfunction()
