# SPDX-License-Identifier: Apache-2.0

from pathlib import Path
import sys

# Paths ------------------------------------------------------------------------

SWE_BASE = Path(__file__).absolute().parents[2]

sys.path.insert(0, str(SWE_BASE / "doc" / "_utils"))
import utils

TESTING_BASE = utils.get_projdir("testing")
CMOCK_BASE = TESTING_BASE.parents[0] / "cmock"
UNITY_BASE = CMOCK_BASE / "vendor" / "unity"
ZEPHYR_BASE = utils.get_projdir("zephyr")

# General configuration --------------------------------------------------------

project = "CMock/Unity"
copyright = "2019-2022, Martin Schröder"
version = release = "0.34.0"

sys.path.insert(0, str(ZEPHYR_BASE / "doc" / "_extensions"))
sys.path.insert(0, str(SWE_BASE / "doc" / "_extensions"))

extensions = [
    "sphinx.ext.intersphinx",
    "recommonmark",
    "zephyr.external_content",
]
source_suffix = [".rst", ".md"]
# master_doc = "wrapper"

linkcheck_ignore = [r"(\.\.(\\|/))+(kconfig|zephyr)"]

rst_epilog = """
"""

rst_prolog = """
.. include:: /consulting.rst
"""

# Options for HTML output ------------------------------------------------------

html_theme = "sphinx_swe_theme"
html_theme_path = [str(SWE_BASE / "doc" / "_theme")]
html_static_path = [
    str(SWE_BASE / "doc" / "_theme" / "sphinx_swe_theme" / "static"),
    str(SWE_BASE / "doc" / "_static"),
]
html_last_updated_fmt = "%b %d, %Y"
html_show_sourcelink = True
html_show_sphinx = False

html_theme_options = {"docset": "testing", "docsets": utils.ALL_DOCSETS}

# Options for intersphinx ------------------------------------------------------

intersphinx_mapping = dict()

# Options for external_content -------------------------------------------------

external_content_contents = [
    (SWE_BASE / "doc" / "testing", "*.rst"),
    (CMOCK_BASE / "docs", "CMock_Summary.md"),
    (UNITY_BASE / "docs", "UnityAssertionsReference.md"),
    (UNITY_BASE / "docs", "UnityConfigurationGuide.md"),
    (UNITY_BASE / "docs", "UnityGettingStartedGuide.md"),
    (UNITY_BASE / "docs", "UnityHelperScriptsGuide.md"),
    (SWE_BASE / "doc", "consulting.rst"),
]


def setup(app):
    utils.add_google_analytics(app)
