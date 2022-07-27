*** Settings ***
Library  ${CURDIR}/../../../infrastructure/directory-structure.py

*** Variables ***
${APP}  example_sensor
${APP_NAME}  samples/drivers/example/${APP}

*** Test Cases ***

Application should have correct structure
	App title should be  Example sensor sample
	App should have release build
	App check integration platform  native_posix
