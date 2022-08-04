*** Settings ***
Variables  ${CURDIR}/../../../variables.py
Library  OperatingSystem
Library  ${ROOT_DIR}/scrum/DocChecker.py

*** Variables ***
${FILE}  ${ROOT_DIR}/renode/drivers/gpio/Mcp23S17.cs

*** Test Cases ***

Registers are implemented
	Register IODIR implemented as value
	Register IPOL implemented as value
	Register GPINTEN implemented as value
	Register DEFVAL implemented as value
	Register INTCON implemented as value
	Register IOCON implemented as value
	Register GPPU implemented as value
	Register INTF implemented as value
	Register INTCAP implemented as value
	Register OLAT implemented as value

*** Keywords ***

Register ${reg} implemented as value
	Check Page Contains Text
	...  ${FILE}
	...  IValueRegisterField.*${reg}A
	Check Page Contains Text
	...  ${FILE}
	...  IValueRegisterField.*${reg}B
	Check Page Contains Text
	...  ${FILE}
	...  Registers.${reg}A, new ByteRegister
	Check Page Contains Text
	...  ${FILE}
	...  Registers.${reg}B, new ByteRegister
	Check Page Contains Text
	...  ${FILE}
	...  out ${reg}A, name: "${reg}A"
	Check Page Contains Text
	...  ${FILE}
	...  out ${reg}B, name: "${reg}B"

Register ${reg} implemented as enum
	Check Page Contains Text
	...  ${FILE}
	...  ${reg} = new .*NumberOfPins
	Check Page Contains Text
	...  ${FILE}
	...  Registers.${reg}, new WordRegister
