#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

import os
import yaml

PROJECT_ROOT = os.path.realpath(os.path.dirname(os.path.realpath(__file__)) + "/../../")


def check_unit_testing_module_enabled():
    yaml_file = PROJECT_ROOT + "/west.yml"
    with open(yaml_file, "r") as stream:
        yml = yaml.safe_load(stream)
        unity = next(
            (x for x in yml["manifest"]["projects"] if x["name"] == "testing"), None
        )
        if not unity:
            raise BaseException("You need to add unity project to west.yml")

        expected_testing_path = "modules/test/testing"
        if unity["path"] != expected_testing_path:
            raise BaseException(
                "Please place testing repo under %s" % (expected_testing_path)
            )
