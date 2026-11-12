[![License](https://img.shields.io/github/license/nekRT/nekRT)](https://github.com/nekRT/nekRT/blob/main/LICENSE)
[![Docs](https://github.com/nekRT/nekRT/actions/workflows/docs.yml/badge.svg)](https://github.com/nekRT/nekRT/actions/workflows/docs.yml)
[![C++ Format and Lint](https://github.com/nekRT/nekRT/actions/workflows/format-check.yml/badge.svg)](https://github.com/nekRT/nekRT/actions/workflows/format-check.yml)
[![Build and Test](https://github.com/nekRT/nekRT/actions/workflows/build_and_test.yml/badge.svg)](https://github.com/nekRT/nekRT/actions/workflows/build_and_test.yml)


# nekRT: Spectral Element Radiation Transport Code
nekRT is a deterministic spectral element radiation transport code built on [mfem](https://github.com/mfem/mfem/).
This project is currently being developed by Liam Pohlmann at the University of Illinois Urbana-Champaign.

## Documentation
The documentation is at <https://nekrt.github.io/nekRT/>.

To build it locally, you need a C++ compiler plus the following packages:

| Package | Version |
|---|---|
| [CMake](https://cmake.org) | ≥ 3.25 |
| [Ninja](https://ninja-build.org) | ≥ 1.13.2 |
| [Doxygen](https://www.doxygen.nl) | ≥ 1.18.0 |
| [Graphviz](https://graphviz.org) | ≥ 14.1.2 |
| [Python](https://www.python.org) | ≥ 3.11 |
| [Sphinx](https://www.sphinx-doc.org) | ≥ 8.2.3, < 9 |
| [myst-parser](https://myst-parser.readthedocs.io) | ≥ 5.1.0, < 6 |
| [breathe](https://breathe.readthedocs.io) | ≥ 5.1.0, < 6 |
| [sphinxcontrib-bibtex](https://sphinxcontrib-bibtex.readthedocs.io) | ≥ 2.7.0, < 3 |
| [sphinx-copybutton](https://sphinx-copybutton.readthedocs.io) | ≥ 0.5.2, < 0.6 |
| [sphinx-design](https://sphinx-design.readthedocs.io) | ≥ 0.7.0, < 0.8 |
| [sphinx-autobuild](https://github.com/sphinx-doc/sphinx-autobuild) | ≥ 2025.8.25 |
| [furo](https://pradyunsg.me/furo/) | ≥ 2025.12.19 |

All of them are in the conda environment `docs/environment.yml`. The Python packages alone are also in `docs/requirements.txt` for pip. Then build the site:

```
conda env create -f docs/environment.yml
conda activate nekrt-docs
cmake --workflow --preset docs
```

and open `build/docs/docs/html/index.html`.

## Installation
See the documentation's [Getting Started](https://nekrt.github.io/nekRT/getting-started/index.html) page.

## License
nekRT is distributed under the BSD-3-Clause license.

