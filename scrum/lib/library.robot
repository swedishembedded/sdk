# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

*** Settings ***
Variables  ${CURDIR}/../variables.py
Library  OperatingSystem
Library  library.py

*** Keywords ***

Has private function ${FUNC}
	Module Has Static Function  ${FUNC}

Has public function ${FUNC}
	Module Has Public Function  ${FUNC}

Implements object interface
	Has public function new
	Has public function free
