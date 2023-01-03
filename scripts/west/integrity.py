#!/usr/bin/python3
# SPDX-License-Identifier: Apache-2.0
# Copyright 2022 Martin Schröder <info@swedishembedded.com>

import sys

# Add zephyr west command path so that we can rebuild
sys.path.insert(1, "../zephyr/scripts/west_commands")
sys.path.insert(1, "../sdk/integrity")

from variables import SWEDISH_EMBEDDED_SDK, PROJECT_ROOT
import pathlib
import subprocess

from build import Build
from textwrap import dedent
from west.commands import WestCommand
from west import log
from west.configuration import config
from run_common import get_build_dir, rebuild

class Integrity(WestCommand):
    def __init__(self):
        super(Integrity, self).__init__(
            "integrity",
            "checks integrity of the entire application repo",
            dedent(
                """
                Runs Swedish Embedded SDK integrity checks as well as local
                repository integrity checks.
                """
            ),
        )
        self.build = Build()

    def do_add_parser(self, parser_adder):
        parser = parser_adder.add_parser(
            self.name, help=self.help, description=self.description
        )
        parser.add_argument("-f", "--fix", help="fix errors discovered")
        return parser  # gets stored as self.parser

    def do_run(self, args, unknown_args):
        run_base = ["robot"]
        run_base.extend(["-V", str(pathlib.Path("integrity") / "variables.py")])
        run_base.extend([str(pathlib.Path("..") / "sdk" / "integrity" / "directory-structure" / "basic.robot")])
        subprocess.check_call(run_base)

        run_base = [str(pathlib.Path(PROJECT_ROOT) / "scripts" / "check")]
        run_base.extend(["-c", "origin/main..HEAD"])
        subprocess.check_call(run_base)
