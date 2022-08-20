# SPDX-License-Identifier: Apache-2.0
# Copyright 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

add_subdirectory_ifdef(CONFIG_UNITY unity)
add_subdirectory_ifdef(CONFIG_CMOCK cmock)
