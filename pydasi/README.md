# pydasi

[![PyPI](https://img.shields.io/pypi/v/pydasi)](https://pypi.org/project/pydasi/)<!-- [![Build Status](https://img.shields.io/github/workflow/status/ecmwf-projects/dasi/Continuous%20Integration/develop)](https://github.com/ecmwf-projects/dasi/actions/workflows/ci.yml) -->
[![Documentation Status](https://readthedocs.org/projects/pydasi/badge/?version=latest)](https://pydasi.readthedocs.io/en/latest/?badge=latest)
[![Code style: black](https://img.shields.io/badge/code%20style-black-000000.svg)](https://github.com/psf/black)
[![License](https://img.shields.io/github/license/ecmwf-projects/dasi)](https://github.com/ecmwf-projects/dasi/blob/master/LICENSE)

The Python interface of `DASI` (Data Access and Storage Interface), which is developed as part of the EuroHPC project IO-SEA.

[Documentation]

## Dependencies

### Required

* Python 3.x
* [dasi] (eckit, metkit, fdb5)

    <!-- The library `dasi` must be available to Python (through the CFFI mechanism) as a shared library. This can be achived by one of the following ways;
    * installing as a system library,
    * setting `dasi_DIR` environment variable, or
    * adding the installation directory in `LD_LIBRARY_PATH`. -->

### Optional

* [pytest]
* [pandoc]
* [Jupyter Notebook]

## Installation

```sh
pip install --user pydasi
```

Check if the module was installed correctly:

```sh
python
>>> import dasi
```

## Usage

An introductory Jupyter Notebook with helpful usage examples is provided in the root of this repository:

```sh
git clone git@github.com:ecmwf-projects/dasi.git
cd dasi/pydasi
jupyter notebook Introduction.ipynb
```

## Development

### Run Unit Tests

To run the unit tests, make sure that the `pytest` module is installed first:

```sh
python -m pytest
```

### Build Documentation

To build the documentation locally, please install the Python dependencies first:

```sh
cd docs
pip install -r requirements.txt
make html
```

The built HTML documentation will be available under the `docs/_build/html/index.html` path.

## License

This software is licensed under the terms of the Apache Licence Version 2.0 which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.

In applying this licence, ECMWF does not waive the privileges and immunities granted to it by virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

[Documentation]: https://pydasi.readthedocs.io/en/latest/
<!-- [Changelog]: ./CHANGELOG.md -->
[dasi]: https://github.com/ecmwf-projects/dasi
[pytest]: https://pytest.org
[pandoc]: https://pandoc.org/
[Jupyter Notebook]: https://jupyter.org
