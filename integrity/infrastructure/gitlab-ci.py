#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2022 Martin Schröder <info@swedishembedded.com>
# SPDX-License-Identifier: Apache-2.0
# Copyright 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

import os
import yaml

PROJECT_ROOT = os.path.realpath(os.path.dirname(os.path.realpath(__file__)) + "/../../")


def check_job_exists(job):
    with open(PROJECT_ROOT + "/.gitlab-ci.yml", "r") as stream:
        yml = yaml.safe_load(stream)
        # We satisfy this by running robot command on scrum folder
        if job not in yml:
            raise BaseException("Job %s does not exist in gitlab ci file!" % (job))
