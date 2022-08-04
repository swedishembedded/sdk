*** Settings ***
Suite Setup                   Setup
Suite Teardown                Teardown
Test Setup                    Reset Emulation
Test Teardown                 Test Teardown
Resource                      ${RENODEKEYWORDS}

*** Variables ***
${PROJECT_BASE}               %{PROJECT_BASE}
${APPLICATION_BINARY_DIR}     %{APPLICATION_BINARY_DIR}
${APPLICATION_SOURCE_DIR}     %{APPLICATION_SOURCE_DIR}
${BOARD}                      %{BOARD}
${UART}                       sysbus.usart2

*** Test Cases ***
Run Shell Sample
    Execute Command           set bin @${APPLICATION_BINARY_DIR}/zephyr/zephyr.elf
    Execute Command           include @${APPLICATION_SOURCE_DIR}/boards/${BOARD}.resc
    Execute Command           set ROOT_DIR @${PROJECT_BASE}

    Execute Command           showAnalyzer ${UART}
    Create Terminal Tester    ${UART}

    Start Emulation

    Wait For Line On Uart     Booting Zephyr OS
