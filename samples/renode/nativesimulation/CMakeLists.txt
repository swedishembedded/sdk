# SPDX-License-Identifier: Apache-2.0 Copyright (c) 2022 Martin Schröder
# <info@swedishembedded.com> Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sample)

target_sources(app PRIVATE src/main.c)

set(_renode_infrastructure ${PROJECT_BASE}/../renode/src/Infrastructure)

add_custom_target(
  peripheral
  COMMAND
    gcc -I${PROJECT_BASE}/include/
    -I${_renode_infrastructure}/src/Emulator/Cores/renode/include/ -fPIC -shared
    -o plugin/peripheral.so plugin/peripheral.c
    ${PROJECT_BASE}/lib/renode/renode.c
  COMMENT "Building peripheral device"
  WORKING_DIRECTORY ${APPLICATION_SOURCE_DIR})

add_dependencies(app peripheral)
