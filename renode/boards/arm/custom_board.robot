# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

*** Settings ***
Suite Setup                   Setup
Suite Teardown                Teardown
Test Setup                    Reset Emulation
Test Teardown                 Test Teardown
Resource                      ${RENODEKEYWORDS}
Variables                     ${CURDIR}/../../../scrum/variables.py

*** Variables ***
${UART}                       sysbus.usart2

*** Test Cases ***
Run Shell Sample
    Execute Command           set bin @${PROJECT_ROOT}/build-apps/custom_board/shell/apps.shell.release/zephyr/zephyr.elf
    Execute Command           include @scripts/single-node/stm32f4_discovery.resc

    Execute Command           showAnalyzer ${UART}
    Create Terminal Tester    ${UART}

    Start Emulation

    Wait For Line On Uart     Booting Zephyr OS
