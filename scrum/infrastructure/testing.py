#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>

import os
import yaml

PROJECT_ROOT = os.path.realpath(os.path.dirname(os.path.realpath(__file__)) + "/../../")


def check_unity_enabled():
    yaml_file = PROJECT_ROOT + "/west.yml"
    with open(yaml_file, "r") as stream:
        yml = yaml.safe_load(stream)
        unity = next(
            (x for x in yml["manifest"]["projects"] if x["name"] == "unity"), None
        )
        if not unity:
            raise BaseException("You need to add unity project to west.yml")

        expected_unity_path = "modules/test/cmock/vendor/unity"
        if unity["path"] != expected_unity_path:
            raise BaseException("Please place unity under %s" % (expected_unity_path))


def check_cmock_enabled():
    yaml_file = PROJECT_ROOT + "/west.yml"
    with open(yaml_file, "r") as stream:
        yml = yaml.safe_load(stream)
        cmock = next(
            (x for x in yml["manifest"]["projects"] if x["name"] == "cmock"), None
        )
        if not cmock:
            raise BaseException("You need to add cmock project to west.yml")
        expected_cmock_path = "modules/test/cmock"
        if cmock["path"] != expected_cmock_path:
            raise BaseException("Please place cmock under %s" % (expected_cmock_path))
