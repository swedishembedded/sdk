# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

*** Settings ***
Variables  ${CURDIR}/../../variables.py
Library  OperatingSystem
Resource  ${ROOT_DIR}/scrum/lib/library.robot

*** Variables ***

*** Test Cases ***

Renode interface is implemented
	Set Test Variable  ${LIBRARY}  renode
	Set Test Variable  ${MODULE}  renode
	Implements object interface
	Has public function connect
	Has public function disconnect
	Has public function wait_request
	Has public function send_response

*** Keywords ***
