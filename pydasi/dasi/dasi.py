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

from os import fsencode

from ._dasi_cffi import DASIException, ffi, lib, logger
from .key import Key


class Dasi:
    """
    This is the Dasi::Dasi.
    """

    def __init__(self, config_file_path: str):
        if config_file_path:
            # Allocate an instance
            session = ffi.new("dasi_t**")
            try:
                lib.dasi_open(session, fsencode(config_file_path))
            except DASIException as e:
                logger.error("Could not create the DASI session!")
                raise RuntimeError() from e
            # Set free function
            session = ffi.gc(session[0], lib.dasi_close)
        else:
            raise DASIException("Missing config file!")

        self._session = session

    @property
    def name(self):
        return "".join(
            "_" + c.lower() if c.isupper() else c
            for c in self.__class__.__name__
        ).strip("_")

    @staticmethod
    def key_class_name(name):
        return "".join(part[:1].upper() + part[1:] for part in name.split("_"))

    def print(self, stream):
        raise NotImplementedError

    def archive(self, key, data):
        if not isinstance(key, Key):
            key = Key(key)

        lib.dasi_archive(  # type: ignore
            self._session, key._cdata, ffi.from_buffer(data), len(data)
        )
