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

*** Test Cases ***

Documentation generation has been setup
	Documentation infrastructure is in place

Documentation is in good order
	Set Test Variable  ${DOC_SDK_BASE}  ${ROOT_DIR}/doc/sdk
	PDF header has been created
	Drivers have been documented
	Libraries have been documented
	Boards have been documented

*** Keywords ***

Documentation infrastructure is in place
	# Sphinx extensions
	Directory Should Exist  ${ROOT_DIR}/doc/_extensions/
	# Documentation utility scripts used during build process
	Directory Should Exist  ${ROOT_DIR}/doc/_scripts/
	# Static files that are shared across all docsets
	Directory Should Exist  ${ROOT_DIR}/doc/_static/
	# Main theme directory used for all doc pages
	Directory Should Exist  ${ROOT_DIR}/doc/_theme/sphinx_swe_theme
	# New cmake driven doc build process
	File Should Exist  ${ROOT_DIR}/doc/CMakeLists.txt

	# Configuration files for all docsets
	File Should Exist  ${ROOT_DIR}/doc/zephyr/conf.py
	File Should Exist  ${ROOT_DIR}/doc/mcuboot/conf.py
	File Should Exist  ${ROOT_DIR}/doc/renode-docs/conf.py
	File Should Exist  ${ROOT_DIR}/doc/sdk/conf.py

	# Main SDK doxygen file used to generate docs for sdk APIs
	File Should Exist  ${ROOT_DIR}/doc/sdk/sdk.doxyfile.in
	File Should Exist  ${ROOT_DIR}/doc/sdk/index.rst
	File Should Exist  ${ROOT_DIR}/doc/sdk/index-tex.rst

PDF header has been created
	File Should Exist  ${ROOT_DIR}/doc/_static/latex/title.tex

Drivers have been documented
	@{DRIVER_CLASSES} =	List Directories In Directory	${ROOT_DIR}/drivers/
	FOR  ${DRIVER_CLASS}  IN  @{DRIVER_CLASSES}
		Directory Should Exist  ${DOC_SDK_BASE}/drivers/${DRIVER_CLASS}/
		@{DRIVERS} =	List Files In Directory  ${ROOT_DIR}/drivers/${DRIVER_CLASS}/  *.c
		FOR  ${DRIVER}  IN  @{DRIVERS}
			${PATH}  ${FILE}  Split Path  ${DRIVER}
			${SECTION}  ${EXT}  Split Extension  ${FILE}
			Set Test Variable  ${CHAPTER_FILES}  ${DOC_SDK_BASE}/drivers/${DRIVER_CLASS}/${SECTION}/
			File Should Exist  ${DOC_SDK_BASE}/drivers/${DRIVER_CLASS}/${SECTION}.rst
			Chapter follows reference structure
		END
		# find invalid directories
		@{INVALIDS} =	List Directories In Directory	${ROOT_DIR}/drivers/${DRIVER_CLASS}/
		FOR  ${INVALID}  IN  @{INVALIDS}
			Log To Console  Do not put directories under driver class folder (drivers/${DRIVER_CLASS}/${INVALID})
			Fail
		END
	END

Subsystems have been documented
	@{SUBSYSTS} =	List Directories In Directory	${ROOT_DIR}/subsys/
	FOR  ${SUBSYS}  IN  @{SUBSYSTS}
		File Should Exist  ${DOC_SDK_BASE}/subsys/${SUBSYS}.rst
		Set Test Variable  ${CHAPTER_FILES}  ${DOC_SDK_BASE}/subsys/${SUBSYS}/
		Chapter follows reference structure
	END

Libraries have been documented
	@{LIBRARIES} =	List Directories In Directory	${ROOT_DIR}/lib/
	FOR  ${LIB}  IN  @{LIBRARIES}
		File Should Exist  ${DOC_SDK_BASE}/lib/${LIB}/index.rst
		Check Page Contains Text
			...  ${DOC_SDK_BASE}/lib/index.rst
			...  ${LIB}/index.rst
		Set Test Variable  ${CHAPTER_FILES}  ${DOC_SDK_BASE}/lib/${LIB}/
		Chapter follows reference structure
	END

Boards have been documented
	@{ARCHS} =	List Directories In Directory	${ROOT_DIR}/boards/
	FOR  ${ARCH}  IN  @{ARCHS}
		@{BOARDS} =	List Directories In Directory	${ROOT_DIR}/boards/${ARCH}/
		FOR  ${BOARD}  IN  @{BOARDS}
			File Should Exist  ${ROOT_DIR}/boards/${ARCH}/${BOARD}/doc/index.rst
			Set Test Variable  ${CHAPTER_FILES}  ${ROOT_DIR}/boards/${ARCH}/${BOARD}/doc/
			Chapter follows board reference structure
		END
	END

