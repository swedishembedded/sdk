# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sample)

target_sources(app PRIVATE src/main.c)

add_custom_target(
  peripheral
  COMMAND
    g++ `sdl2-config --cflags` -I${PROJECT_BASE}/include/
    -I${PROJECT_BASE}/../control/include -I${PROJECT_BASE}/../imgui/
    -I${PROJECT_BASE}/../imgui/backends/ -Wall -Wextra -Werror -o
    peripheral/peripheral.elf peripheral/main.cpp
    ${PROJECT_BASE}/../imgui/imgui*.cpp
    ${PROJECT_BASE}/../imgui/backends/imgui_impl_sdl.cpp
    ${PROJECT_BASE}/../imgui/backends/imgui_impl_opengl3.cpp `sdl2-config
    --libs` -lGL -ldl
  WORKING_DIRECTORY ${APPLICATION_SOURCE_DIR})

add_dependencies(app peripheral)
