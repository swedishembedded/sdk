# Copyright (c) 2021 Antmicro <www.antmicro.com>
# SPDX-License-Identifier: Apache-2.0
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(shell)

target_sources(app PRIVATE src/main.c)
