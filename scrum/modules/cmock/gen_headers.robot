*** Settings ***
Library  OperatingSystem
Variables  ${CURDIR}/../../variables.py
Library  ${PROJECT_ROOT}/modules/cmock/gen_headers.py
Suite Setup  Setup

*** Variables ***
${BUILD_DIR}  ${PROJECT_ROOT}/build/modules/cmock/

*** Test Cases ***

Script filters out all comments
	Gen Headers
	...  ${CURDIR}/input.txt
	...  ${BUILD_DIR}/output.txt
	...  ${BUILD_DIR}/wrap.txt
	Should have Same Content
	...  ${CURDIR}/output.txt
	...  ${BUILD_DIR}/output.txt
	Should have Same Content
	...  ${CURDIR}/wrap.txt
	...  ${BUILD_DIR}/wrap.txt

*** Keywords ***

Setup
	Create Directory  ${BUILD_DIR}

Should Have Same Content
	[Arguments]  ${expected_file}  ${actual_file}
	${expected} =    Get File    ${expected_file}
	${actual} =    Get File    ${actual_file}
	Should Be Equal As Strings    ${expected}    ${actual}
