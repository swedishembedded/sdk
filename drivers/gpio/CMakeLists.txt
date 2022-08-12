# SPDX-License-Identifier: Apache 2.0
# Copyright 2022 Martin Schröder <info@swedishembedded.com>

zephyr_library()

zephyr_library_sources_ifdef(CONFIG_GPIO_MCP23S17V2 mcp23s17.c)
