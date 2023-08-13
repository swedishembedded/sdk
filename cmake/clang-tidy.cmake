# SPDX-License-Identifier: Apache-2.0
# ~~~
# Copyright 2022 Martin Schröder <mkschreder.uk@gmail.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training
# ~~~

find_program(
  CLANG_TIDY
  NAMES "clang-tidy-15" "clang-tidy-12" "clang-tidy"
  DOC "Path to clang-tidy executable")

# Enable clang-tidy for target
function(check_clang_tidy target)
  # Add a custom script that will run after the target was built
  add_custom_command(
    TARGET ${target}
    POST_BUILD
    COMMAND $ENV{PROJECT_BASE}/scripts/run-clang-tidy --build-dir
            ${CMAKE_BINARY_DIR}
    COMMENT "Running static analysis")
endfunction()
