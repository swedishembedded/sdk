# SPDX-License-Identifier: Apache-2.0
# Copyright 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

*** Settings ***
Variables  ${CURDIR}/../../integrity/variables.py

Library  OperatingSystem
Library  String

*** Variables ***
${RELEASES}  ${ROOT_DIR}/release/

*** Test Cases ***

Shell application has been built
	Release of apps/shell is built for custom_board

*** Keywords ***

Release of ${APP} is built for ${BOARD}
	${NAME} =  Replace String  ${APP}  /  .
	Set Test Variable  ${RELEASE_BUILD_DIR}  build-release/${BOARD}/${APP}/
	File Should Exist  ${RELEASES}/${NAME}-${BOARD}-${VERSION}.tar.gz
	App release contains all necessary files

App release contains all necessary files
	File Should Exist  ${RELEASE_BUILD_DIR}/release/spdx/zephyr.spdx
	File Should Exist  ${RELEASE_BUILD_DIR}/release/spdx/app.spdx
	File Should Exist  ${RELEASE_BUILD_DIR}/release/spdx/build.spdx
	File Should Exist  ${RELEASE_BUILD_DIR}/release/zephyr.elf
	File Should Exist  ${RELEASE_BUILD_DIR}/release/zephyr.hex
	File Should Exist  ${RELEASE_BUILD_DIR}/release/reference.pdf
