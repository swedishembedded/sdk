# SPDX-License-Identifier: Apache-2.0
# ~~~
# Copyright (c) 2022 Martin Schröder
# Consulting: https://swedishembedded.com/go
# Training: https://swedishembedded.com/tag/training
# ~~~

cmake_minimum_required(VERSION 3.20.0)
project(doc LANGUAGES NONE)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE} COMPONENTS doc)

set(SPHINXOPTS_DEFAULT
    "-j auto"
    CACHE STRING "Default Sphinx Options")
set(LATEXMKOPTS
    "-halt-on-error -no-shell-escape"
    CACHE STRING "Default latexmk options")
separate_arguments(SPHINXOPTS_DEFAULT UNIX_COMMAND ${SPHINXOPTS_DEFAULT})
separate_arguments(SPHINXOPTS_EXTRA UNIX_COMMAND ${SPHINXOPTS_EXTRA})
separate_arguments(LATEXMKOPTS UNIX_COMMAND ${LATEXMKOPTS})

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

# Add a documentation target
function(add_doc_target name)
  add_custom_target(
    ${name}
    ${ARGN}
    COMMENT "Building ${name}")
  add_custom_target(
    ${name}-all
    ${ARGN}
    COMMENT "Building ${name}-all")
endfunction()

# Add a docset
function(add_docset name)
  cmake_parse_arguments(DOCSET "DODGY" "" "" ${ARGN})

  set(docset_cfg_dir ${CMAKE_CURRENT_LIST_DIR}/${name})
  set(docset_build_dir ${CMAKE_BINARY_DIR}/${name})
  set(docset_src_dir ${CMAKE_BINARY_DIR}/${name}/src)
  set(docset_doctree_dir ${CMAKE_BINARY_DIR}/${name}/doctree)
  set(docset_html_dir ${CMAKE_BINARY_DIR}/html/${name})
  set(docset_latex_dir ${CMAKE_BINARY_DIR}/latex/${name})
  set(docset_make_dirs ${docset_build_dir};${docset_src_dir})
  set(docset_clean_dirs ${docset_build_dir};${docset_html_dir})
  set(docset_env DOXYGEN_EXECUTABLE=${DOXYGEN_EXECUTABLE})

  if(NOT ${DOCSET_DODGY})
    set(extra_args ${SPHINXOPTS_EXTRA})
  endif()

  add_doc_target(
    ${name}-inventory
    COMMAND
    ${CMAKE_COMMAND}
    -E
    make_directory
    ${docset_make_dirs}
    COMMAND
    ${CMAKE_COMMAND}
    -E
    env
    ${docset_env}
    ${SPHINXBUILD}
    -b
    inventory
    -c
    ${docset_cfg_dir}
    -d
    ${docset_doctree_dir}
    -w
    ${docset_build_dir}/inventory.log
    ${SPHINXOPTS_DEFAULT}
    ${docset_src_dir}
    ${docset_html_dir}
    USES_TERMINAL
    COMMENT
    "Building ${name} inventory...")

  add_doc_target(
    ${name}
    COMMAND
    ${CMAKE_COMMAND}
    -E
    make_directory
    ${docset_make_dirs}
    COMMAND
    ${CMAKE_COMMAND}
    -E
    env
    ${docset_env}
    ${SPHINXBUILD}
    -b
    html
    -c
    ${docset_cfg_dir}
    -d
    ${docset_doctree_dir}
    -w
    ${docset_build_dir}/html.log
    ${SPHINXOPTS_DEFAULT}
    ${extra_args}
    ${docset_src_dir}
    ${docset_html_dir}
    USES_TERMINAL
    COMMENT
    "Building ${name} docset...")

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
    ${docset_cfg_dir}
    -d
    ${docset_doctree_dir}
    -w
    ${docset_build_dir}/latex.log
    -t
    svgconvert
    ${SPHINXOPTS}
    ${docset_src_dir}
    ${docset_latex_dir}
    USES_TERMINAL
    COMMENT
    "Running Sphinx LaTeX build...")

  set_target_properties(
    ${name}-latex
    PROPERTIES ADDITIONAL_CLEAN_FILES
               "${docset_src_dir};${docset_latex_dir};${docset_doctree_dir}")

  if(LATEX_PDFLATEX_FOUND AND LATEXMK)
    find_program(MAKE make)
    if(NOT MAKE)
      message(FATAL_ERROR "The 'make' command was not found")
    endif()
    set(pdf_build_command ${MAKE})

    add_custom_target(
      ${name}-pdf
      COMMAND ${CMAKE_COMMAND} -E env LATEXMKOPTS="${LATEXMKOPTS}"
              ${pdf_build_command}
      WORKING_DIRECTORY ${docset_latex_dir}
      COMMENT "Building PDF file..."
      USES_TERMINAL)

    add_dependencies(${name}-pdf ${name}-latex)
  endif()

  add_custom_target(
    ${name}-linkcheck
    COMMAND ${CMAKE_COMMAND} -E make_directory ${docset_make_dirs}
    COMMAND
      ${CMAKE_COMMAND} -E env ${docset_env} ${SPHINXBUILD} -b linkcheck -c
      ${docset_cfg_dir} -d ${docset_doctree_dir} -w
      ${docset_build_dir}/linkcheck.log ${SPHINXOPTS_DEFAULT} ${extra_args}
      ${docset_src_dir} ${docset_build_dir}
    USES_TERMINAL
    COMMENT "Checking links for ${name} docset...")

  set_target_properties(
    ${name}-inventory ${name}-inventory-all ${name} ${name}-all
    ${name}-linkcheck PROPERTIES ADDITIONAL_CLEAN_FILES "${docset_clean_dirs}")

  add_custom_target(
    ${name}-clean
    COMMENT "Cleaning the build"
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${docset_clean_dirs})
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

add_docset(testing)

add_custom_target(
  copy-content
  COMMENT "Copying content"
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_LIST_DIR}/_static/html/index.html ${HTML_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/versions.json
          ${HTML_DIR})

add_custom_target(
  merge-search-indexes
  COMMAND ${PYTHON_EXECUTABLE} ${SWE_BASE}/doc/_scripts/merge_search_indexes.py
          -b ${CMAKE_BINARY_DIR}
  COMMENT "Merging search indexes...")

add_custom_target(pdf ALL COMMENT "Building all PDFs")
add_dependencies(
  pdf
  copy-content
  merge-search-indexes
  sdk-pdf
  mcuboot-pdf
  # zephyr-pdf
  develop-pdf
  userspace-pdf
  # renode-docs-pdf
)

add_custom_target(build-all ALL COMMENT "Building all docs")
add_dependencies(
  build-all
  copy-content
  merge-search-indexes
  sdk-all
  mcuboot-all
  zephyr-all
  develop-all
  userspace-all
  renode-docs-all
  testing-all)
