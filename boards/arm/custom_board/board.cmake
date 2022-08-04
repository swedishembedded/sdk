# SPDX-License-Identifier: Apache-2.0

board_runner_args(jlink "--device=STM32F401RE" "--speed=4000")

include(${ZEPHYR_BASE}/boards/common/openocd.board.cmake)
include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)

# specify which platforms are supported
set(SUPPORTED_EMU_PLATFORMS renode)
# specify default platform for "run" command
set(EMU_PLATFORM renode)
# add renode script for "run" command
if(EXISTS ${APPLICATION_SOURCE_DIR}/simulation/${BOARD}.resc)
  set(RENODE_SCRIPT ${APPLICATION_SOURCE_DIR}/simulation/${BOARD}.resc)
elseif(EXISTS ${CMAKE_CURRENT_LIST_DIR}/${BOARD}.resc)
  set(RENODE_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/${BOARD}.resc)
endif()
