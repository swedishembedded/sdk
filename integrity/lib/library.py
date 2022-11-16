# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

import os
import clang.cindex
from clang.cindex import CursorKind
from clang.cindex import LinkageKind
from robot.libraries.BuiltIn import BuiltIn

ROOT_DIR = os.path.realpath(os.path.dirname(os.path.realpath(__file__)) + "/../../")


def module_has_static_function(name):
    LIBRARY = BuiltIn().get_variable_value("${LIBRARY}")
    MODULE = BuiltIn().get_variable_value("${MODULE}")

    name = "_" + MODULE + "_" + name
    success = False

    def walk(node, typename):
        nonlocal success
        if (
            node.kind == CursorKind.FUNCTION_DECL
            and node.linkage == LinkageKind.INTERNAL
            and node.spelling == name
        ):
            success = True

    index = clang.cindex.Index.create()
    tu = index.parse(ROOT_DIR + "/lib/" + LIBRARY + "/" + MODULE + ".c")

    for c in tu.cursor.get_children():
        walk(c, "struct example_sensor")

    if not success:
        raise BaseException(
            "Library source %s should have a static function '%s'" % (MODULE, name)
        )


def module_has_public_function(name):
    LIBRARY = BuiltIn().get_variable_value("${LIBRARY}")
    MODULE = BuiltIn().get_variable_value("${MODULE}")

    name = MODULE + "_" + name
    success = False

    def walk(node, typename):
        nonlocal success
        if (
            node.kind == CursorKind.FUNCTION_DECL
            and node.linkage == LinkageKind.EXTERNAL
            and node.spelling == name
        ):
            success = True

    index = clang.cindex.Index.create()
    tu = index.parse(ROOT_DIR + "/lib/" + LIBRARY + "/" + MODULE + ".c")

    for c in tu.cursor.get_children():
        walk(c, "struct example_sensor")

    if not success:
        raise BaseException(
            "Library source %s should have a public function '%s'" % (MODULE, name)
        )
