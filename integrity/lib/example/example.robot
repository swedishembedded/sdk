# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

*** Settings ***
Variables  ${CURDIR}/../../variables.py
Library  OperatingSystem
Resource  ${INTEGRITY_DIR}/lib/library.robot

*** Variables ***

*** Test Cases ***

Example sensor driver is ready
	Set Test Variable  ${LIBRARY}  example
	Set Test Variable  ${MODULE}  example
	Has public function object_init
