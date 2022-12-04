# SPDX-License-Identifier: Apache-2.0

import os
from pathlib import Path
import sys
from sphinx.config import eval_config_file


# Paths ------------------------------------------------------------------------

SWE_BASE = Path(__file__).absolute().parents[2]
DOC_BASE = Path(__file__).absolute().parents[1]

sys.path.insert(0, str(DOC_BASE / "_utils"))
sys.path.insert(0, str(DOC_BASE / "_extensions"))
import utils

ZEPHYR_BASE = utils.get_projdir("zephyr")

# pylint: disable=undefined-variable,used-before-assignment

# General ----------------------------------------------------------------------

# Import all Zephyr configuration, override as needed later
os.environ["ZEPHYR_BASE"] = str(ZEPHYR_BASE)
os.environ["ZEPHYR_BUILD"] = str(utils.get_builddir() / "zephyr")

conf = eval_config_file(str(ZEPHYR_BASE / "doc" / "conf.py"), tags)
locals().update(conf)

extensions.extend(
    [
        "sphinx.ext.intersphinx",
    ]
)

# Options for HTML output ------------------------------------------------------

html_theme = "sphinx_swe_theme"
html_theme_path = [str(SWE_BASE / "doc" / "_theme")]
html_favicon = None
html_static_path.append(str(SWE_BASE / "doc" / "_static"))
html_last_updated_fmt = "%b %d, %Y"
html_show_sourcelink = True
html_logo = None

html_context = {
    "show_license": True,
    "docs_title": docs_title,
    "is_release": is_release,
}

html_theme_options = {"docset": "zephyr", "docsets": utils.ALL_DOCSETS}

rst_prolog = """
.. include:: /consulting.rst
"""

external_content_contents.extend(
    [
        (SWE_BASE / "doc", "consulting.rst"),
    ]
)

# Options for intersphinx ------------------------------------------------------

intersphinx_mapping = dict()

kconfig_mapping = utils.get_intersphinx_mapping("kconfig")
if kconfig_mapping:
    intersphinx_mapping["kconfig"] = kconfig_mapping

# -- Options for zephyr.warnings_filter ----------------------------------------

warnings_filter_silent = True

# -- Options for zephyr.kconfig ------------------------------------------------

kconfig_generate_db = True

# pylint: enable=undefined-variable,used-before-assignment


def setup(app):
    app.add_css_file("css/zephyr.css")

    utils.add_google_analytics(app)
