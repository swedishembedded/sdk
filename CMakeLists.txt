# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

# store project base in current scope
set(PROJECT_BASE ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "SDK base directory")
# put it also in the env scope
set(ENV{PROJECT_BASE} ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(cmake)
add_subdirectory(drivers)
add_subdirectory(lib)
add_subdirectory(testbench)

# Add the main include directory to list of include paths
zephyr_include_directories(include)

# Add sensible warnings to all files built in smart tracker
# treat all warnings as errors
zephyr_compile_options("-Werror")
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

# Disable old style declaration
# Reason: zephyr pinctrl fails to compile
#zephyr_compile_options("-Wno-old-style-declaration")

# Disable unused parameter warning
# Reason: zephyr pm functions fail to compile
zephyr_compile_options("-Wno-unused-parameter")

# Disable error on type limits (zephyr ASSERT)
# Reason: zephyr sys/assert does not compile
zephyr_compile_options("-Wno-type-limits")

# Disable error on comparison of integers of different signedness (zephyr)
# Reason: zephyr os/cbprintf does not compile
zephyr_compile_options("-Wno-sign-compare")

# Disable qualifiers (zephyr net_if.h)
zephyr_compile_options("-Wno-ignored-qualifiers")

# Disable error on missing field initializers
# Reason: sync_status in drivers/spi/spi_context.h
zephyr_compile_options("-Wno-missing-field-initializers")

# Disable error on checking non-literal format (zephyr)
# Reason: zephyr drivers/serial/uart_native_posix.c does not compile
zephyr_compile_options("-Wno-format-nonliteral")

# warn for any constants defined as 2.2 instead of 2.2f (as doubles)
# Unfortunately has to be disabled due to zephyr not compiling with it
zephyr_compile_options("-Wno-double-promotion")
