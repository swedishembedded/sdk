# SPDX-License-Identifier: Apache-2.0
# Copyright 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

*** Settings ***
Library  OperatingSystem
Library  Process
Variables  ${CURDIR}/../variables.py

*** Test Cases ***

Basic directory structure has been setup
	Project has a gitlab ci pipeline
	Project has a west yaml file
	Project has a top level CMake file
	Project has a top level Kconfig file
	Project has a license file
	Project has a gitignore file
	Project has a CODEOWNERS file
	Project has a VERSION file
	Project has a checkpatch config
	Project should be a zephyr module
	Project has basic set of build scripts

Codeowners file is sorted
	${result}=  Run Process  ${SWEDISH_EMBEDDED_SDK}/scripts/ci/check-file-sorted  ${PROJECT_ROOT}/CODEOWNERS
	IF  ${result.rc} != 0
		Log To Console  CODEOWNERS file is not sorted
		Log To Console  ${result.stdout}
		Fail
	END

*** Keywords ***

Project has a gitlab ci pipeline
	File Should Exist  ${PROJECT_ROOT}/.gitlab-ci.yml

Project has a top level CMake file
	File Should Exist  ${PROJECT_ROOT}/CMakeLists.txt

Project has a top level Kconfig file
	File Should Exist  ${PROJECT_ROOT}/Kconfig

Project has a license file
	File Should Exist  ${PROJECT_ROOT}/LICENSE

Project has a gitignore file
	File Should Exist  ${PROJECT_ROOT}/.gitignore

Project has a west yaml file
	File Should Exist  ${PROJECT_ROOT}/west.yml

Project has a CODEOWNERS file
	File Should Exist  ${PROJECT_ROOT}/CODEOWNERS

Project has a VERSION file
	File Should Exist  ${PROJECT_ROOT}/VERSION

Project has a checkpatch config
	File Should Exist  ${PROJECT_ROOT}/.checkpatch.conf

Project should be a zephyr module
	File Should Exist  ${PROJECT_ROOT}/zephyr/module.yml

Project has basic set of build scripts
	File Should Exist  ${PROJECT_ROOT}/scripts/init
	File Should Exist  ${PROJECT_ROOT}/scripts/build
	File Should Exist  ${PROJECT_ROOT}/scripts/test
	File Should Exist  ${PROJECT_ROOT}/scripts/check
	File Should Exist  ${PROJECT_ROOT}/scripts/fix
