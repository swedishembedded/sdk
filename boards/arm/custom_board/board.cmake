# SPDX-License-Identifier: Apache-2.0
# Copyright Nordic Semiconductor
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

board_runner_args(jlink "--device=STM32F401RE" "--speed=4000")

include(${ZEPHYR_BASE}/boards/common/openocd.board.cmake)
include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)

set(SUPPORTED_EMU_PLATFORMS robotbench)
