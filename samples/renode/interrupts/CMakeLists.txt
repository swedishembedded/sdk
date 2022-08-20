# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sample)

target_sources(app PRIVATE src/main.c)

# TODO: this needs to be made generic
set(LIB_CONTROL_ROOT ${PROJECT_BASE}/../modules/lib/control/)
set(LIB_IMGUI_ROOT ${PROJECT_BASE}/../modules/lib/imgui/)
set(LIB_IMPLOT_ROOT ${PROJECT_BASE}/../modules/lib/implot/)

add_custom_target(
  peripheral
  COMMAND
    g++ `sdl2-config --cflags` -I${PROJECT_BASE}/include/
    -I${LIB_CONTROL_ROOT}/include -I${LIB_IMGUI_ROOT} -I${LIB_IMPLOT_ROOT}
    -I${APPLICATION_BINARY_DIR}/ -I${LIB_IMGUI_ROOT}/backends/ -Wall -Wextra
    -Werror -o peripheral/peripheral.elf peripheral/main.cpp
    ${PROJECT_BASE}/lib/renode/renode.c ${LIB_IMGUI_ROOT}/imgui*.cpp
    ${LIB_IMGUI_ROOT}/backends/imgui_impl_sdl.cpp ${LIB_IMPLOT_ROOT}/implot*.cpp
    ${LIB_IMGUI_ROOT}/backends/imgui_impl_opengl3.cpp `sdl2-config --libs` -lGL
    -ldl -lpthread
  WORKING_DIRECTORY ${APPLICATION_SOURCE_DIR})

add_dependencies(app peripheral)
