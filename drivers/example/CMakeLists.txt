# SPDX-License-Identifier: Apache 2.0
# Copyright 2022 Martin Schröder <info@swedishembedded.com>

zephyr_library()

zephyr_library_sources_ifdef(CONFIG_EXAMPLE_DRIVER example_driver.c)
zephyr_library_sources_ifdef(CONFIG_EXAMPLE_SENSOR example_sensor.c)
