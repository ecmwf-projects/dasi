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

from .cffi import DASIException, ffi, ffi_decode, ffi_encode, lib
from .utils import get_logger

logger = get_logger(name=__name__)


def _new_query(pair=None):
    # allocate an instance
    cquery = ffi.new("dasi_query_t **")
    if isinstance(pair, str):
        lib.dasi_new_query_from_string(cquery, ffi_encode(pair))
    else:
        lib.dasi_new_query(cquery)
    # set the free function
    cquery = ffi.gc(cquery[0], lib.dasi_free_query)
    return cquery


class Query:
    """
    Container for the keyword:value pairs that is used for retrieving data.
    """

    # TODO think of simplifying
    def __init__(self, data=None):
        if isinstance(data, Query):
            self._cdata = data._cdata
        elif isinstance(data, ffi.CData):
            if ffi.typeof(data) is ffi.typeof("dasi_query_t *"):
                self._cdata = data
        else:
            self._cdata = _new_query(data)
            if isinstance(data, dict):
                for keyword, value in data.items():
                    self[keyword] = value

    def __setitem__(self, keyword, value):
        length = len(value)
        buf = [ffi.from_buffer(ffi_encode(item)) for item in value]
        lib.dasi_query_set(self._cdata, ffi_encode(keyword), buf, length)

    def __getitem__(self, keyword):
        value = ffi.new("const char **")
        lib.dasi_query_get(self._cdata, ffi_encode(keyword), value)
        return ffi_decode(value[0])

    def __delitem__(self, keyword):
        lib.dasi_query_erase(self._cdata, ffi_encode(keyword))

    def __len__(self):
        count = ffi.new("long*", 0)
        lib.dasi_query_keyword_count(self._cdata, count)
        return count[0]

    def get_value(self, keyword, number):
        value = ffi.new("const char **")
        lib.dasi_query_get(self._cdata, ffi_encode(keyword), number, value)
        return ffi_decode(value[0])

    def has(self, keyword):
        has = ffi.new("dasi_bool_t*", 1)
        lib.dasi_query_has(self._cdata, ffi_encode(keyword), has)
        return has[0] != 0

    def count_value(self, keyword) -> int:
        try:
            count = ffi.new("long*", 0)
            lib.dasi_query_value_count(self._cdata, ffi_encode(keyword), count)
            return count[0]
        except DASIException as e:
            logger.warning(str(e))
            return 0

    def clear(self):
        lib.dasi_query_clear(self._cdata)
