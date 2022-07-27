*** Settings ***
Variables  ${CURDIR}/../variables.py
Library  OperatingSystem
Library  library.py

*** Keywords ***

Module has private function ${FUNC}
	Module Has Static Function  ${FUNC}

Module has public function ${FUNC}
	Module Has Public Function  ${FUNC}
