# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

*** Settings ***
Library  ${CURDIR}/../../../infrastructure/directory-structure.py

*** Variables ***
${APP}  mcp23s17
${APP_NAME}  samples/drivers/gpio/${APP}

*** Test Cases ***

Application should have correct structure
	App title should be  MCP23S17
	App should have release build
	App check integration platform  native_posix
