# SPDX-License-Identifier: Apache-2.0

import os
from pathlib import Path
import sys
from sphinx.config import eval_config_file


# Paths ------------------------------------------------------------------------

SWE_BASE = Path(__file__).absolute().parents[2]
DOC_BASE = Path(__file__).absolute().parents[1]
ZEPHYR_BASE = Path(__file__).absolute().parents[3] / "zephyr"

sys.path.insert(0, str(DOC_BASE / "_utils"))
sys.path.insert(0, str(DOC_BASE / "_extensions"))
sys.path.insert(0, str(ZEPHYR_BASE / "doc" / "_extensions"))
import utils

RENODE_DOCS_BASE = utils.get_projdir("renode-docs")

# pylint: disable=undefined-variable,used-before-assignment

# General ----------------------------------------------------------------------

conf = eval_config_file(str(RENODE_DOCS_BASE / "source" / "conf.py"), tags)
locals().update(conf)

extensions.extend(["zephyr.external_content", "sphinx.ext.intersphinx"])

# Options for HTML output ------------------------------------------------------

html_theme = "sphinx_swe_theme"
html_theme_path = [str(SWE_BASE / "doc" / "_theme")]
html_favicon = None
html_static_path = [
    str(SWE_BASE / "doc" / "_static"),
    str(SWE_BASE / "doc" / "_theme" / "sphinx_swe_theme" / "static"),
]
html_last_updated_fmt = "%b %d, %Y"
html_show_sourcelink = True
html_logo = None
html_style = "css/swe.css"
html_context = {"show_license": True}

html_theme_options = {"docset": "renode", "docsets": utils.ALL_DOCSETS}

templates_path = [str(SWE_BASE / "doc" / "_theme" / "sphinx_swe_theme")]

latex_logo = str(RENODE_DOCS_BASE / "source" / "renode-sphinx" / "logo-latex.pdf")
latex_additional_files = [
    str(RENODE_DOCS_BASE / "source" / "**" / "img" / "*.png"),
    str(RENODE_DOCS_BASE / "source" / "renode-sphinx" / "renode.sty"),
    latex_logo,
]

rst_prolog = """
.. include:: /consulting.rst
"""

# Options for intersphinx ------------------------------------------------------

intersphinx_mapping = dict()

# zephyr.external_content

external_content_contents = [
    (RENODE_DOCS_BASE / "source", "*"),
    (SWE_BASE / "doc", "consulting.rst"),
]

# pylint: enable=undefined-variable,used-before-assignment


def setup(app):
    app.add_css_file("css/zephyr.css")
    app.add_css_file("css/swe.css")

    utils.add_google_analytics(app)
