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
${INTEGRITY_DIR}  ${ROOT_DIR}/integrity/

*** Keywords ***

Directory has app files ${APP_SOURCE}
	File Should Exist  ${APP_SOURCE}/CMakeLists.txt
	File Should Exist  ${APP_SOURCE}/sample.yaml
	File Should Exist  ${APP_SOURCE}/prj.conf
	File Should Exist  ${APP_SOURCE}/README.md
	File Should Exist  ${APP_SOURCE}/src/main.c

Directory has board files ${BOARD_SOURCE}
	File Should Exist  ${BOARD_SOURCE}/board.cmake
	File Should Exist  ${BOARD_SOURCE}/Kconfig.board
	File Should Exist  ${BOARD_SOURCE}/Kconfig.defconfig
	File Should Exist  ${BOARD_SOURCE}/${BOARD}_defconfig
	File Should Exist  ${BOARD_SOURCE}/${BOARD}.dts
	File Should Exist  ${BOARD_SOURCE}/${BOARD}.yaml
	File Should Exist  ${BOARD_SOURCE}/support/openocd.cfg

Directory has apps structure ${dir}
	@{APPS} =	List Directories In Directory	${dir}
	FOR  ${APP}  IN  @{APPS}
		Set Test Variable  ${APP_NAME}  apps/${APP}
		Set Test Variable  ${APP_SOURCE}  ${ROOT_DIR}/${APP_NAME}
		Directory has app files ${APP_SOURCE}
		# Each app should have a robot file
		File Should Exist  ${CURDIR}/../apps/${APP}.robot
	END

Directory has samples structure ${dir}
	@{L1S} =	List Directories In Directory	${dir}
	FOR  ${L1}  IN  @{L1S}
		Directory Should Exist  ${dir}/${L1}
		@{L2S} =	List Directories In Directory	${dir}/${L1}
		FOR  ${L2}  IN  @{L2S}
			Directory Should Exist  ${dir}/${L1}/${L2}
			# TODO: add additional samples checks
		END
	END

Directory has boards structure ${dir}
	@{ARCHS} =	List Directories In Directory	${dir}
	FOR  ${ARCH}  IN  @{ARCHS}
		@{BOARDS} =	List Directories In Directory	${ROOT_DIR}/boards/${ARCH}/
		FOR  ${BOARD}  IN  @{BOARDS}
			Set Test Variable  ${ARCH}  ${ARCH}
			Set Test Variable  ${BOARD}  ${BOARD}
			Set Test Variable  ${BOARD_SOURCE}  ${ROOT_DIR}/boards/${ARCH}/${BOARD}
			Directory has board files ${BOARD_SOURCE}
		END
	END

Directory has drivers structure ${driversbase}
	File Should Exist  ${driversbase}/CMakeLists.txt
	File Should Exist  ${driversbase}/Kconfig
	@{DRIVER_TYPES} =	List Directories In Directory	${driversbase}
	FOR  ${DRIVER_TYPE}  IN  @{DRIVER_TYPES}
		File Should Exist  ${driversbase}/${DRIVER_TYPE}/CMakeLists.txt
		File Should Exist  ${driversbase}/${DRIVER_TYPE}/Kconfig
	END

Directory has lib structure ${dir}
	File Should Exist  ${dir}/CMakeLists.txt
	File Should Exist  ${dir}/Kconfig
	@{LIBRARIES} =	List Directories In Directory	${dir}
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
			File Should Exist  ${INTEGRITY_DIR}/lib/${LIBRARY}/${NAME}.robot

			# make sure driver C file is included
			${STR}    Get File    ${ROOT_DIR}/tests/lib/${LIBRARY}/${NAME}/CMakeLists.txt
			Should Contain    ${STR}    ${NAME}.c

			# check if unit test includes the file
			${STR}    Get File    ${ROOT_DIR}/tests/lib/${LIBRARY}/${NAME}/src/unit.c
			Should Contain    ${STR}    ${NAME}.c

			Set Test Variable  ${APP_SOURCE}  ${ROOT_DIR}/samples/lib/${LIBRARY}/${NAME}
			${res} =    run process    ls    ${APP_SOURCE}
			# If a sample exists, check it
			Run Keyword If  ${res.rc} == 0  Directory has app files ${APP_SOURCE}

			# Each sample should have a robot test to verify it's content
			Run Keyword If  ${res.rc} == 0  File Should Exist  ${CURDIR}/../samples/lib/${LIBRARY}/${NAME}.robot
		END
	END

Directory has cmake structure ${dir}
	File Should Exist  ${dir}/CMakeLists.txt
	# TODO: check that all files are included into the cmake list and have cmake extensions

Directory has test files ${TEST_SOURCE}
	File Should Exist  ${TEST_SOURCE}/CMakeLists.txt
	File Should Exist  ${TEST_SOURCE}/testcase.yaml
	File Should Exist  ${TEST_SOURCE}/prj.conf
	File Should Exist  ${TEST_SOURCE}/README.md
	File Should Exist  ${TEST_SOURCE}/src/unit.c
	File Should Exist  ${TEST_SOURCE}/src/integration.c
	Check Test Configuration

Directory has tests structure ${dir}
	@{TEST_CLASSES} =	List Directories In Directory	${dir}
	FOR  ${TEST_CLASS}  IN  @{TEST_CLASSES}
		@{TEST_TYPES} =	List Directories In Directory	${dir}/${TEST_CLASS}/
		FOR  ${TEST_TYPE}  IN  @{TEST_TYPES}
			@{TESTS} =	List Directories In Directory	${dir}/${TEST_CLASS}/${TEST_TYPE}/
			FOR  ${TEST}  IN  @{TESTS}
				Set Test Variable  ${TEST_NAME}  ${TEST_CLASS}/${TEST_TYPE}/${TEST}/
				Set Test Variable  ${TEST_SOURCE}  ${dir}/${TEST_NAME}/
				Directory has test files ${TEST_SOURCE}
				# A test should test only one C file and that file should exist
				# Organize your directries such that your tests correspond to
				# the source files you are testing
				File Should Exist  ${ROOT_DIR}/${TEST_CLASS}/${TEST_TYPE}/${TEST}.c
			END
		END
	END

Directory has subsys structure ${subsys}
	@{DIRS}=  List Directories In Directory  ${subsys}
	FOR  ${DIR}  IN  @{DIRS}
		Set Test Variable  ${SUBSYS_DIR}  ${subsys}/${DIR}/
		File Should Exist  ${SUBSYS_DIR}/CMakeLists.txt
		File Should Exist  ${SUBSYS_DIR}/Kconfig
	END

Directory has scripts structure ${dir}
	# should be used for initializing a blank repo
	File Should Exist  ${dir}/init
	# should be used for building firmware only
	File Should Exist  ${dir}/build
	# should run integrity and other repo checks
	File Should Exist  ${dir}/check
	# should build and run all tests and generate coverage
	File Should Exist  ${dir}/test
