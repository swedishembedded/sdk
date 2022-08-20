# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

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

Leds can be controlled by buttons
	Set Test Variable  ${GPIO}  GPIO_E0
	Boot
	FOR  ${PIN}  IN RANGE  8  15
		${btn}=  Evaluate  ${PIN} - 8
		Write Line To Uart  gpio get ${GPIO} ${PIN}
		Wait For Line On Uart  Value 0  timeout=2

		Execute Command  sysbus.spi1.gpioExpander.A${btn} Press
		Write Line To Uart  gpio get ${GPIO} ${PIN}
		Wait For Line On Uart  Value 1  timeout=2

		Execute Command  sysbus.spi1.gpioExpander.A${btn} Release
		Write Line To Uart  gpio get ${GPIO} ${PIN}
		Wait For Line On Uart  Value 0  timeout=2
	END

*** Keywords ***

Boot
	Execute Command           set bin @${APPLICATION_BINARY_DIR}/zephyr/zephyr.elf
	Execute Command           set APPLICATION_BINARY_DIR @${APPLICATION_BINARY_DIR}
	Execute Command           include @${CURDIR}/testbench.resc
	Create Terminal Tester    ${UART}
	Start Emulation
	Wait For Line On Uart		MCP23S17 initialized

