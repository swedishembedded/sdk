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

GPA pins are connected to gpio register
	Set Test Variable  ${GPIO}  GPIO_E0
	Boot
	FOR  ${PIN}  IN RANGE  0  7
		Write Line To Uart  gpio get ${GPIO} ${PIN}
		Wait For Line On Uart  Value 0  timeout=2

		Execute Command  sysbus.spi1.gpioExpander.A${PIN} Press
		Write Line To Uart  gpio get ${GPIO} ${PIN}
		Wait For Line On Uart  Value 1  timeout=2

		Execute Command  sysbus.spi1.gpioExpander.A${PIN} Release
		Write Line To Uart  gpio get ${GPIO} ${PIN}
		Wait For Line On Uart  Value 0  timeout=2
	END
	Check gpio io works 0x12 0x14 A0 0x01
	Check gpio io works 0x12 0x14 A1 0x02
	Check gpio io works 0x12 0x14 A2 0x04
	Check gpio io works 0x12 0x14 A3 0x08
	Check gpio io works 0x12 0x14 A4 0x10
	Check gpio io works 0x12 0x14 A5 0x20
	Check gpio io works 0x12 0x14 A6 0x40
	Check gpio io works 0x12 0x14 A7 0x80

GPB pins are connected to gpio register
	Set Test Variable  ${GPIO}  GPIO_E0
	Boot
	FOR  ${PIN}  IN RANGE  8  15
		${idx}=  Evaluate  ${PIN} - 8
		Write Line To Uart  gpio get ${GPIO} ${PIN}
		Wait For Line On Uart  Value 0  timeout=2

		Execute Command  sysbus.spi1.gpioExpander.B${idx} Press
		Write Line To Uart  gpio get ${GPIO} ${PIN}
		Wait For Line On Uart  Value 1  timeout=2

		Execute Command  sysbus.spi1.gpioExpander.B${idx} Release
		Write Line To Uart  gpio get ${GPIO} ${PIN}
		Wait For Line On Uart  Value 0  timeout=2
	END
	Check gpio io works 0x13 0x15 B0 0x01
	Check gpio io works 0x13 0x15 B1 0x02
	Check gpio io works 0x13 0x15 B2 0x04
	Check gpio io works 0x13 0x15 B3 0x08
	Check gpio io works 0x13 0x15 B4 0x10
	Check gpio io works 0x13 0x15 B5 0x20
	Check gpio io works 0x13 0x15 B6 0x40
	Check gpio io works 0x13 0x15 B7 0x80

*** Keywords ***

Boot
	Execute Command           set bin @${APPLICATION_BINARY_DIR}/zephyr/zephyr.elf
	Execute Command           set APPLICATION_BINARY_DIR @${APPLICATION_BINARY_DIR}
	Execute Command           include @${APPLICATION_SOURCE_DIR}/boards/${BOARD}.resc
	Create Terminal Tester    ${UART}
	Start Emulation

Check gpio io works ${gpio} ${olat} ${name} ${exp}
	Check gpio input works ${gpio} ${olat} ${name} ${exp}
	Check gpio output works ${gpio} ${olat} ${name} ${exp}

Check gpio input works ${gpio} ${olat} ${name} ${exp}
	${value}=  Execute Command  sysbus.spi1.gpioExpander ReadByte ${gpio}
	Should Contain  ${value}  0x00

	Execute Command  sysbus.spi1.gpioExpander.${name} Press
	Sleep  50ms
	${value}=  Execute Command  sysbus.spi1.gpioExpander ReadByte ${gpio}
	Should Contain  ${value}  ${exp}
	# OLAT should be zero here
	${value}=  Execute Command  sysbus.spi1.gpioExpander ReadByte ${olat}
	Should Contain  ${value}  0x00

	Execute Command  sysbus.spi1.gpioExpander.${name} Release
	Sleep  50ms
	${value}=  Execute Command  sysbus.spi1.gpioExpander ReadByte ${gpio}
	Should Contain  ${value}  0x00

Check gpio output works ${gpio} ${olat} ${name} ${exp}
	${value}=  Execute Command  sysbus.spi1.gpioExpander ReadByte ${gpio}
	Should Contain  ${value}  0x00

	Execute Command  sysbus.spi1.gpioExpander WriteByte ${gpio} ${exp}
	${value}=  Execute Command  sysbus.spi1.gpioExpander ReadByte ${gpio}
	Should Contain  ${value}  ${exp}
	# OLAT should contain output value as well
	${value}=  Execute Command  sysbus.spi1.gpioExpander ReadByte ${olat}
	Should Contain  ${value}  ${exp}

	Execute Command  sysbus.spi1.gpioExpander WriteByte ${gpio} 0x00
	Execute Command  sysbus.spi1.gpioExpander WriteByte ${olat} 0x00
