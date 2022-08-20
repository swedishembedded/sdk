# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

find_program(RENODE renode)
find_program(RENODE_TEST renode-test)

add_subdirectory_ifdef(CONFIG_TESTBENCH_NUCLEO401RE_MCP23S17
                       nucleo401re_mcp23s17)

set(TESTBENCH_DIR
    "${CMAKE_CURRENT_LIST_DIR}/${TESTBENCH}"
    CACHE INTERNAL "TESTBENCH_DIR")

set(RENODE_COMMANDS "path add @${PROJECT_BASE}/renode\;")
string(APPEND RENODE_COMMANDS
       "set bin @${APPLICATION_BINARY_DIR}/zephyr/${KERNEL_ELF_NAME}\;")
string(APPEND RENODE_COMMANDS
       "set APPLICATION_BINARY_DIR @${APPLICATION_BINARY_DIR}\;")
string(APPEND RENODE_COMMANDS "set SDK_BASE @${PROJECT_BASE}\;")
string(APPEND RENODE_COMMANDS "set PROJECT_BASE @${PROJECT_BASE}\;")
string(APPEND RENODE_COMMANDS "set BOARD @${BOARD}\;")
string(APPEND RENODE_COMMANDS "set BOARD_DIR @${BOARD_DIR}\;")
string(APPEND RENODE_COMMANDS
       "set APPLICATION_SOURCE_DIR @${APPLICATION_SOURCE_DIR}\;")
string(APPEND RENODE_COMMANDS "i @${PROJECT_BASE}/renode/plugins.resc\;")

if(EXISTS ${PROJECT_BASE}/testbench/${TESTBENCH}/testbench.resc)
  set(BENCH_COMMANDS
      "include @${PROJECT_BASE}/testbench/${TESTBENCH}/testbench.resc\; s\;")
  string(APPEND BENCH_COMMANDS "startVisual 8000\;")
  # Runs default testbench for the project
  add_custom_target(
    testbench
    COMMAND ${RENODE} --console --disable-xwt -e "${RENODE_COMMANDS}" -e
            "${BENCH_COMMANDS}"
    WORKING_DIRECTORY ${APPLICATION_BINARY_DIR}
    DEPENDS ${APPLICATION_BINARY_DIR}/zephyr/${KERNEL_ELF_NAME}
    USES_TERMINAL)
  add_custom_target(
    testbench_xwt
    COMMAND ${RENODE} -e "${RENODE_COMMANDS}" -e "${BENCH_COMMANDS}"
    WORKING_DIRECTORY ${APPLICATION_BINARY_DIR}
    DEPENDS ${APPLICATION_BINARY_DIR}/zephyr/${KERNEL_ELF_NAME}
    USES_TERMINAL)
  add_custom_target(
    testbench_debugserver
    COMMAND ${RENODE} --console --disable-xwt -e "${RENODE_COMMANDS}\;" -e
            "machine StartGdbServer 3333\;" -e "${BENCH_COMMANDS}"
    WORKING_DIRECTORY ${APPLICATION_BINARY_DIR}
    DEPENDS ${APPLICATION_BINARY_DIR}/zephyr/${KERNEL_ELF_NAME}
    USES_TERMINAL)
  # Add automatic robot script running support Currently only test benches
  # support automatic robot script running
  add_custom_target(
    robotbench
    COMMAND
      PROJECT_BASE=${PROJECT_BASE}
      APPLICATION_BINARY_DIR=${APPLICATION_BINARY_DIR}
      APPLICATION_SOURCE_DIR=${APPLICATION_SOURCE_DIR} BOARD=${BOARD}
      ${RENODE_TEST} --show-log
      ${PROJECT_BASE}/testbench/${TESTBENCH}/testbench.robot
    WORKING_DIRECTORY ${APPLICATION_BINARY_DIR}
    DEPENDS ${APPLICATION_BINARY_DIR}/zephyr/${KERNEL_ELF_NAME}
    USES_TERMINAL)
else()
  add_custom_target(
    testbench
    COMMAND echo "No testbench is configured for this project."
    WORKING_DIRECTORY ${APPLICATION_BINARY_DIR}
    USES_TERMINAL)
  add_custom_target(
    robotbench
    COMMAND echo "No testbench is configured for this project."
    WORKING_DIRECTORY ${APPLICATION_BINARY_DIR}
    USES_TERMINAL)
