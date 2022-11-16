# SPDX-License-Identifier: Apache-2.0
# Copyright 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

*** Settings ***
Variables  ${CURDIR}/../../variables.py
Library  OperatingSystem
Resource  ${INTEGRITY_DIR}/drivers/driver.robot

*** Variables ***

*** Test Cases ***

Driver is ready
	Set Test Variable  ${DRIVER_TYPE}  gpio
	Set Test Variable  ${DRIVER_NAME}  mcp23s17
	Driver has proper format
	Driver has simulation
	Driver has private function init
	Driver implements GPIO api

*** Keywords ***
