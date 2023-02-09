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

from os import environ as osenv
from os import path as ospath

from cffi import FFI
from pkg_resources import parse_version

from .log import logger

__file_dir__ = ospath.dirname(__file__)
"""current file path directory"""


with open(ospath.join(ospath.dirname(__file_dir__), "VERSION")) as file_:
    __pydasi_version__ = file_.read().strip()


ffi = FFI()



class DASIException(RuntimeError):
    """Raised when dasi library throws exception"""

    pass


class CFFIModuleLoadFailed(ImportError):
    """Raised when the shared library fails to load"""

    pass


class PatchedLib:
    """
    Patch a CFFI library with error handling

    Finds the header file associated with the DASI C API and parses it,
    loads the shared library, and patches the accessors with
    automatic python-C error handling.
    """

    def __init__(self):
        logger.info("Loading the DASI library...")
        # parse the C source; types, functions, globals, etc.
        ffi.cdef(self.__read_header())

        lib_names = []
        for dir_ in ("DASI_DIR", "dasi_DIR"):
            if osenv.get(dir_):
                self.__insert_lib_name(lib_names, dir_, "lib/libdasi")
                self.__insert_lib_name(lib_names, dir_, "lib64/libdasi")
                self.__insert_lib_name(lib_names, dir_, "lib/libdasi.so")
                self.__insert_lib_name(lib_names, dir_, "lib64/libdasi.so")

        for libname in lib_names:
            try:
                self.__lib = ffi.dlopen(libname)
                logger.info("- loaded: " + libname)
                break
            except Exception:
                logger.debug("- cannot load: " + libname)
        else:
            raise CFFIModuleLoadFailed(
                "The shared library 'dasi' could not be found on the system!"
            )

        # All of the executable members of the CFFI-loaded library are
        # functions in the DASI C API. These should be wrapped with the correct
        # error handling. Otherwise forward these on directly.

        for f in dir(self.__lib):
            try:
                attr = getattr(self.__lib, f)
                setattr(
                    self,
                    f,
                    self.__check_error(attr, f) if callable(attr) else attr,
                )
            except Exception as e:
                logger.error(str(e))
                logger.error("Error retrieving attribute", f, "from library")

        # Initialise and setup for python-appropriate behaviour
        self.dasi_initialise_api()  # type: ignore
        # check the version
        self.__check_version()

    def __insert_lib_name(self, lib_names, dir, name):
        lib_names.insert(0, ospath.join(osenv[dir], name))

    def __check_version(self):
        """check the library version against pydasi version"""
        tmp_str = ffi.new("char**")
        self.dasi_version(tmp_str)  # type: ignore
        lib_version = ffi.string(tmp_str[0]).decode("utf-8")  # type: ignore

        if parse_version(lib_version) < parse_version(__pydasi_version__):
            msg = "The library version '{}' is older than '{}'.".format(
                lib_version, __pydasi_version__
            )
            raise CFFIModuleLoadFailed(msg)
        else:
            logger.info("Version: %s", lib_version)

    def __read_header(self) -> str:
        with open(ospath.join(__file_dir__, "dasi_cffi.h")) as header_file:
            retval = header_file.read()
        return retval

    def __check_error(self, fn, name):
        """
        If calls into the DASI library return errors, ensure that they get
        detected and reported by throwing an appropriate python exception.
        """

        def wrapped_fn(*args, **kwargs):
            retval = fn(*args, **kwargs)
            if retval not in (
                self.__lib.DASI_SUCCESS,  # type: ignore
                self.__lib.DASI_ITERATION_COMPLETE,  # type: ignore
            ):
                err = self.__lib.dasi_get_error_string(retval)  # type: ignore
                excpt_str = "Error in function '{}': {}".format(name, err)
                raise DASIException(excpt_str)
            return retval

        return wrapped_fn


try:
    lib = PatchedLib()
except CFFIModuleLoadFailed as e:
    logger.error(str(e))
    raise ImportError() from e
