*** Settings ***
Library  OperatingSystem
Resource  ${CURDIR}/readme.robot
Variables  ${CURDIR}/../variables.py

*** Keywords ***

Git repository has been setup
	Zephyr repository has a west yaml file
	Zephyr repository has a top level CMake file
	Zephyr repository has a top level Kconfig file
	Zephyr repository has a readme file
	Zephyr repository has a license file
	Zephyr repository has a gitignore file
	Zephyr repository has a CODEOWNERS file
	Zephyr repository has a precommit hook
	Zephyr repository has a VERSION file
	Zephyr repository has an init script that sets up the workspace
	Zephyr repository has a robot framework script that checks repository structure

Zephyr repository has a readme file
	Set Test Variable  ${README}  ${PROJECT_ROOT}/README.rst
	File Should Exist  ${README}
	Readme should follow sales letter structure

Zephyr repository has a top level CMake file
	File Should Exist  ${PROJECT_ROOT}/CMakeLists.txt

Zephyr repository has a top level Kconfig file
	File Should Exist  ${PROJECT_ROOT}/Kconfig

Zephyr repository has a license file
	File Should Exist  ${PROJECT_ROOT}/LICENSE

Zephyr repository has a gitignore file
	File Should Exist  ${PROJECT_ROOT}/.gitignore

Zephyr repository has a west yaml file
	File Should Exist  ${PROJECT_ROOT}/west.yml

Zephyr repository has a CODEOWNERS file
	File Should Exist  ${PROJECT_ROOT}/CODEOWNERS

Zephyr repository has a precommit hook
	File Should Exist  ${PROJECT_ROOT}/.githooks/pre-commit

Zephyr repository has a VERSION file
	File Should Exist  ${PROJECT_ROOT}/VERSION

Zephyr repository has an init script that sets up the workspace
	File Should Exist  ${PROJECT_ROOT}/scripts/init

Zephyr repository has a robot framework script that checks repository structure
	File Should Exist  ${PROJECT_ROOT}/scrum/infrastructure.robot
