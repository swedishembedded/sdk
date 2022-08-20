# SPDX-License-Identifier: Apache-2.0
# Copyright 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

*** Settings ***
Library  OperatingSystem
Library  Process
Library  directory-structure.py

*** Variables ***
${ROOT_DIR}  ${CURDIR}/../../

*** Test Cases ***

Apps have proper directory structure
	@{APPS} =	List Directories In Directory	${ROOT_DIR}/apps/
	FOR  ${APP}  IN  @{APPS}
		Set Test Variable  ${APP_NAME}  apps/${APP}
		Set Test Variable  ${APP_SOURCE}  ${ROOT_DIR}/${APP_NAME}
		App directory has proper structure
		# Each app should have a robot file
		File Should Exist  ${CURDIR}/../apps/${APP}.robot
	END

Samples have proper directory structure
	@{L1S} =	List Directories In Directory	${ROOT_DIR}/samples/
	FOR  ${L1}  IN  @{L1S}
		Directory Should Exist  ${ROOT_DIR}/${L1}
		@{L2S} =	List Directories In Directory	${ROOT_DIR}/${L1}
		FOR  ${L2}  IN  @{L2S}
			Directory Should Exist  ${ROOT_DIR}/${L1}/${L2}
		END
	END

Boards have proper directory structure
	@{ARCHS} =	List Directories In Directory	${ROOT_DIR}/boards/
	FOR  ${ARCH}  IN  @{ARCHS}
		@{BOARDS} =	List Directories In Directory	${ROOT_DIR}/boards/${ARCH}/
		FOR  ${BOARD}  IN  @{BOARDS}
			Set Test Variable  ${ARCH}  ${ARCH}
			Set Test Variable  ${BOARD}  ${BOARD}
			Set Test Variable  ${BOARD_SOURCE}  ${ROOT_DIR}/boards/${ARCH}/${BOARD}
			Board directory has proper structure
		END
	END

CMake libraries have proper structure
	CMake directory has proper structure

Drivers have proper structure
	File Should Exist  ${ROOT_DIR}/drivers/CMakeLists.txt
	File Should Exist  ${ROOT_DIR}/drivers/Kconfig
	@{DRIVER_TYPES} =	List Directories In Directory	${ROOT_DIR}/drivers/
	FOR  ${DRIVER_TYPE}  IN  @{DRIVER_TYPES}
		File Should Exist  ${ROOT_DIR}/drivers/${DRIVER_TYPE}/CMakeLists.txt
		File Should Exist  ${ROOT_DIR}/drivers/${DRIVER_TYPE}/Kconfig
		@{DRIVERS} =	List Files In Directory	${ROOT_DIR}/drivers/${DRIVER_TYPE}/  *.c
		FOR  ${DRIVER}  IN  @{DRIVERS}
			# make sure there is a test for each driver
			${PATH}  ${FILE} =  Split Path  ${DRIVER}
			${NAME}  ${EXT} =  Split Extension  ${FILE}
			Directory Should Exist  ${ROOT_DIR}/tests/drivers/${DRIVER_TYPE}/${NAME}/
			# make sure there is a scrum task for the driver
			File Should Exist  ${ROOT_DIR}/scrum/drivers/${DRIVER_TYPE}/${NAME}.robot
			# make sure driver C file is included
			${STR}    Get File    ${ROOT_DIR}/tests/drivers/${DRIVER_TYPE}/${NAME}/CMakeLists.txt
			Should Contain    ${STR}    ${NAME}.c
			# check if unit test includes the file
			${STR}    Get File    ${ROOT_DIR}/tests/drivers/${DRIVER_TYPE}/${NAME}/src/unit.c
			Should Contain    ${STR}    ${NAME}.c
			# All drivers must have a sample
			Set Test Variable  ${APP_SOURCE}  ${ROOT_DIR}/samples/drivers/${DRIVER_TYPE}/${NAME}
			App directory has proper structure
			# Each sample should have a robot test to verify it's content
			File Should Exist  ${CURDIR}/../samples/drivers/${DRIVER_TYPE}/${NAME}.robot
		END
	END

