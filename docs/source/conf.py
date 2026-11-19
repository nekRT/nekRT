"""Sphinx configuration for the nekRT documentation."""

import re
import sys
from datetime import date
from pathlib import Path

DOCS_DIR = Path(__file__).resolve().parent.parent
REPO_DIR = DOCS_DIR.parent

# Local Sphinx extensions.
sys.path.insert(0, str(Path(__file__).resolve().parent / "_ext"))

# -- Project information -----------------------------------------------------

# The version is set once, in the project() call of the top-level CMakeLists.txt.
_cmakelists = (REPO_DIR / "CMakeLists.txt").read_text()
_version = re.search(
    r"project\s*\([^)]*\bVERSION\s+([0-9][0-9.]*)", _cmakelists)

project = "nekRT"
author = "Liam Pohlmann"
copyright = f"{date.today().year}, {author}"
release = _version.group(1)
version = ".".join(release.split(".")[:2])

# -- General configuration ---------------------------------------------------

extensions = [
    "myst_parser",
    "breathe",
    "sphinx.ext.mathjax",
    "sphinx.ext.githubpages",
    "sphinxcontrib.bibtex",
    "sphinx_copybutton",
    "sphinx_design",
    "sphinx_licenseinfo",
    "schema_doc",
    "sphinx.ext.autosectionlabel",
]

autosectionlabel_prefix_document = True

source_suffix = {".md": "markdown", ".rst": "restructuredtext"}
root_doc = "index"
exclude_patterns = []
numfig = True

# -- MyST --------------------------------------------------------------------

myst_enable_extensions = [
    "amsmath",
    "colon_fence",
    "deflist",
    "dollarmath",
]
myst_heading_anchors = 3

# -- Math --------------------------------------------------------------------

# Shared notation, so every page writes the same symbol the same way.
mathjax3_config = {
    "tex": {
        "macros": {
            "vr": r"\mathbf{r}",
            "vOmega": r"\boldsymbol{\Omega}",
            "angflux": r"\psi",
            "sclflux": r"\phi",
            "sigt": r"\Sigma_t",
            "sigs": r"\Sigma_s",
        }
    }
}

# -- Input file schema --------------------------------------------------------

# The JSON Schema that the {schema-block} directive (docs/source/_ext/schema_doc.py)
# builds the input file tables from.
schema_path = str(REPO_DIR / "python" / "src" /
                  "nekrt" / "schemas" / "input-v0.schema.json")

# -- Breathe (C++ API from Doxygen XML) --------------------------------------

# Written by the docs-doxygen CMake target; see docs/CMakeLists.txt, which
# passes the real path with -D. The default matches the "docs" preset.
breathe_projects = {"nekrt": str(
    REPO_DIR / "build" / "docs" / "docs" / "doxygen-xml")}
breathe_default_project = "nekrt"
breathe_default_members = ("members", "undoc-members")

# -- Bibliography ------------------------------------------------------------

bibtex_bibfiles = ["references.bib"]
bibtex_default_style = "unsrt"

# -- HTML output -------------------------------------------------------------

html_theme = "furo"
html_title = f"nekRT {release} documentation"
html_static_path = ["_static", "logo"]
html_css_files = ["css/nekrt.css"]
html_show_sphinx = False

# The site palette: violet brand colors on cool neutrals. Only the logo uses
# magma (docs/source/logo/nekrt_logo.py). Furo follows the reader's system
# light/dark setting and offers a toggle; these are its two palettes.
# docs/doxygen/customization.css uses the same colors for the Doxygen pages.
html_theme_options = {
    "light_logo": "logo_dark_circle.svg",
    "dark_logo": "logo_white_circle.svg",
    "light_css_variables": {
        "color-brand-primary": "#641A80",
        "color-brand-content": "#641A80",
        "color-brand-visited": "#8C2981",
        "color-foreground-primary": "#1B1424",
        "color-foreground-secondary": "#564E68",
        "color-background-primary": "#FFFFFF",
        "color-background-secondary": "#F3F0F7",
        "color-background-hover": "#E2DBEC",
        "color-background-border": "#D3CBDF",
        "color-sidebar-background": "#DDD4EA",
        "color-sidebar-item-background--hover": "#D4C9E3",
        "color-sidebar-search-background": "#F5F2F9",
        "color-link--hover": "#CA3E72",
        "color-link-underline--hover": "#CA3E72",
        "color-inline-code-background": "#F3F0F7",
        "nekrt-heading": "#251255",
        "nekrt-accent": "#CA3E72",
        "nekrt-table-head": "#3B0F70",
    },
    "dark_css_variables": {
        "color-brand-primary": "#B79CF5",
        "color-brand-content": "#B79CF5",
        "color-brand-visited": "#9A86C9",
        "color-foreground-primary": "#E7E4EF",
        "color-foreground-secondary": "#A9A3B8",
        "color-background-primary": "#14121C",
        "color-background-secondary": "#1A1724",
        "color-background-hover": "#252033",
        "color-background-border": "#2E2940",
        "color-sidebar-background": "#0E0C14",
        "color-sidebar-search-background": "#14121C",
        "color-sidebar-search-background--focus": "#14121C",
        "color-link--hover": "#D6C8FB",
        "color-link-underline--hover": "#D6C8FB",
        "color-inline-code-background": "#211D2D",
        "nekrt-heading": "#F1EDF9",
        "nekrt-accent": "#9D84E8",
        "nekrt-table-head": "#3B2A66",
    },
    "source_repository": "https://github.com/nekRT/nekRT/",
    "source_branch": "main",
    "source_directory": "docs/source/",
}
