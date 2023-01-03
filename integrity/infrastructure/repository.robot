# SPDX-License-Identifier: Apache-2.0
# Copyright 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

*** Settings ***
Library  OperatingSystem
Library  Process
Variables  ${CURDIR}/../variables.py

*** Test Cases ***

Git repository has been setup
	Zephyr repository has a precommit hook
	Zephyr repository has an init script that sets up the workspace

*** Keywords ***

Zephyr repository has a precommit hook
	File Should Exist  ${PROJECT_ROOT}/.githooks/pre-commit

Zephyr repository has an init script that sets up the workspace
	File Should Exist  ${PROJECT_ROOT}/scripts/init
