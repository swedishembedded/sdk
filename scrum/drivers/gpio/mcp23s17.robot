*** Settings ***
Variables  ${CURDIR}/../../variables.py
Library  OperatingSystem
Resource  ${ROOT_DIR}/scrum/drivers/driver.robot

*** Variables ***

*** Test Cases ***

Driver is ready
	Set Test Variable  ${DRIVER_TYPE}  gpio
	Set Test Variable  ${DRIVER_NAME}  mcp23s17
	Driver has proper format
	Driver has simulation
	Driver has private function init
	Driver implements GPIO api

*** Keywords ***
