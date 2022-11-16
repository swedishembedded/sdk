# SPDX-License-Identifier: Apache-2.0
# Copyright 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

*** Settings ***
Library  OperatingSystem
Library  Process
Library  directory-structure.py
Resource  ${CURDIR}/DirectoryStructure.robot

*** Variables ***
${ROOT_DIR}  ${CURDIR}/../../

*** Test Cases ***

Apps have proper directory structure
	Directory has apps structure ${ROOT_DIR}/apps
Samples have proper directory structure
	Directory has samples structure ${ROOT_DIR}/samples
Boards have proper directory structure
	Directory has boards structure ${ROOT_DIR}/boards
CMake have proper directory structure
	Directory has cmake structure ${ROOT_DIR}/cmake
Drivers have proper directory structure
	Directory has drivers structure ${ROOT_DIR}/drivers
Lib have proper directory structure
	Directory has lib structure ${ROOT_DIR}/lib
Scripts have proper directory structure
	Directory has scripts structure ${ROOT_DIR}/scripts

