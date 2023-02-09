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

from logging import DEBUG, getLogger

from ._dasi_cffi import ffi, ffi_encode, lib

logger = getLogger(__name__)
logger.setLevel(DEBUG)


class Key:
    """
    This is the Dasi::Key.
    """

    def __init__(self, pair=None):
        logger.debug("Initialize a key. pair type: %s", type(pair))
        if isinstance(pair, Key):
            self = pair.copy()
        elif isinstance(pair, ffi.CData):
            self._cdata = pair
        else:
            self._cdata = self._new_key(pair)
            self.insert(pair)

    def __setitem__(self, keyword, value):
        lib.dasi_key_set(self._cdata, ffi_encode(keyword), ffi_encode(value))

    def _new_key(self, pair=None):
        # instantiate a key object
        ckey = ffi.new("dasi_key_t**")
        if isinstance(pair, str):
            lib.dasi_new_key_from_string(ckey, ffi_encode(pair))
        else:
            lib.dasi_new_key(ckey)
        # set the free function
        ckey = ffi.gc(ckey[0], lib.dasi_free_key)
        return ckey

    @property
    def name(self):
        return "".join(
            "_" + c.lower() if c.isupper() else c
            for c in self.__class__.__name__
        ).strip("_")

    @staticmethod
    def key_class_name(name):
        return "".join(part[:1].upper() + part[1:] for part in name.split("_"))

    def insert(self, pair):
        if isinstance(pair, dict):
            for [keyword, value] in pair.items():
                self[keyword] = value

    def copy(self):
        return Key(self._cdata)

    def print(self, stream):
        raise NotImplementedError

    def has(self, name):
        raise NotImplementedError
