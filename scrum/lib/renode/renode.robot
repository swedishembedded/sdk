*** Settings ***
Variables  ${CURDIR}/../../variables.py
Library  OperatingSystem
Resource  ${ROOT_DIR}/scrum/lib/library.robot

*** Variables ***

*** Test Cases ***

Renode interface is implemented
	Set Test Variable  ${LIBRARY}  renode
	Set Test Variable  ${MODULE}  renode
	Implements object interface
	Has public function connect
	Has public function disconnect
	Has public function wait_request
	Has public function send_response

*** Keywords ***
