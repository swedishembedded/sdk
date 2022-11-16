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
	Set Test Variable  ${DRIVER_NAME}  example_sensor
	Driver has proper format
	Driver has simulation
	Driver has private function init
	Driver has private function sample_fetch
	Driver has private function channel_get
