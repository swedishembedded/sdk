# SPDX-License-Identifier: Apache-2.0
# Copyright 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

*** Keywords ***

Chapter follows board reference structure
	Chapter has an overview
	Chapter has a specification of supported hardware
	Chapter covers programming
	Chapter covers debugging
	Chapter covers testing
	Chapter lists hardware references

Chapter has an overview
	File Should Exist  ${CHAPTER_FILES}/overview.rst

Chapter has a specification of supported hardware
	File Should Exist  ${CHAPTER_FILES}/hardware.rst

Chapter covers programming
	File Should Exist  ${CHAPTER_FILES}/programming.rst

Chapter covers debugging
	File Should Exist  ${CHAPTER_FILES}/debugging.rst

Chapter covers testing
	File Should Exist  ${CHAPTER_FILES}/testing.rst

Chapter lists hardware references
	File Should Exist  ${CHAPTER_FILES}/references.rst
