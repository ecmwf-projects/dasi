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

from cffi import FFI
from dasi.utils import get_logger, get_version
from pkg_resources import parse_version

from .findlib import FindLib

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
    elif isinstance(buf, bytes):
        return buf.decode(encoding="utf-8", errors="surrogateescape")

    return str()


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
        logger.info("Initializing the interface to DASI library...")
        # parse the C source; types, functions, globals, etc.
        ffi.cdef(self.__read_header())

        try:
            dasi_lib = FindLib("libdasi")
            self.__lib = ffi.dlopen(dasi_lib.library)
            logger.info("- loaded: " + dasi_lib.library)
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
        self.dasi_initialise_api()
        # check the version
        self.__check_version()

    def get_lib_version(self):
        tmp = ffi.new("char**")
        self.__lib.dasi_version(tmp)
        return ffi_decode(tmp[0])

    def __check_version(self):
        """check the library version against pydasi version"""

        lib_version = self.get_lib_version()
        version = get_version()
        if parse_version(lib_version) < parse_version(version):
            msg = "The library version '{}' is older than '{}'.".format(
                lib_version, version
            )
            raise CFFIModuleLoadFailed(msg)
        else:
            logger.info("- library version: %s", lib_version)

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
