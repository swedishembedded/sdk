# Copyright (c) 2022 Martin Schröder
# SPDX-License-Identifier: Apache-2.0
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training

cmake_minimum_required(VERSION 3.20.0)
project(doc LANGUAGES NONE)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE} COMPONENTS doc)

set(SPHINXOPTS_DEFAULT
    "-j auto"
    CACHE STRING "Default Sphinx Options")
set(LATEXMKOPTS
    "-halt-on-error -no-shell-escape"
    CACHE STRING "Default latexmk options")
separate_arguments(SPHINXOPTS_DEFAULT)
separate_arguments(SPHINXOPTS_EXTRA)
separate_arguments(LATEXMKOPTS)

# Set Swedish Embedded SDK base
get_filename_component(SWE_BASE ${CMAKE_CURRENT_LIST_DIR}../ DIRECTORY)

find_package(PythonInterp 3.4)
find_package(Doxygen 1.8.17 REQUIRED)

find_program(SPHINXBUILD sphinx-build)
if(NOT SPHINXBUILD)
  message(FATAL_ERROR "'sphinx-build' command not found")
endif()

find_package(LATEX COMPONENTS PDFLATEX)
find_program(LATEXMK latexmk)
if(NOT LATEX_PDFLATEX_FOUND OR NOT LATEXMK)
  message(
    WARNING "LaTeX components not found. PDF build will not be available.")
endif()

find_program(WEST west)
if(NOT WEST)
  message(FATAL_ERROR "'west' command was not found")
endif()

set(SWE_BINARY_DIR ${CMAKE_BINARY_DIR}/swe)
set(ZEPHYR_BINARY_DIR ${CMAKE_BINARY_DIR}/zephyr)

set(KCONFIG_BINARY_DIR ${CMAKE_BINARY_DIR}/kconfig)
list(INSERT MODULE_EXT_ROOT 0 ${ZEPHYR_BASE})
file(MAKE_DIRECTORY ${KCONFIG_BINARY_DIR})

set(HTML_DIR ${CMAKE_BINARY_DIR}/html)
set(LATEX_DIR ${CMAKE_BINARY_DIR}/latex)
file(MAKE_DIRECTORY ${HTML_DIR})

message(STATUS "Zephyr base: ${ZEPHYR_BASE}")
message(STATUS "SDK base: ${SWE_BASE}")

add_custom_target(
  zephyr-devicetree
  COMMAND
    ${CMAKE_COMMAND} -E env
    PYTHONPATH=${ZEPHYR_BASE}/scripts/dts/python-devicetree/src
    ZEPHYR_BASE=${ZEPHYR_BASE} ${PYTHON_EXECUTABLE}
    ${ZEPHYR_BASE}/doc/_scripts/gen_devicetree_rest.py --vendor-prefixes
    ${ZEPHYR_BASE}/dts/bindings/vendor-prefixes.txt --dts-root ${ZEPHYR_BASE}
    ${ZEPHYR_BINARY_DIR}/src/build/dts/api
  VERBATIM USES_TERMINAL
  COMMENT "Generating Devicetree bindings documentation...")

function(add_doc_target name)
  add_custom_target(${name} ${ARGN})
  add_custom_target(${name}-all ${ARGN})
endfunction()

