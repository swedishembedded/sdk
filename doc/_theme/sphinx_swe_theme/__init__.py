# SPDX-License-Identifier: Apache-2.0

import os

__version__ = "1.0.0"


def setup(app):
    app.add_html_theme("sphinx_swe_theme", os.path.dirname(os.path.abspath(__file__)))
    return {"parallel_read_safe": True, "parallel_write_safe": True}
