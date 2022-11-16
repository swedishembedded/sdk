# SPDX-License-Identifier: Apache-2.0
# Copyright 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

*** Settings ***
Library  OperatingSystem
Library  gitlab-ci.py

*** Test Cases ***

Gitlab ci has been setup
	Gitlab task template has been added
	Gitlab CI has all required jobs
	Gitlab repository has merge method set to fast forward merge
	Gitlab repository has mandatory squashing
	Gitlab repository has merge checks enabled
	Gitlab repository has squash commit template
	Gitlab merge request template has been added

*** Keywords ***

Gitlab task template has been added
	File Should Exist  ${CURDIR}/../../.gitlab/issue_templates/task.md

Gitlab merge request template has been added
	File Should Exist  ${CURDIR}/../../.gitlab/merge_request_templates/default.md

Gitlab CI has all required jobs
	Gitlab CI has a check job
	Gitlab CI has a build job
	Gitlab CI has a test job
	Gitlab CI has a build_from_scratch job

Gitlab CI has a ${job} job
	Check job exists  ${job}

Gitlab CI has a fully configured merge request pipeline
	Merge request pipeline runs robot tests

Gitlab repository has merge method set to fast forward merge
	File Should Exist  ${CURDIR}/gitlab-ci/fast-forward-merge.jpg

Gitlab repository has mandatory squashing
	File Should Exist  ${CURDIR}/gitlab-ci/mandatory-squashing.jpg

Gitlab repository has squash commit template
	File Should Exist  ${CURDIR}/gitlab-ci/squash-commit-template.jpg

Gitlab repository has merge checks enabled
	File Should Exist  ${CURDIR}/gitlab-ci/merge-checks.jpg
