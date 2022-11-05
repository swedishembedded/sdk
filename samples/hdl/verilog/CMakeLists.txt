# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sample)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/yosys.cfg.in
               ${CMAKE_CURRENT_BINARY_DIR}/yosys.cfg)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/formal.sby.in
               ${CMAKE_CURRENT_BINARY_DIR}/formal.sby)
execute_process(
  COMMAND yosys-config --datdir
  OUTPUT_STRIP_TRAILING_WHITESPACE
  OUTPUT_VARIABLE YOSYS_DATA_DIR)
message(${YOSYS_DATA_DIR}/include)
zephyr_include_directories(${YOSYS_DATA_DIR}/include)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/zephyr/include/generated/plc.hpp
  COMMAND yosys -s ${CMAKE_CURRENT_BINARY_DIR}/yosys.cfg)

add_custom_target(formal COMMAND sby -f ${CMAKE_CURRENT_BINARY_DIR}/formal.sby)

# zephyr_include_directories(${CMAKE_CURRENT_BINARY_DIR}/zephyr/include)

target_sources(
  app PRIVATE src/main.cpp
              ${CMAKE_CURRENT_BINARY_DIR}/zephyr/include/generated/plc.hpp)

add_dependencies(app formal)
