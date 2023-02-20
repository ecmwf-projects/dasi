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

from .cffi import ffi, ffi_decode, ffi_encode, lib
from .utils import get_logger

logger = get_logger(name=__name__)


def _new_key(pair=None):
    ckey = ffi.new("dasi_key_t **")
    if isinstance(pair, str):
        lib.dasi_new_key_from_string(ckey, ffi_encode(pair))
    else:
        lib.dasi_new_key(ckey)
    ckey = ffi.gc(ckey[0], lib.dasi_free_key)
    return ckey


class Key:
    """
    Container for keyword:value pairs.
    """

    # TODO think of simplifying
    def __init__(self, data=None):
        if isinstance(data, Key):
            self._cdata = data._cdata
        elif isinstance(data, ffi.CData):
            if ffi.typeof(data) is ffi.typeof("dasi_key_t *"):
                self._cdata = data
        else:
            self._cdata = _new_key(data)
            if isinstance(data, dict):
                self.insert(data)

    def __setitem__(self, keyword, value):
        lib.dasi_key_set(self._cdata, ffi_encode(keyword), ffi_encode(value))

    def __getitem__(self, keyword):
        value = ffi.new("const char **")
        lib.dasi_key_get(self._cdata, ffi_encode(keyword), value)
        return ffi_decode(value[0])

    def __delitem__(self, keyword):
        lib.dasi_key_erase(self._cdata, ffi_encode(keyword))

    def __len__(self):
        count = ffi.new("long*", 0)
        lib.dasi_key_count(self._cdata, count)
        return count[0]

    # useful for setting multiple pairs at once
    # e.g., {"key3": "value3", "key4": "value4"}
    # more pythonic as opposed to parsing "key3=value3,key4=value4"
    def insert(self, pairs: dict):
        for keyword, value in pairs.items():
            self[keyword] = value

    def has(self, keyword: str) -> bool:
        has = ffi.new("dasi_bool_t*", 1)
        lib.dasi_key_has(self._cdata, ffi_encode(keyword), has)
        return has[0] != 0

    def clear(self):
        lib.dasi_key_clear(self._cdata)
