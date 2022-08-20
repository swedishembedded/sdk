# SPDX-License-Identifier: Apache-2.0
# Copyright 2022 Martin Schröder <info@swedishembedded.com>
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

*** Keywords ***

Chapter follows guide structure
	Chapter has a small introduction
	Chapter covers prerequisite knowledge
	Chapter covers goal of this chapter
	Chapter covers action steps for achieving the goal
	Chapter covers pitfalls along the way
	Chapter covers what has been achieved
	Chapter has an offer at the end

Chapter covers prerequisite knowledge
	File Should Exist  ${CHAPTER_FILES}/prerequisites.rst

Chapter covers goal of this chapter
	File Should Exist  ${CHAPTER_FILES}/what-we-will-do.rst

Chapter covers action steps for achieving the goal
	File Should Exist  ${CHAPTER_FILES}/implementation.rst

Chapter covers pitfalls along the way
	File Should Exist  ${CHAPTER_FILES}/pitfalls.rst

Chapter covers what has been achieved
	File Should Exist  ${CHAPTER_FILES}/what-we-have-done.rst

Chapter has an offer at the end
	File Should Exist  ${CHAPTER_FILES}/offer.rst
