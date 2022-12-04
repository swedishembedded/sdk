# SPDX-License-Identifier: Apache-2.0

from os import PathLike
from pathlib import Path
from typing import Tuple, Optional

from sphinx.application import Sphinx
from sphinx.cmd.build import get_parser
from west.manifest import Manifest


_SWE_BASE = Path(__file__).parents[2]
"""SWE Repository root"""

_MANIFEST = Manifest.from_file(_SWE_BASE / "west.yml")
"""Manifest instance"""

ALL_DOCSETS = {
    "sdk": ("SDK", "index", "sdk"),
    "mcuboot": ("Bootloader", "index", "mcuboot"),
    "zephyr": ("Kernel", "index", "zephyr"),
    # "userspace": ("Libraries", "index", "userspace"),
    "renode-docs": ("Simulator", "index", "renode-docs"),
    # "docker-image": ("Docker Image", "index", "docker-image"),
}
"""All supported docsets (name: title, home page, manifest project name)."""


def get_projdir(name: str) -> Path:
    """Obtain the project directory for the given docset.

    Args:
        docset: Target docset.

    Returns:
        Project path for the given docset.
    """

    p = next((p for p in _MANIFEST.projects if p.name == name), None)
    assert p, f"Project {name} not in manifest"

    return Path(p.topdir) / Path(p.path)


def get_builddir() -> PathLike:
    """Obtain Sphinx base build directory for a given docset.

    Returns:
        Base build path.
    """
    parser = get_parser()
    args = parser.parse_args()
    return (Path(args.outputdir) / ".." / "..").resolve()


def get_outputdir(docset: str) -> PathLike:
    """Obtain Sphinx output directory for a given docset.

    Args:
        docset: Target docset.

    Returns:
        Build path of the given docset.
    """

    return get_builddir() / "html" / docset


def get_srcdir(docset: str) -> PathLike:
    """Obtain sources directory for a given docset.

    Args:
        docset: Target docset.

    Returns:
        Sources directory of the given docset.
    """

    return get_builddir() / docset / "src"


def get_intersphinx_mapping(docset: str) -> Optional[Tuple[str, str]]:
    """Obtain intersphinx configuration for a given docset.

    Args:
        docset: Target docset.

    Notes:
        Relative links are used for URL prefix.

    Returns:
        Intersphinx configuration if available.
    """

    outputdir = get_outputdir(docset)
    inventory = outputdir / "objects.inv"
    if not inventory.exists():
        return

    return (str(Path("..") / docset), str(inventory))


def add_google_analytics(app: Sphinx) -> None:
    """Add Google Analytics to a docset.

    Args:
        app: Sphinx instance.
    """

    # app.add_js_file("js/ga-tracker.js")
