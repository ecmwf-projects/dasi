# DASI

A metadata-driven data store, developed as part of the [IO-SEA project](https://iosea-project.eu/)

:warning: This project is under development, use at your own risk!

## Requirements

* A C++17 compiler (e.g., GCC 9 or higher)
* CMake, version 3.12 or higher (https://cmake.org/)
* ecbuild, version 3.4 or higher (https://github.com/ecmwf/ecbuild)
* (optional) yaml-cpp, version 0.6.0 or higher (https://github.com/jbeder/yaml-cpp);
  if not present, the package will be downloaded and built automatically
* (for the Python API) Python, version 3.7 or higher

## Building and installing

```
git clone https://github.com/ecmwf-projects/dasi.git
cd dasi
mkdir build
cd build
ecbuild --prefix=../install .. # add -Dyaml-cpp_ROOT=/path if needed
make -j
ctest
make install
```

## Authors

Simon Smart, Olivier Iffrig (firstname.lastname @ecmwf.int)

## License

[Apache License 2.0](LICENSE). In applying this licence, ECMWF does not waive
the privileges and immunities granted to it by virtue of its status as an
intergovernmental organisation nor does it submit to any jurisdiction.