Libraries have proper structure
	File Should Exist  ${ROOT_DIR}/lib/CMakeLists.txt
	File Should Exist  ${ROOT_DIR}/lib/Kconfig
	@{LIBRARIES} =	List Directories In Directory	${ROOT_DIR}/lib/
	FOR  ${LIBRARY}  IN  @{LIBRARIES}
		File Should Exist  ${ROOT_DIR}/lib/${LIBRARY}/CMakeLists.txt
		File Should Exist  ${ROOT_DIR}/lib/${LIBRARY}/Kconfig
		@{SOURCES} =	List Files In Directory	${ROOT_DIR}/lib/${LIBRARY}/  *.c
		FOR  ${SOURCE}  IN  @{SOURCES}
			# make sure there is a test for each library object
			${PATH}  ${FILE} =  Split Path  ${SOURCE}
			${NAME}  ${EXT} =  Split Extension  ${FILE}
			Directory Should Exist  ${ROOT_DIR}/tests/lib/${LIBRARY}/${NAME}/

			# make sure there is a scrum task for the library object
			File Should Exist  ${ROOT_DIR}/scrum/lib/${LIBRARY}/${NAME}.robot

			# make sure driver C file is included
			${STR}    Get File    ${ROOT_DIR}/tests/lib/${LIBRARY}/${NAME}/CMakeLists.txt
			Should Contain    ${STR}    ${NAME}.c

			# check if unit test includes the file
			${STR}    Get File    ${ROOT_DIR}/tests/lib/${LIBRARY}/${NAME}/src/unit.c
			Should Contain    ${STR}    ${NAME}.c

			Set Test Variable  ${APP_SOURCE}  ${ROOT_DIR}/samples/lib/${LIBRARY}/${NAME}
			${res} =    run process    ls    ${APP_SOURCE}
			# If a sample exists, check it
			Run Keyword If  ${res.rc} == 0  App directory has proper structure

			# Each sample should have a robot test to verify it's content
			Run Keyword If  ${res.rc} == 0  File Should Exist  ${CURDIR}/../samples/lib/${LIBRARY}/${NAME}.robot
		END
	END

Scripts have proper structure
	There is a repository init script
	There is a script that builds all configurations
	There is a script that checks compliance

Tests have proper directory structure
	@{TEST_CLASSES} =	List Directories In Directory	${ROOT_DIR}/tests/
	FOR  ${TEST_CLASS}  IN  @{TEST_CLASSES}
		@{TEST_TYPES} =	List Directories In Directory	${ROOT_DIR}/tests/${TEST_CLASS}/
		FOR  ${TEST_TYPE}  IN  @{TEST_TYPES}
			@{TESTS} =	List Directories In Directory	${ROOT_DIR}/tests/${TEST_CLASS}/${TEST_TYPE}/
			FOR  ${TEST}  IN  @{TESTS}
				Set Test Variable  ${TEST_NAME}  ${TEST_CLASS}/${TEST_TYPE}/${TEST}/
				Set Test Variable  ${TEST_SOURCE}  ${ROOT_DIR}/tests/${TEST_NAME}/
				Test directory has proper structure
				# A test should test only one C file and that file should exist
				# Organize your directries such that your tests correspond to
				# the source files you are testing
				File Should Exist  ${ROOT_DIR}/${TEST_CLASS}/${TEST_TYPE}/${TEST}.c
			END
		END
	END

*** Keywords ***

App directory has proper structure
	File Should Exist  ${APP_SOURCE}/CMakeLists.txt
	File Should Exist  ${APP_SOURCE}/sample.yaml
	File Should Exist  ${APP_SOURCE}/prj.conf
	File Should Exist  ${APP_SOURCE}/README.md
	File Should Exist  ${APP_SOURCE}/src/main.c

Board directory has proper structure
	File Should Exist  ${BOARD_SOURCE}/board.cmake
	File Should Exist  ${BOARD_SOURCE}/Kconfig.board
	File Should Exist  ${BOARD_SOURCE}/Kconfig.defconfig
	File Should Exist  ${BOARD_SOURCE}/${BOARD}_defconfig
	File Should Exist  ${BOARD_SOURCE}/${BOARD}.dts
	File Should Exist  ${BOARD_SOURCE}/${BOARD}.yaml
	File Should Exist  ${BOARD_SOURCE}/support/openocd.cfg

CMake directory has proper structure
	File Should Exist  ${ROOT_DIR}/cmake/CMakeLists.txt

There is a repository init script
	File Should Exist  ${ROOT_DIR}/scripts/init

There is a script that builds all configurations
	File Should Exist  ${ROOT_DIR}/scripts/build

There is a script that checks compliance
	File Should Exist  ${ROOT_DIR}/scripts/check

Test directory has proper structure
	File Should Exist  ${TEST_SOURCE}/CMakeLists.txt
	File Should Exist  ${TEST_SOURCE}/testcase.yaml
	File Should Exist  ${TEST_SOURCE}/prj.conf
	File Should Exist  ${TEST_SOURCE}/README.md
	File Should Exist  ${TEST_SOURCE}/src/unit.c
	File Should Exist  ${TEST_SOURCE}/src/integration.c
	Check Test Configuration
