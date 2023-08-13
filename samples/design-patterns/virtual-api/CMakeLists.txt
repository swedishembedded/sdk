# SPDX-License-Identifier: Apache-2.0 Copyright (c) 2022 Martin Schröder
# <info@swedishembedded.com> Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sample)

add_subdirectory(rust)

target_sources(app PRIVATE src/main.c src/serial.c src/serial_impl.c)
target_link_libraries(app PRIVATE rust_lib)
