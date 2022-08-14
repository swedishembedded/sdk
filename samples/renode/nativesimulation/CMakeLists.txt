# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sample)

target_sources(app PRIVATE src/main.c)

add_custom_target(
  peripheral
  COMMAND
    gcc -I${PROJECT_BASE}/include/
    -I${PROJECT_BASE}/../renode/src/Infrastructure/src/Emulator/Cores/renode/include/
    -fPIC -shared -o plugin/peripheral.so plugin/peripheral.c
  WORKING_DIRECTORY ${APPLICATION_SOURCE_DIR})

add_dependencies(app peripheral)
