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

Example sensor driver is ready
	Set Test Variable  ${DRIVER_TYPE}  example
	Set Test Variable  ${DRIVER_NAME}  example_driver
	Driver has proper format
	Driver has private function init
