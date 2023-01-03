#!/usr/bin/python3
# SPDX-License-Identifier: Apache-2.0
# Copyright 2022 Martin Schröder <info@swedishembedded.com>

import sys

# Add zephyr west command path so that we can rebuild
sys.path.insert(1, "../zephyr/scripts/west_commands")

import pathlib
import subprocess

from build import Build
from textwrap import dedent
from west.commands import WestCommand  # your extension must subclass this
from west import log  # use this for user output
from west.configuration import config
from run_common import get_build_dir, rebuild


class Simulate(WestCommand):
    def __init__(self):
        super(Simulate, self).__init__(
            "simulate",
            "start renode simulation using most recently built firmware",
            # self.description:
            dedent(
                """
                This is a custom simulation command that is more flexible
                than existing SUPPORTED_EMU option for simulating on renode"""
            ),
        )
        self.build = Build()

    def do_add_parser(self, parser_adder):
        parser = parser_adder.add_parser(
            self.name, help=self.help, description=self.description
        )

        parser.add_argument("-s", "--source_dir", help="an optional argument")
        parser.add_argument("-d", "--build_dir", help="build directory")
        parser.add_argument(
            "-b",
            "--board",
            default="",
            help="platform for simulation (boards/<platform>/<config>.resc)",
        )
        parser.add_argument(
            "-c",
            "--config",
            default="default",
            help="configuration to simulate (boards/<platform>/<config>.resc)",
        )
        parser.add_argument("app", help="application to simulate")
        return parser  # gets stored as self.parser

    def do_run(self, args, unknown_args):
        build_dir = get_build_dir(args)
        rebuild(self, build_dir, args)
        run_base = ["renode"]
        run_base.extend(
            [str(pathlib.Path("boards") / args.board / str(args.config + ".resc"))]
        )
        subprocess.check_call(run_base)
