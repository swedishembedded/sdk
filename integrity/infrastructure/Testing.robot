# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

*** Settings ***
Library  OperatingSystem
Library  testing.py

*** Variables ***
${ROOT_DIR}  ${CURDIR}/../../

*** Keywords ***

Sources under ${dir} have unit tests under ${testbase}
	@{SOURCES} =	List Files In Directory	${dir}/  *.c
	FOR  ${SOURCE}  IN  @{SOURCES}
		# make sure there is a test for each driver
		${PATH}  ${FILE} =  Split Path  ${SOURCE}
		${NAME}  ${EXT} =  Split Extension  ${FILE}
		# The test directory should exist
		Directory Should Exist  ${testbase}/${NAME}/
		#${STR}    Get File    ${testbase}/${NAME}/CMakeLists.txt
		#Should Contain    ${STR}    ${NAME}.c
		# The main C file must be in the cmake file
		${STR}    Get File    ${testbase}/${NAME}/src/unit.c
		Should Contain    ${STR}    ${NAME}.c
		# Unit test must not have logging enabled (bad for coverage)
		${STR}    Get File    ${testbase}/${NAME}/prj.conf
		Should Contain    ${STR}    CONFIG_TEST_LOGGING_DEFAULTS=n
		# Check that name follows a good pattern
		${STR}    Get File    ${testbase}/${NAME}/testcase.yaml
		Should Contain    ${STR}    ${NAME}.unit
		# This will match tag
		Should Contain    ${STR}    ${NAME}.c
	END
