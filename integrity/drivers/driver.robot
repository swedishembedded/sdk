# SPDX-License-Identifier: Apache-2.0
# Copyright 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

*** Settings ***
Variables  ${CURDIR}/../variables.py
Library  OperatingSystem
Library  ${CURDIR}/driver.py

*** Keywords ***

Driver has proper format
	Driver Check data is private
	Driver has compatible ${DRIVER_NAME}
	Driver Check supports multi instance

Driver has compatible ${COMPAT}
	Driver Check Compatible  ${COMPAT}

Driver has private function ${FUNC}
	Driver Has Static Function  ${FUNC}

Driver has simulation
	${SIM_DEVICE}=  Driver name to camel case  ${DRIVER_NAME}
	File Should Exist  ${ROOT_DIR}/renode/drivers/${DRIVER_TYPE}/${SIM_DEVICE}.cs

Driver implements GPIO api
	Driver Has Static Function  pin_configure
	Driver Has Static Function  port_get_raw
	Driver Has Static Function  port_set_masked_raw
	Driver Has Static Function  port_set_bits_raw
	Driver Has Static Function  port_clear_bits_raw
	Driver Has Static Function  port_toggle_bits
	Driver Has Static Function  pin_interrupt_configure
	Driver Has Static Function  manage_callback
	Driver Has Static Function  get_pending_int
