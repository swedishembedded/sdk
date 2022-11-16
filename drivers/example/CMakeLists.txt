# SPDX-License-Identifier: Apache 2.0
# Copyright 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

zephyr_library()
zephyr_library_property(ALLOW_EMPTY TRUE)
zephyr_library_sources_ifdef(CONFIG_EXAMPLE_DRIVER example_driver.c)
zephyr_library_sources_ifdef(CONFIG_EXAMPLE_SENSOR example_sensor.c)

check_clang_tidy(..__sdk__drivers__example)
