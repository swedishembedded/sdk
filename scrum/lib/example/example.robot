*** Settings ***
Variables  ${CURDIR}/../../variables.py
Library  OperatingSystem
Resource  ${ROOT_DIR}/scrum/lib/library.robot

*** Variables ***

*** Test Cases ***

Example sensor driver is ready
	Set Test Variable  ${LIBRARY}  example
	Set Test Variable  ${MODULE}  example
	Module has public function object_init
