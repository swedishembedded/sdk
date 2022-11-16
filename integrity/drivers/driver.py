# SPDX-License-Identifier: Apache-2.0
# Copyright 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

import os
import subprocess
import clang.cindex
from clang.cindex import CursorKind
from clang.cindex import LinkageKind
from robot.libraries.BuiltIn import BuiltIn

ROOT_DIR = os.path.realpath(os.path.dirname(os.path.realpath(__file__)) + "/../../")


def driver_check_data_is_private():
    DRIVER_TYPE = BuiltIn().get_variable_value("${DRIVER_TYPE}")
    DRIVER_NAME = BuiltIn().get_variable_value("${DRIVER_NAME}")

    config_found = False
    data_found = False

    def walk(node):
        nonlocal config_found
        nonlocal data_found

        # logger.console("%s %s" % (node.kind, node.spelling))
        if node.kind == CursorKind.STRUCT_DECL and node.spelling == DRIVER_NAME:
            data_found = True
        if node.kind == CursorKind.STRUCT_DECL and node.spelling == (
            DRIVER_NAME + "_config"
        ):
            config_found = True

    index = clang.cindex.Index.create()
    tu = index.parse(ROOT_DIR + "/drivers/" + DRIVER_TYPE + "/" + DRIVER_NAME + ".c")

    for c in tu.cursor.get_children():
        walk(c)

    if not config_found:
        raise BaseException(
            "Driver should have a config struct inside the file named struct "
            + DRIVER_NAME
            + "_config"
        )
    if not data_found:
        raise BaseException(
            "Driver should have a data struct inside the file named struct "
            + DRIVER_NAME
        )


def driver_has_static_function(name):
    DRIVER_TYPE = BuiltIn().get_variable_value("${DRIVER_TYPE}")
    DRIVER_NAME = BuiltIn().get_variable_value("${DRIVER_NAME}")

    name = "_" + DRIVER_NAME + "_" + name
    success = False

    def walk(node):
        nonlocal success
        if (
            node.kind == CursorKind.FUNCTION_DECL
            and node.linkage == LinkageKind.INTERNAL
            and node.spelling == name
        ):
            success = True

    index = clang.cindex.Index.create()
    tu = index.parse(ROOT_DIR + "/drivers/" + DRIVER_TYPE + "/" + DRIVER_NAME + ".c")

    for c in tu.cursor.get_children():
        walk(c)

    if not success:
        raise BaseException("Driver should have a static function '%s'" % (name))


def driver_check_supports_multi_instance():
    DRIVER_TYPE = BuiltIn().get_variable_value("${DRIVER_TYPE}")
    DRIVER_NAME = BuiltIn().get_variable_value("${DRIVER_NAME}")

    success = False

    def walk(node):
        nonlocal success
        if (
            node.kind == CursorKind.FUNCTION_DECL
            and node.spelling == "DT_INST_FOREACH_STATUS_OKAY"
        ):
            success = True

    index = clang.cindex.Index.create()
    tu = index.parse(ROOT_DIR + "/drivers/" + DRIVER_TYPE + "/" + DRIVER_NAME + ".c")

    for c in tu.cursor.get_children():
        walk(c)

    if not success:
        raise BaseException(
            "Driver should support multi-instance through DT_INST_FOREACH_STATUS_OKAY"
        )


def driver_check_compatible(text):
    DRIVER_TYPE = BuiltIn().get_variable_value("${DRIVER_TYPE}")
    DRIVER_NAME = BuiltIn().get_variable_value("${DRIVER_NAME}")

    src = ROOT_DIR + "/drivers/" + DRIVER_TYPE + "/" + DRIVER_NAME + ".c"
    text = "DT_DRV_COMPAT .*" + text
    proc = subprocess.run(["grep", "-Ri", text, src], capture_output=True)
    if proc.returncode != 0:
        raise BaseException('Page %s does not contain text "%s"' % (src, text))


def driver_name_to_camel_case(text):
    return text.replace("_", " ").title().replace(" ", "")
