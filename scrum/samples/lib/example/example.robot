*** Settings ***
Library  ${CURDIR}/../../../infrastructure/directory-structure.py

*** Variables ***
${APP}  example
${APP_NAME}  samples/lib/example/${APP}

*** Test Cases ***

Application should have correct structure
	App title should be  Example sample
	App should have release build
	App check integration platform  native_posix
