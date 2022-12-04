# SPDX-License-Identifier: Apache-2.0

from pathlib import Path
import sys

# Paths ------------------------------------------------------------------------

SWE_BASE = Path(__file__).absolute().parents[2]

sys.path.insert(0, str(SWE_BASE / "doc" / "_utils"))
import utils

DOCKER_IMAGE_BASE = utils.get_projdir("develop")
ZEPHYR_BASE = utils.get_projdir("zephyr")

# General configuration --------------------------------------------------------

project = "Docker Image"
copyright = "2022 Martin Schröder"
version = release = "1.9.99"

sys.path.insert(0, str(ZEPHYR_BASE / "doc" / "_extensions"))
sys.path.insert(0, str(SWE_BASE / "doc" / "_extensions"))

extensions = ["sphinx.ext.intersphinx", "zephyr.external_content"]
source_suffix = [".rst", ".md"]

linkcheck_ignore = [r"(\.\.(\\|/))+(kconfig|zephyr)"]

rst_epilog = """
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
html_theme_options = {"docset": "develop", "docsets": utils.ALL_DOCSETS}

rst_prolog = """
.. include:: /consulting.rst
"""

# Options for intersphinx ------------------------------------------------------

intersphinx_mapping = dict()

# Options for external_content -------------------------------------------------

external_content_contents = [
    (SWE_BASE / "doc" / "develop", "*.rst"),
    (DOCKER_IMAGE_BASE / "doc", "*.rst"),
    (SWE_BASE / "doc", "consulting.rst"),
]


def setup(app):
    utils.add_google_analytics(app)
