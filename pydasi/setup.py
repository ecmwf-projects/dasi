# Copyright 2023 European Centre for Medium-Range Weather Forecasts (ECMWF)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import io
import os

from setuptools import find_packages, setup

with open(os.path.join("dasi", "VERSION")) as version_file:
    __pydasi_version__ = version_file.read().strip()


def read(path):
    file_path = os.path.join(os.path.dirname(__file__), *path.split("/"))
    return io.open(file_path, encoding="utf-8").read()


setup(
    name="pydasi",
    version=__pydasi_version__,
    description="The Python interface to DASI "
    "(Data Access and Storage Interface),"
    " developed as part of the EuroHPC project IO-SEA.",
    long_description=read("README.md"),
    long_description_content_type="text/markdown",
    author="European Centre for Medium-Range Weather Forecasts (ECMWF)",
    author_email="software.support@ecmwf.int",
    license="Apache License Version 2.0",
    url="https://github.com/ecmwf-projects/dasi",
    packages=find_packages(
        include=["dasi", "dasi.utils", "dasi.cffi"],
    ),
    package_data={
        "": ["CHANGELOG", "README.md", "LICENSE"],
        "dasi": ["VERSION"],
        "dasi.cffi": ["*.h", "**/lib*"],
    },
    include_package_data=True,
    install_requires=["cffi>=1.1.1", "pytest"],
    zip_safe=True,
    keywords=["dasi", "ecmwf"],
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: Apache Software License",
        "Natural Language :: English",
        "Programming Language :: Python",
        "Programming Language :: Python :: 3 :: Only",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: Implementation :: CPython",
        "Programming Language :: Python :: Implementation :: PyPy",
        "Operating System :: OS Independent",
    ],
)
