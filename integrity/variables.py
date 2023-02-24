#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

import os
import pathlib

PROJECT_ROOT = os.getcwd()
SWEDISH_EMBEDDED_SDK = pathlib.Path(
    os.path.realpath(os.path.dirname(os.path.realpath(__file__)) + "/../")
)
ROOT_DIR = PROJECT_ROOT
INTEGRITY_DIR = ROOT_DIR + "/integrity/"

VERSION_PARTS = {
    k: int(v) for k, v in (l.split("=") for l in open(ROOT_DIR + "/VERSION"))
}
try:
    VERSION_MAJOR = VERSION_PARTS["VERSION_MAJOR"]
    VERSION_MINOR = VERSION_PARTS["VERSION_MINOR"]
    VERSION_PATCH = VERSION_PARTS["VERSION_PATCH"]
    VERSION = str(VERSION_MAJOR) + "." + str(VERSION_MINOR) + "." + str(VERSION_PATCH)
except:
    print("Failed to parse version.")