endif()

if(EXISTS ${BOARD_DIR}/${BOARD}.resc)
  set(BENCH_COMMANDS "include @${BOARD_DIR}/${BOARD}.resc\;")
  add_custom_target(
    boardbench
    COMMAND ${RENODE} --console --disable-xwt -e "${RENODE_COMMANDS}\;" -e
            "${BENCH_COMMANDS}" -e "start\;"
    WORKING_DIRECTORY ${APPLICATION_BINARY_DIR}
    DEPENDS ${APPLICATION_BINARY_DIR}/zephyr/${KERNEL_ELF_NAME}
    USES_TERMINAL)
  add_custom_target(
    boardbench_xwt
    COMMAND ${RENODE} -e "${RENODE_COMMANDS}" -e "${BENCH_COMMANDS}" -e
            "start\;"
    WORKING_DIRECTORY ${APPLICATION_BINARY_DIR}
    DEPENDS ${APPLICATION_BINARY_DIR}/zephyr/${KERNEL_ELF_NAME}
    USES_TERMINAL)
  add_custom_target(
    boardbench_debugserver
    COMMAND ${RENODE} --console --disable-xwt -e "${RENODE_COMMANDS}\;" -e
            "${BENCH_COMMANDS}" -e "machine StartGdbServer 3333\;"
    WORKING_DIRECTORY ${APPLICATION_BINARY_DIR}
    DEPENDS ${APPLICATION_BINARY_DIR}/zephyr/${KERNEL_ELF_NAME}
    USES_TERMINAL)
else()
  add_custom_target(
    boardbench
    COMMAND echo "No board level simulation script defined for this board"
    WORKING_DIRECTORY ${APPLICATION_BINARY_DIR}
    USES_TERMINAL)
endif()

if(EXISTS ${APPLICATION_SOURCE_DIR}/boards/${BOARD}.resc)
  set(BENCH_COMMANDS
      "include @${APPLICATION_SOURCE_DIR}/boards/${BOARD}.resc\;")
  add_custom_target(
    appbench
    COMMAND ${RENODE} --console --disable-xwt -e "${RENODE_COMMANDS}" -e
            "${BENCH_COMMANDS}" -e "start\;"
    WORKING_DIRECTORY ${APPLICATION_BINARY_DIR}
    DEPENDS ${APPLICATION_BINARY_DIR}/zephyr/${KERNEL_ELF_NAME}
    USES_TERMINAL)
  add_custom_target(
    appbench_xwt
    COMMAND ${RENODE} -e "${RENODE_COMMANDS}" -e "${BENCH_COMMANDS}" -e
            "start\;"
    WORKING_DIRECTORY ${APPLICATION_BINARY_DIR}
    DEPENDS ${APPLICATION_BINARY_DIR}/zephyr/${KERNEL_ELF_NAME}
    USES_TERMINAL)
  # starts a debug server but does not start the app (so you can set
  # breakpoints)
  add_custom_target(
    appbench_debugserver
    COMMAND ${RENODE} --console --disable-xwt -e "${RENODE_COMMANDS}\;" -e
            "machine StartGdbServer 3333\;" -e "${BENCH_COMMANDS}"
    WORKING_DIRECTORY ${APPLICATION_BINARY_DIR}
    DEPENDS ${APPLICATION_BINARY_DIR}/zephyr/${KERNEL_ELF_NAME}
    USES_TERMINAL)
else()
  add_custom_target(
    appbench
    COMMAND echo "No application level simulation script defined for this board"
    WORKING_DIRECTORY ${APPLICATION_BINARY_DIR}
    USES_TERMINAL)
endif()

string(APPEND RENODE_COMMANDS "s\;")

add_custom_target(
  run_robot
  COMMAND
    PROJECT_BASE=${PROJECT_BASE}
    APPLICATION_BINARY_DIR=${APPLICATION_BINARY_DIR}
    APPLICATION_SOURCE_DIR=${APPLICATION_SOURCE_DIR} BOARD=${BOARD}
    ${RENODE_TEST} --show-log ${APPLICATION_SOURCE_DIR}/sample.robot
  WORKING_DIRECTORY ${APPLICATION_BINARY_DIR}
  DEPENDS ${APPLICATION_BINARY_DIR}/zephyr/${KERNEL_ELF_NAME}
  USES_TERMINAL)
