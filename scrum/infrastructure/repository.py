#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>

import os
import re


def check_merge_request_title():
    title = os.getenv("CI_MERGE_REQUEST_TITLE")
    regex = r"^Issue \#[0-9]+: [a-zA-Z0-9_\-\s]+$"
    # Check if title is present and does not match regex
    if title and not re.match(regex, title):
        raise BaseException(
            "Merge request title (%s) does not match format (%s)" % (title, regex)
        )
