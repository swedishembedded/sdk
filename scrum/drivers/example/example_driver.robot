*** Settings ***
Variables  ${CURDIR}/../../variables.py
Library  OperatingSystem
Resource  ${ROOT_DIR}/scrum/drivers/driver.robot

*** Variables ***

*** Test Cases ***

Example sensor driver is ready
	Set Test Variable  ${DRIVER_TYPE}  example
	Set Test Variable  ${DRIVER_NAME}  example_driver
	Driver has proper format
	Driver has private function init
