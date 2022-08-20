# SPDX-License-Identifier: Apache-2.0
# Copyright 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

*** Keywords ***

Chapter follows reference structure
	Chapter has a small introduction
	Chapter has summary of main features
	Chapter has functional description
	Chapter has usage examples
	Chapter has doxygen reference

Chapter has a small introduction
	File Should Exist  ${CHAPTER_FILES}/what-is-this.rst

Chapter has summary of main features
	File Should Exist  ${CHAPTER_FILES}/main-features.rst

Chapter has functional description
	File Should Exist  ${CHAPTER_FILES}/description.rst

Chapter has usage examples
	File Should Exist  ${CHAPTER_FILES}/usage.rst

Chapter has doxygen reference
	File Should Exist  ${CHAPTER_FILES}/api-reference.rst
