# SPDX-License-Identifier: Apache-2.0

import os
from pathlib import Path
import sys


# Paths ------------------------------------------------------------------------

SWE_BASE = Path(__file__).absolute().parents[2]
ZEPHYR_BASE = Path(__file__).absolute().parents[3] / "zephyr"

sys.path.insert(0, str(SWE_BASE / "doc" / "_utils"))
import utils

# ZEPHYR_BASE = utils.get_projdir("zephyr")

# General configuration --------------------------------------------------------

project = "Swedish Embedded SDK"
copyright = "2022 Martin Schröder"
author = "Martin Schröder"
version = release = "0.36.0"

sys.path.insert(0, str(ZEPHYR_BASE / "doc" / "_extensions"))
sys.path.insert(0, str(SWE_BASE / "doc" / "_extensions"))
# sys.path.insert(0, str(SWE_BASE / "doc" / "_theme"))

extensions = [
    # "sphinx.ext.intersphinx",
    "breathe",
    "interbreathe",
    "sphinxcontrib.mscgen",
    "zephyr.application",
    "zephyr.html_redirects",
    "zephyr.warnings_filter",
    # "zephyr.kconfig",
    "zephyr.external_content",
    "zephyr.doxyrunner",
    "zephyr.link-roles",
    "sphinx_tabs.tabs",
    "sphinx_togglebutton",
    "notfound.extension",
]

linkcheck_ignore = [
    # intersphinx links
    r"(\.\.(\\|/))+(zephyr|mcuboot)",
    # link to access local documentation
    "http://localhost:8000/latest/index.html",
    # used as example in doxygen
    "https://google.com:443",
]

linkcheck_anchors_ignore = [r"page="]

rst_epilog = """
"""

rst_prolog = """
.. include:: /consulting.rst
"""

# Options for HTML output ------------------------------------------------------

html_theme = "sphinx_swe_theme"
html_theme_path = [str(SWE_BASE / "doc" / "_theme")]
html_static_path = [
    str(SWE_BASE / "doc" / "_static"),
    str(SWE_BASE / "doc" / "_theme" / "sphinx_swe_theme" / "static"),
]
html_last_updated_fmt = "%b %d, %Y"
html_show_sourcelink = True
html_show_sphinx = False

html_theme_options = {"docset": "sdk", "docsets": utils.ALL_DOCSETS}

# Options for intersphinx ------------------------------------------------------

intersphinx_mapping = dict()

zephyr_mapping = utils.get_intersphinx_mapping("zephyr")
if zephyr_mapping:
    intersphinx_mapping["zephyr"] = zephyr_mapping

renode_mapping = utils.get_intersphinx_mapping("renode")
if renode_mapping:
    intersphinx_mapping["renode"] = renode_mapping

# -- Options for doxyrunner plugin ---------------------------------------------

doxyrunner_doxygen = os.environ.get("DOXYGEN_EXECUTABLE", "doxygen")
doxyrunner_doxyfile = SWE_BASE / "doc" / "sdk" / "sdk.doxyfile.in"
doxyrunner_outdir = utils.get_builddir() / "sdk" / "doxygen"
doxyrunner_fmt = True
doxyrunner_fmt_vars = {
    "SWE_BASE": str(SWE_BASE),
}

# Options for breathe ----------------------------------------------------------

breathe_projects = {"sdk": str(doxyrunner_outdir / "xml")}
breathe_default_project = "sdk"
breathe_domain_by_extension = {"h": "c", "c": "c"}
breathe_separate_member_pages = True

# Options for html_redirect ----------------------------------------------------

html_redirect_pages = []

# -- Options for zephyr.warnings_filter ----------------------------------------

warnings_filter_config = str(SWE_BASE / "doc" / "sdk" / "known-warnings.txt")

# Options for zephyr.link-roles ------------------------------------------------

link_roles_manifest_project = "sdk"
link_roles_manifest_baseurl = "https://github.com/swedishembedded/sdk"

# Options for external_content -------------------------------------------------

external_content_contents = [
    (SWE_BASE / "doc" / "sdk", "*"),
    (SWE_BASE, "boards/**/*.rst"),
    (SWE_BASE, "samples/**/*.rst"),
    (SWE_BASE, "testbench/**/*.rst"),
    (SWE_BASE / "doc", "consulting.rst"),
]
external_content_keep = ["versions.txt"]

# Options for table_from_rows --------------------------------------------------

table_from_rows_base_dir = SWE_BASE
table_from_sample_yaml_board_reference = "/includes/sample_board_rows.txt"

# Options for options_from_kconfig ---------------------------------------------

options_from_kconfig_base_dir = SWE_BASE
options_from_kconfig_zephyr_dir = ZEPHYR_BASE

# -- Options for LaTeX output ---------------------------------------------

latex_elements = {
    "papersize": "a4paper",
    "maketitle": open(SWE_BASE / "doc" / "_static" / "latex" / "title.tex").read(),
    "preamble": open(SWE_BASE / "doc" / "_static" / "latex" / "preamble.tex").read(),
    "fontpkg": r"\usepackage{charter}",
    "sphinxsetup": ",".join(
        (
            # NOTE: colors match those found in light.css stylesheet
            "verbatimwithframe=false",
            "VerbatimColor={HTML}{f0f2f4}",
            "InnerLinkColor={HTML}{2980b9}",
            "warningBgColor={HTML}{e9a499}",
            "warningborder=0pt",
            r"HeaderFamily=\rmfamily\bfseries",
        )
    ),
}
latex_logo = str(SWE_BASE / "doc" / "_static" / "images" / "logo-latex.pdf")
latex_documents = [
    (
        "index-tex",
        "swedishembedded.tex",
        "Swedish Embedded Platform SDK",
        author,
        "manual",
    ),
]

# Options for manifest_revisions_table -----------------------------------------

manifest_revisions_table_manifest = SWE_BASE / "west.yml"

# Options for sphinx_notfound_page ---------------------------------------------

notfound_urls_prefix = "/sdk/doc/{}/sdk/".format(
    "latest" if version.endswith("99") else version
)


def setup(app):
    app.add_css_file("css/zephyr.css")

    utils.add_google_analytics(app)
