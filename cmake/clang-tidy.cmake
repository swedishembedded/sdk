# SPDX-License-Identifier: Apache-2.0
# Copyright 2022 Martin Schröder <mkschreder.uk@gmail.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

find_program(
  CLANG_TIDY
  NAMES "clang-tidy-12" "clang-tidy"
  DOC "Path to clang-tidy executable")

function(check_clang_tidy TARGET)
  # Only check header files from the same current source dir
  get_filename_component(SOURCE_DIR_NAME "${CMAKE_SOURCE_DIR}" NAME)
  set(DO_CLANG_TIDY
      "${CLANG_TIDY}"
      "-extra-arg=-ferror-limit=3"
      "-extra-arg=-Wno-clang-diagnostic-error"
      "-extra-arg=-Wno-unknown-warning-option"
      "-p=${CMAKE_BUILD_DIR}/"
      "--header-filter=${SOURCE_DIR_NAME}/.*")

  set_target_properties(${TARGET} PROPERTIES C_CLANG_TIDY "${DO_CLANG_TIDY}")
  set_target_properties(${TARGET} PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
endfunction()
