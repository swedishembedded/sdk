*** Settings ***
Library  OperatingSystem
Library  testing.py

*** Variables ***
${ROOT_DIR}  ${CURDIR}/../../

*** Test Cases ***

Testing infrastructure has been fully setup
	Check unity enabled
	Unity directory structure has been setup
	Check cmock enabled
	Cmock directory structure has been setup
	There is a Kconfig option to enable unit testing
	There is a unity config
	There is a zephyr specific teardown that works with twister
	There is support for unity command line arguments in tests
	There is a code coverage check

*** Keywords ***

Unity directory structure has been setup
	Unity is placed in expected location
	Unity config has been created
	Unity twister compliant teardown has been created
	Unity cmake and kconfig have been created

Unity is placed in expected location
	Directory Should Exist  ${ROOT_DIR}/../modules/test/cmock/vendor/unity/

Unity config has been created
	File Should Exist  ${ROOT_DIR}/modules/unity/unity_cfg.yaml
	File Should Exist  ${ROOT_DIR}/modules/unity/unity_config.h

Unity twister compliant teardown has been created
	File Should Exist  ${ROOT_DIR}/modules/unity/teardown.c

Unity cmake and kconfig have been created
	File Should Exist  ${ROOT_DIR}/modules/unity/Kconfig
	File Should Exist  ${ROOT_DIR}/modules/unity/CMakeLists.txt

CMock directory structure has been setup
	CMock is placed in expected location

CMock is placed in expected location
	Directory Should Exist  ${ROOT_DIR}/../modules/test/cmock/

CMock cmake and kconfig have been created
	File Should Exist  ${ROOT_DIR}/modules/cmock/Kconfig
	File Should Exist  ${ROOT_DIR}/modules/cmock/CMakeLists.txt

There is a Kconfig option to enable unit testing
	${CONFIG} =  Grep File  ${ROOT_DIR}/modules/cmock/Kconfig  CMOCK
	Should Not Be Empty  ${CONFIG}

There is a unity config
	File Should Exist  ${ROOT_DIR}/modules/unity/unity_cfg.yaml

There is a zephyr specific teardown that works with twister
	${STR} =  Grep File  ${ROOT_DIR}/modules/unity/teardown.c  test_suite_teardown
	Should Not Be Empty  ${STR}

There is support for unity command line arguments in tests
	${STR} =  Grep File  ${ROOT_DIR}/modules/unity/main.c  native_get_test_cmd_line_args
	Should Not Be Empty  ${STR}

There is a code coverage check
	File Should Exist  ${ROOT_DIR}/scripts/coverage
	${STR} =  Grep File  ${ROOT_DIR}/scripts/test  scripts/coverage
	Should Not Be Empty  ${STR}
