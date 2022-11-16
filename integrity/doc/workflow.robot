# SPDX-License-Identifier: Apache-2.0
# Copyright 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

*** Settings ***
Library  OperatingSystem

Library  ${CURDIR}/../DocChecker.py
Resource  ${CURDIR}/guide.robot
Resource  ${CURDIR}/reference.robot
Resource  ${CURDIR}/board.robot

*** Variables ***
${ROOT_DIR}  ${CURDIR}/../../
${WORKFLOW}  ${ROOT_DIR}/doc/workflow/

*** Test Cases ***

Workflow has been documented
	Setting up a new project
	Building your code using docker
	Flashing on real hardware
	Testing your application
	Debugging your application
	Simulating your application
	Getting help
	Further reading

*** Keywords ***

Setting up a new project
	Check Page Contains Text
		...  ${WORKFLOW}
		...  ^Setting up a new project

Building your code using docker
	Check Page Contains Text
		...  ${WORKFLOW}
		...  ^Building your code using docker

Testing your application
	Check Page Contains Text
		...  ${WORKFLOW}
		...  ^Testing your application

Flashing on real hardware
	Check Page Contains Text
		...  ${WORKFLOW}
		...  ^Flashing on real hardware

Debugging your application
	Check Page Contains Text
		...  ${WORKFLOW}
		...  ^Debugging your application

Simulating your application
	Check Page Contains Text
		...  ${WORKFLOW}
		...  ^Simulating your application

Getting help
	Check Page Contains Text
		...  ${WORKFLOW}
		...  Getting help

Further reading
	Check Page Contains Text
		...  ${WORKFLOW}
		...  Further reading
