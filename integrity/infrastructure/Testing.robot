# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

*** Settings ***
Library  OperatingSystem
Library  testing.py

*** Variables ***
${ROOT_DIR}  ${CURDIR}/../../
${TESTS_DIR}  ${ROOT_DIR}/tests/

*** Keywords ***

Source file should have integration test ${SOURCE}
	# make sure there is a test for each driver
	${PATH}  ${FILE} =  Split Path  ${SOURCE}
	${NAME}  ${EXT} =  Split Extension  ${FILE}

	Directory Should Exist  ${TESTS_DIR}/${NAME}/

	# Integration tests currently include the file
	${STR}    Get File    ${TESTS_DIR}/${NAME}/CMakeLists.txt
	Should Contain    ${STR}    ${NAME}.c

	# Check that name follows a good pattern
	${STR}    Get File    ${TESTS_DIR}/${NAME}/testcase.yaml
	Should Contain    ${STR}    ${NAME}.integration
	# Quick check for tag (could be done by parsing python)
	Should Contain    ${STR}    ${NAME}.c

Source file should have unit test ${SOURCE}
	# make sure there is a test for each driver
	${PATH}  ${FILE} =  Split Path  ${SOURCE}
	${NAME}  ${EXT} =  Split Extension  ${FILE}

	Directory Should Exist  ${TESTS_DIR}/${NAME}/

	# The main C file must be in the cmake file
	${STR}    Get File    ${TESTS_DIR}/${NAME}/src/unit.c
	Should Contain    ${STR}    ${NAME}.c

	# Check that name follows a good pattern
	${STR}    Get File    ${TESTS_DIR}/${NAME}/testcase.yaml
	Should Contain    ${STR}    ${NAME}.unit
	# Quick check for tag (could be done by parsing python)
	Should Contain    ${STR}    ${NAME}.c

Sources under ${dir} have unit tests under ${testbase}
	Set Test Variable  ${TESTS_DIR}  ${testbase}

	@{SOURCES} =	List Files In Directory	${dir}/  *.c
	FOR  ${SOURCE}  IN  @{SOURCES}
		Source file should have unit test ${SOURCE}
	END

Sources under ${dir} have integration tests under ${testbase}
	Set Test Variable  ${TESTS_DIR}  ${testbase}

	@{SOURCES} =	List Files In Directory	${dir}/  *.c
	FOR  ${SOURCE}  IN  @{SOURCES}
		Source file should have integration test ${SOURCE}
	END
