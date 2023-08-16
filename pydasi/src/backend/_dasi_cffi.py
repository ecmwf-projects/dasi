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

from os import path as ospath

from .findlib import FindLib

from utils.log import get_logger
from utils.version import __version__

from cffi import FFI
from pkg_resources import parse_version


logger = get_logger(name=__package__)

__file_dir__ = ospath.dirname(__file__)
"""current file path directory"""


ffi = FFI()


def ffi_encode(data) -> bytes:
    """convert data to bytes (c char)"""
    if isinstance(data, bytes):
        return data

    if not isinstance(data, str):
        data = str(data)

    return data.encode(encoding="utf-8", errors="surrogateescape")


def ffi_decode(data: FFI.CData) -> str:
    """convert data back to str type"""
    buf = ffi.string(data)
    if isinstance(buf, str):
        return buf
    else:
        return buf.decode(encoding="utf-8", errors="surrogateescape")


def read_lib_version(lib) -> str:
    tmp = ffi.new("char**")
    lib.dasi_version(tmp)
    return ffi_decode(tmp[0])


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
        logger.info("Initializing the interface to DASI C library...")

        # parse the C source; types, functions, globals, etc.
        ffi.cdef(open(ospath.join(__file_dir__, "dasi_cffi.h")).read())
        # ffi.cdef(self.__read_header())

        try:
            libdasi = FindLib("libdasi")
            self.__lib = ffi.dlopen(libdasi.path)
            logger.info("- loaded: " + libdasi.path)
        except Exception as e:
            logger.error(str(e))
            raise CFFIModuleLoadFailed from e

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
        self.__lib.dasi_initialise_api()

        # check the version
        lib_version = read_lib_version(self.__lib)
        if parse_version(lib_version) < parse_version(__version__):
            msg = "The library version '{}' is older than '{}'.".format(
                lib_version, __version__
            )
            raise CFFIModuleLoadFailed(msg)
        else:
            logger.info("- version: %s", lib_version)

    def __check_error(self, fn, name: str):
        """
        If calls into the DASI library return errors, ensure that they get
        detected and reported by throwing an appropriate python exception.
        """

        def wrapped_fn(*args, **kwargs):
            retval = fn(*args, **kwargs)
            if retval not in (
                self.__lib.DASI_SUCCESS,
                self.__lib.DASI_ITERATION_COMPLETE,
            ):
                err = ffi_decode(self.__lib.dasi_get_error_string(retval))
                excpt_str = "Error in function '{}': {}".format(name, err)
                raise DASIException(excpt_str)
            return retval

        return wrapped_fn


try:
    lib = PatchedLib()
except CFFIModuleLoadFailed as e:
    raise ImportError() from e
