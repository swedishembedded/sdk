# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

*** Settings ***
Library  OperatingSystem
Library  testing.py

*** Variables ***
${ROOT_DIR}  ${CURDIR}/../../

*** Test Cases ***

Testing infrastructure has been fully setup
	Check unit testing module enabled
	Unity directory structure has been setup
	Cmock directory structure has been setup
	There is a Kconfig option to enable unit testing
	There is a unity config
	There is a zephyr specific teardown that works with twister
	There is support for unity command line arguments in tests
	There is a code coverage check

*** Keywords ***

Unity directory structure has been setup
	Unity is placed in expected location
	Unity config has been created
	Unity twister compliant teardown has been created
	Unity cmake and kconfig have been created

Unity is placed in expected location
	Directory Should Exist  ${ROOT_DIR}/../modules/test/cmock/vendor/unity/

Unity config has been created
	File Should Exist  ${ROOT_DIR}/../modules/test/testing/unity/unity_cfg.yaml
	File Should Exist  ${ROOT_DIR}/../modules/test/testing/unity/unity_config.h

Unity twister compliant teardown has been created
	File Should Exist  ${ROOT_DIR}/../modules/test/testing/unity/teardown.c

Unity cmake and kconfig have been created
	File Should Exist  ${ROOT_DIR}/../modules/test/testing/unity/Kconfig
	File Should Exist  ${ROOT_DIR}/../modules/test/testing/unity/CMakeLists.txt

CMock directory structure has been setup
	CMock is placed in expected location

CMock is placed in expected location
	Directory Should Exist  ${ROOT_DIR}/../modules/test/cmock/

CMock cmake and kconfig have been created
	File Should Exist  ${ROOT_DIR}/modules/cmock/Kconfig
	File Should Exist  ${ROOT_DIR}/modules/cmock/CMakeLists.txt

There is a Kconfig option to enable unit testing
	${CONFIG} =  Grep File  ${ROOT_DIR}/../modules/test/testing/cmock/Kconfig  CMOCK
	Should Not Be Empty  ${CONFIG}

There is a unity config
	File Should Exist  ${ROOT_DIR}/../modules/test/testing/unity/unity_cfg.yaml

There is a zephyr specific teardown that works with twister
	${STR} =  Grep File  ${ROOT_DIR}/../modules/test/testing/unity/teardown.c  test_suite_teardown
	Should Not Be Empty  ${STR}

There is support for unity command line arguments in tests
	${STR} =  Grep File  ${ROOT_DIR}/../modules/test/testing/unity/main.c  native_get_test_cmd_line_args
	Should Not Be Empty  ${STR}

There is a code coverage check
	File Should Exist  ${ROOT_DIR}/scripts/coverage
	${STR} =  Grep File  ${ROOT_DIR}/scripts/test  scripts/coverage
	Should Not Be Empty  ${STR}
