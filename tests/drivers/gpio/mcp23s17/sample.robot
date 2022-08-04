*** Settings ***
Resource                      ${RENODEKEYWORDS}
Suite Setup                   Setup
Suite Teardown                Teardown
Test Setup                    Reset Emulation
Test Teardown                 Test Teardown

*** Variables ***
${PROJECT_BASE}               %{PROJECT_BASE}
${APPLICATION_BINARY_DIR}     %{APPLICATION_BINARY_DIR}
${APPLICATION_SOURCE_DIR}     %{APPLICATION_SOURCE_DIR}
${BOARD}                      %{BOARD}
${UART}                       sysbus.usart2

*** Test Cases ***

Test runs on board
	Boot
	Wait For Line On Uart  PROJECT EXECUTION SUCCESSFUL

*** Keywords ***

Boot
	Execute Command           set bin @${APPLICATION_BINARY_DIR}/zephyr/zephyr.elf
	Execute Command           set APPLICATION_BINARY_DIR @${APPLICATION_BINARY_DIR}
	Execute Command           include @${APPLICATION_SOURCE_DIR}/boards/${BOARD}.resc
	Create Terminal Tester    ${UART}
	Start Emulation

