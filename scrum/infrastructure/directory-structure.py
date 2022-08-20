#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>
# SPDX-License-Identifier: Apache-2.0
# Copyright 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

import os
import yaml
from robot.libraries.BuiltIn import BuiltIn

PROJECT_ROOT = os.path.realpath(os.path.dirname(os.path.realpath(__file__)) + "/../../")


def get_app_name():
    APP = BuiltIn().get_variable_value("${APP_NAME}")
    if not APP:
        raise BaseException("APP_NAME variable not defined!")
    return APP


def app_title_should_be(title):
    app = get_app_name()
    yaml_file = PROJECT_ROOT + "/" + app + "/sample.yaml"
    with open(yaml_file, "r") as stream:
        yml = yaml.safe_load(stream)
        # We satisfy this by extending the pipeline
        if "sample" not in yml or "name" not in yml["sample"]:
            raise BaseException(
                "YAML file (%s) needs to have name of the sample" % (yaml_file)
            )
        if yml["sample"]["name"] != title:
            raise BaseException(
                "Application title for app '%s' must be '%s'" % (app, title)
            )


def app_should_have_release_build():
    app = get_app_name()
    yaml_file = PROJECT_ROOT + "/" + app + "/sample.yaml"
    with open(yaml_file, "r") as stream:
        yml = yaml.safe_load(stream)
        # We satisfy this by extending the pipeline
        test_id = app.replace("/", ".") + ".release"
        if "tests" not in yml or test_id not in yml["tests"]:
            raise BaseException(
                "Application '%s' does not have a release build (%s) defined in sample.yaml!"
                % (app, test_id)
            )


def app_check_integration_platform(platform):
    app = get_app_name()
    yaml_file = PROJECT_ROOT + "/" + app + "/sample.yaml"
    with open(yaml_file, "r") as stream:
        yml = yaml.safe_load(stream)
        # We satisfy this by extending the pipeline
        test_id = app.replace("/", ".") + ".release"
        if "tests" not in yml or test_id not in yml["tests"]:
            raise BaseException(
                "Application '%s' does not have a release build (%s) defined in sample.yaml!"
                % (app, test_id)
            )
        if platform not in yml["tests"][test_id]["integration_platforms"]:
            raise BaseException(
                "Application '%s' does not have '%s' among integration platforms."
                % (app, platform)
            )


def check_test_configuration():
    TEST_NAME = BuiltIn().get_variable_value("${TEST_NAME}")
    yaml_file = PROJECT_ROOT + "/tests/" + TEST_NAME + "/testcase.yaml"
    with open(yaml_file, "r") as stream:
        yml = yaml.safe_load(stream)
        if "tests" not in yml:
            raise BaseException(
                "Test '%s' does not have 'tests' section in testcase.yaml file"
                % (TEST_NAME)
            )
        # We satisfy this by extending the pipeline
        test_id = TEST_NAME.replace("/", ".") + "unit"
        source_file = TEST_NAME.strip("/") + ".c"
        if test_id not in yml["tests"]:
            raise BaseException(
                "Test '%s' does not have a unit test (%s) defined in testcase.yaml!"
                % (TEST_NAME, test_id)
            )
        if source_file not in yml["tests"][test_id]["tags"]:
            raise BaseException(
                "Test '%s' does not have tag '%s'" % (TEST_NAME, source_file)
            )

        test_id = TEST_NAME.replace("/", ".") + "integration"
        if test_id not in yml["tests"]:
            raise BaseException(
                "Test '%s' does not have an integration test (%s) defined in testcase.yaml!"
                % (TEST_NAME, test_id)
            )
        if source_file not in yml["tests"][test_id]["tags"]:
            raise BaseException(
                "Test '%s' does not have tag '%s'" % (TEST_NAME, source_file)
            )
