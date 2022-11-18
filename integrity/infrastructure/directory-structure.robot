# SPDX-License-Identifier: Apache-2.0
# Copyright 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

*** Settings ***
Library  OperatingSystem
Library  Process
Library  directory-structure.py
Resource  ${CURDIR}/DirectoryStructure.robot
Resource  ${CURDIR}/Testing.robot

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
	Set Test Variable  ${driverbase}  ${ROOT_DIR}/drivers
	Directory has drivers structure ${driverbase}
    # Additional project specific checks
    @{DRIVER_TYPES} =    List Directories In Directory    ${driverbase}
    FOR    ${DRIVER_TYPE}    IN    @{DRIVER_TYPES}
        @{DRIVERS} =    List Files In Directory    ${driverbase}/${DRIVER_TYPE}/    *.c
        FOR    ${DRIVER}    IN    @{DRIVERS}
            ${PATH}    ${FILE} =    Split Path    ${DRIVER}
            ${NAME}    ${EXT} =    Split Extension    ${FILE}

			Set Test Variable  ${TESTS_DIR}  ${ROOT_DIR}/tests/drivers/${DRIVER_TYPE}/

			# Drivers are required to have unit and integration tests
			Source file should have unit test ${driverbase}/${DRIVER_TYPE}/${DRIVER}
			Source file should have integration test ${driverbase}/${DRIVER_TYPE}/${DRIVER}

			# make sure there is integrity check for driver
			File Should Exist  ${INTEGRITY_DIR}/drivers/${DRIVER_TYPE}/${NAME}.robot

			# All drivers must have a sample
			Set Test Variable  ${APP_SOURCE}  ${ROOT_DIR}/samples/drivers/${DRIVER_TYPE}/${NAME}
			Directory has app files ${APP_SOURCE}

			# Each sample should have a robot test to verify it's content
			File Should Exist  ${CURDIR}/../samples/drivers/${DRIVER_TYPE}/${NAME}.robot
        END
    END

Lib have proper directory structure
	Directory has lib structure ${ROOT_DIR}/lib
Scripts have proper directory structure
	Directory has scripts structure ${ROOT_DIR}/scripts

