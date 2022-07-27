*** Settings ***
Variables  ${CURDIR}/../variables.py
Library  OperatingSystem
Library  driver.py

*** Keywords ***

Driver has proper format
	Driver Check data is private
	Driver Check supports multi instance
	Driver has compatible ${DRIVER_NAME}

Driver has compatible ${COMPAT}
	Driver Check Compatible  ${COMPAT}

Driver has private function ${FUNC}
	Driver Has Static Function  ${FUNC}

Driver has simulation
	${SIM_DEVICE}=  Driver name to camel case  ${DRIVER_NAME}
	File Should Exist  ${ROOT_DIR}/renode/drivers/${DRIVER_TYPE}/${SIM_DEVICE}.cs
