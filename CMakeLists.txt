# SPDX-License-Identifier: Apache-2.0 Copyright (c) 2022 Martin Schröder
# <info@swedishembedded.com> Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

# store project base in current scope
set(PROJECT_BASE ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# put it also in the env scope
set(ENV{PROJECT_BASE} ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      "${CMAKE_CURRENT_LIST_DIR}/cmake/rust/")
enable_language(Rust)

add_subdirectory(cmake)
add_subdirectory(drivers)
add_subdirectory(lib)
add_subdirectory(testbench)

# Add the main include directory to list of include paths
zephyr_include_directories(include)

# enable extra warnings
zephyr_compile_options("-Wextra")
# enable standard set of warnings (doesn't matter if already enabled)
zephyr_compile_options("-Wall")
# format options validation
zephyr_compile_options("-Wformat=2")
# more format options validation
zephyr_compile_options("-Wformat-truncation")
# detects some out of bounds array indices
zephyr_compile_options("-Warray-bounds")
# detects uninitialized variables
zephyr_compile_options("-Wuninitialized")
# detects passing null arguments into functions that do not check for it
zephyr_compile_options("-Wnonnull")

# Disable old style declaration Reason: zephyr pinctrl fails to compile
# zephyr_compile_options("-Wno-old-style-declaration")

# Warn for unused parameters
zephyr_compile_options("-Wno-unused-parameter")

# Warn for type limits when working with ints
zephyr_compile_options("-Wno-type-limits")

# Warn signed/unsigned compare
zephyr_compile_options("-Wno-sign-compare")

# Warn about ignored qualifiers (such as const)
zephyr_compile_options("-Wignored-qualifiers")

# Warn about missing field initializers
zephyr_compile_options("-Wmissing-field-initializers")

# Warn about nonliteral format
zephyr_compile_options("-Wno-format-nonliteral")

# Warn about double promotion
zephyr_compile_options("-Wno-double-promotion")
