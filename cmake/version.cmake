# SPDX-License-Identifier: Apache-2.0
# ~~~
# Copyright Nordic Semiconductor
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training
#
# .rst:
# version.cmake
# -------------
#
# Inputs:
#
#   ``*VERSION*`` and other constants set by
#   maintainers in ``${PROJECT_BASE}/VERSION``
#
# Outputs with examples::
#
#   PROJECT_VERSION           1.14.99
#   PROJECT_VERSION_STRING    "1.14.99"
#   PROJECT_VERSION_MAJOR       1
#   PROJECT_VERSION_MINOR        14
#   PROJECT_VERSION_PATCH             99
#   PROJECT_VERSION_NUMBER    0x10E63
#
# Most outputs are converted to C macros, see ``version.h.in``
#
# See also: independent and more dynamic ``PROJECT_BUILD_VERSION`` in
# ``git.cmake``.
# ~~~

include(${ZEPHYR_BASE}/cmake/hex.cmake)

if(NOT DEFINED PROJECT_BUILD_VERSION)
  find_package(Git QUIET)
  if(GIT_FOUND)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} describe --abbrev=12 --always
      WORKING_DIRECTORY ${PROJECT_BASE}
      OUTPUT_VARIABLE PROJECT_BUILD_VERSION
      OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_STRIP_TRAILING_WHITESPACE
      ERROR_VARIABLE stderr
      RESULT_VARIABLE return_code)
    if(return_code)
      message(STATUS "git describe failed: ${stderr}")
    elseif(NOT "${stderr}" STREQUAL "")
      message(STATUS "git describe warned: ${stderr}")
    endif()
  endif()
endif()

file(READ ${PROJECT_BASE}/VERSION ver)

string(REGEX MATCH "VERSION_MAJOR=([0-9]*)" _ ${ver})
set(PROJECT_VERSION_MAJOR ${CMAKE_MATCH_1})

string(REGEX MATCH "VERSION_MINOR=([0-9]*)" _ ${ver})
set(PROJECT_VERSION_MINOR ${CMAKE_MATCH_1})

string(REGEX MATCH "VERSION_PATCH=([0-9]*)" _ ${ver})
set(PROJECT_VERSION_PATCH ${CMAKE_MATCH_1})

set(PROJECT_VERSION
    ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

if(DEFINED PROJECT_BUILD_VERSION)
  set(PROJECT_BUILD_VERSION_STR ", build: ${PROJECT_BUILD_VERSION}")
endif()

set(MAJOR ${PROJECT_VERSION_MAJOR}) # Temporary convenience variable
set(MINOR ${PROJECT_VERSION_MINOR}) # Temporary convenience variable
set(PATCH ${PROJECT_VERSION_PATCH}) # Temporary convenience variable

math(EXPR PROJECT_VERSION_NUMBER_INT
     "(${MAJOR} << 16) + (${MINOR} << 8)  + (${PATCH})")

to_hex(${PROJECT_VERSION_NUMBER_INT} PROJECT_VERSION_NUMBER)

set(PROJECT_VERSION_STRING "\"${PROJECT_VERSION}\"")

configure_file(${CMAKE_CURRENT_LIST_DIR}/version.h.in
               ${PROJECT_BINARY_DIR}/include/generated/project_version.h)

message(STATUS "PROJECT_VERSION: ${PROJECT_VERSION}")
message(STATUS "PROJECT_VERSION_STRING: ${PROJECT_VERSION_STRING}")
message(STATUS "PROJECT_VERSION_MAJOR: ${PROJECT_VERSION_MAJOR}")
message(STATUS "PROJECT_VERSION_MINOR: ${PROJECT_VERSION_MINOR}")
message(STATUS "PROJECT_VERSION_PATCH: ${PROJECT_VERSION_PATCH}")
message(STATUS "PROJECT_VERSION_NUMBER: ${PROJECT_VERSION_NUMBER}")

# Cleanup convenience variables
unset(MAJOR)
unset(MINOR)
unset(PATCH)
