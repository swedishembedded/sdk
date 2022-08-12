# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Swedish Embedded Group AB

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(example_sensor)

target_sources(app PRIVATE src/main.c)
