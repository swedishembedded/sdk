# SPDX-License-Identifier: Apache-2.0

find_program(RENODE renode)

set(RENODE_COMMANDS "")
string(APPEND RENODE_COMMANDS
       "set bin @${APPLICATION_BINARY_DIR}/zephyr/${KERNEL_ELF_NAME}\;")
string(APPEND RENODE_COMMANDS
       "set APPLICATION_BINARY_DIR @${APPLICATION_BINARY_DIR}\;")
string(APPEND RENODE_COMMANDS "set SDK_BASE @${PROJECT_BASE}\;")

if(EXISTS ${APPLICATION_SOURCE_DIR}/simulation/${BOARD}.resc)
  string(APPEND RENODE_COMMANDS
         "include @${APPLICATION_SOURCE_DIR}/simulation/${BOARD}.resc\;")
elseif(EXISTS ${BOARD_DIR}/${BOARD}.resc)
  string(APPEND RENODE_COMMANDS "include @${BOARD_DIR}/${BOARD}.resc\;")
endif()

string(APPEND RENODE_COMMANDS "s\;")

message(WARNING "${RENODE_COMMANDS}")
add_custom_target(
  sim
  COMMAND ${RENODE} --console --disable-xwt -e "${RENODE_COMMANDS}"
  WORKING_DIRECTORY ${APPLICATION_BINARY_DIR}
  DEPENDS ${APPLICATION_BINARY_DIR}/zephyr/${KERNEL_ELF_NAME}
  USES_TERMINAL)

add_custom_target(
  debugserver-sim
  COMMAND ${RENODE} --console --disable-xwt -e
          "${RENODE_COMMANDS}\;machine StartGdbServer 3333\;"
  WORKING_DIRECTORY ${APPLICATION_BINARY_DIR}
  DEPENDS ${APPLICATION_BINARY_DIR}/zephyr/${KERNEL_ELF_NAME}
  USES_TERMINAL)