function(add_docset name)
  cmake_parse_arguments(DOCSET "DODGY" "" "" ${ARGN})

  set(DOCSET_CFG_DIR ${CMAKE_CURRENT_LIST_DIR}/${name})
  set(DOCSET_BUILD_DIR ${CMAKE_BINARY_DIR}/${name})
  set(DOCSET_SRC_DIR ${CMAKE_BINARY_DIR}/${name}/src)
  set(DOCSET_DOCTREE_DIR ${CMAKE_BINARY_DIR}/${name}/doctree)
  set(DOCSET_HTML_DIR ${CMAKE_BINARY_DIR}/html/${name})
  set(DOCSET_LATEX_DIR ${CMAKE_BINARY_DIR}/latex/${name})
  set(DOCSET_MAKE_DIRS ${DOCSET_BUILD_DIR};${DOCSET_SRC_DIR})
  set(DOCSET_CLEAN_DIRS ${DOCSET_BUILD_DIR};${DOCSET_HTML_DIR})
  set(DOCSET_ENV DOXYGEN_EXECUTABLE=${DOXYGEN_EXECUTABLE})

  if(NOT ${DOCSET_DODGY})
    set(EXTRA_ARGS ${SPHINXOPTS_EXTRA})
  endif()

  add_doc_target(
    ${name}-inventory
    COMMAND
    ${CMAKE_COMMAND}
    -E
    make_directory
    ${DOCSET_MAKE_DIRS}
    COMMAND
    ${CMAKE_COMMAND}
    -E
    env
    ${DOCSET_ENV}
    ${SPHINXBUILD}
    -b
    inventory
    -c
    ${DOCSET_CFG_DIR}
    -d
    ${DOCSET_DOCTREE_DIR}
    -w
    ${DOCSET_BUILD_DIR}/inventory.log
    ${SPHINXOPTS_DEFAULT}
    ${DOCSET_SRC_DIR}
    ${DOCSET_HTML_DIR}
    USES_TERMINAL
    COMMENT
    "Building ${name} inventory...")

  add_doc_target(
    ${name}
    COMMAND
    ${CMAKE_COMMAND}
    -E
    make_directory
    ${DOCSET_MAKE_DIRS}
    COMMAND
    ${CMAKE_COMMAND}
    -E
    env
    ${DOCSET_ENV}
    ${SPHINXBUILD}
    -b
    html
    -c
    ${DOCSET_CFG_DIR}
    -d
    ${DOCSET_DOCTREE_DIR}
    -w
    ${DOCSET_BUILD_DIR}/html.log
    ${SPHINXOPTS_DEFAULT}
    ${EXTRA_ARGS}
    ${DOCSET_SRC_DIR}
    ${DOCSET_HTML_DIR}
    USES_TERMINAL
    COMMENT
    "Building ${name} docset...")

  # -------------------------------------------------------------------------------
  # pdf

  add_doc_target(
    ${name}-latex
    COMMAND
    ${CMAKE_COMMAND}
    -E
    env
    ${SPHINX_ENV}
    ${SPHINXBUILD}
    -b
    latex
    -c
    ${DOCSET_CFG_DIR}
    -d
    ${DOCSET_DOCTREE_DIR}
    -w
    ${DOCSET_BUILD_DIR}/latex.log
    -t
    svgconvert
    ${SPHINXOPTS}
    ${DOCSET_SRC_DIR}
    ${DOCSET_LATEX_DIR}
    USES_TERMINAL
    COMMENT
    "Running Sphinx LaTeX build...")

  set_target_properties(
    ${name}-latex
    PROPERTIES ADDITIONAL_CLEAN_FILES
               "${DOCSET_SRC_DIR};${DOCSET_LATEX_DIR};${DOCSET_DOCTREE_DIR}")

  if(LATEX_PDFLATEX_FOUND AND LATEXMK)
    find_program(MAKE make)
    if(NOT MAKE)
      message(FATAL_ERROR "The 'make' command was not found")
    endif()
    set(PDF_BUILD_COMMAND ${MAKE})

    add_custom_target(
      ${name}-pdf
      COMMAND ${CMAKE_COMMAND} -E env LATEXMKOPTS="${LATEXMKOPTS}"
              ${PDF_BUILD_COMMAND}
      WORKING_DIRECTORY ${DOCSET_LATEX_DIR}
      COMMENT "Building PDF file..."
      USES_TERMINAL)

    add_dependencies(${name}-pdf ${name}-latex)
  endif()

  add_custom_target(
    ${name}-linkcheck
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DOCSET_MAKE_DIRS}
    COMMAND
      ${CMAKE_COMMAND} -E env ${DOCSET_ENV} ${SPHINXBUILD} -b linkcheck -c
      ${DOCSET_CFG_DIR} -d ${DOCSET_DOCTREE_DIR} -w
      ${DOCSET_BUILD_DIR}/linkcheck.log ${SPHINXOPTS_DEFAULT} ${EXTRA_ARGS}
      ${DOCSET_SRC_DIR} ${DOCSET_BUILD_DIR}
    USES_TERMINAL
    COMMENT "Checking links for ${name} docset...")

  set_target_properties(
    ${name}-inventory ${name}-inventory-all ${name} ${name}-all
    ${name}-linkcheck PROPERTIES ADDITIONAL_CLEAN_FILES "${DOCSET_CLEAN_DIRS}")

  add_custom_target(${name}-clean COMMAND ${CMAKE_COMMAND} -E rm -rf
                                          ${DOCSET_CLEAN_DIRS})
endfunction()

add_docset(zephyr DODGY)
add_dependencies(zephyr zephyr-devicetree)
add_dependencies(zephyr-latex zephyr-devicetree)
add_dependencies(zephyr-all zephyr-devicetree)

add_docset(sdk)

add_docset(renode-docs)

add_docset(mcuboot)

add_docset(develop)

add_docset(userspace)

add_custom_target(
  copy-content
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_LIST_DIR}/_static/html/index.html ${HTML_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/versions.json
          ${HTML_DIR})

add_custom_target(
  merge-search-indexes
  COMMAND ${PYTHON_EXECUTABLE} ${SWE_BASE}/doc/_scripts/merge_search_indexes.py
          -b ${CMAKE_BINARY_DIR}
  COMMENT "Merging search indexes...")

add_custom_target(pdf ALL)
add_dependencies(
  pdf
  copy-content
  merge-search-indexes
  sdk-pdf
  mcuboot-pdf
  zephyr-pdf
  develop-pdf
  userspace-pdf
  # renode-docs-pdf
)

add_custom_target(build-all ALL)
add_dependencies(
  build-all
  copy-content
  merge-search-indexes
  sdk-all
  mcuboot-all
  zephyr-all
  develop-all
  userspace-all
  renode-docs-all)
