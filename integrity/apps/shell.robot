# SPDX-License-Identifier: Apache-2.0
# Copyright 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

*** Settings ***
Library  ${CURDIR}/../infrastructure/directory-structure.py

*** Variables ***
${APP}  shell
${APP_NAME}  apps/${APP}

*** Test Cases ***

Application should have correct structure
	App title should be  Example shell application
	App should have release build
	App check integration platform  native_posix
