# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(test)

if(CONFIG_CMOCK)
  unity_generate_test_runner(src/unit.c)
  cmock_generate_mocks(${ZEPHYR_BASE}/include/posix/sys/socket.h sys)
  cmock_generate_mocks(${ZEPHYR_BASE}/include/posix/arpa/inet.h arpa)
  target_sources(app PRIVATE src/unit.c)
else()
  target_sources(
    app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../../../lib/renode/renode.c)
  unity_generate_test_runner(src/integration.c)
  target_sources(app PRIVATE src/integration.c)
endif()
