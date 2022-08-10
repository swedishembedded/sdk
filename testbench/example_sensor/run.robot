*** Settings ***
Suite Setup                   Setup
Suite Teardown                Teardown
Test Setup                    Reset Emulation
Test Teardown                 Test Teardown
Resource                      ${RENODEKEYWORDS}
Variables                     ${CURDIR}/../../scrum/variables.py

*** Variables ***
${UART}                       sysbus.usart2

*** Test Cases ***
Example sensor driver can read temperature
	Execute Command           include @${CURDIR}/custom_board.resc

	Create Terminal Tester    ${UART}
	Start Emulation
	Wait For Line On Uart     Booting Zephyr OS
	Execute Command	sysbus.spi1.exampleSensor Temperature 25
	Wait For Line On Uart     Temperature: 25.0
	Execute Command	sysbus.spi1.exampleSensor Temperature 41
	Wait For Line On Uart     Temperature: 41.0
	Execute Command	sysbus.spi1.exampleSensor Temperature 41.2
	Wait For Line On Uart     Temperature: 41.2
	Execute Command	sysbus.spi1.exampleSensor Temperature -10
	Wait For Line On Uart     Temperature: -10.0

Temperature limits are reported correctly
	Execute Command           include @${CURDIR}/custom_board.resc

	Create Terminal Tester    ${UART}
	Start Emulation
	Wait For Line On Uart     Booting Zephyr OS
	Execute Command	sysbus.spi1.exampleSensor Temperature 155
	Wait For Line On Uart     Temperature: 150.0
	Execute Command	sysbus.spi1.exampleSensor Temperature -60
	Wait For Line On Uart     Temperature: -55
