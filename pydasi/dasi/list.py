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

from .cffi import FFI, ffi, ffi_decode, lib
from .key import Key
from .utils import get_logger

logger = get_logger(name=__name__)


class List:
    """
    Read metadata from the dasi session.
    TODO documentation
    """

    def __init__(self, cdata: FFI.CData):
        logger.debug("Initialize List...")
        self.__key = Key()
        self.__timestamp = 0
        self.__uri = ""
        self.__offset = 0
        self.__length = 0
        if ffi.typeof(cdata) is ffi.typeof("dasi_list_t *"):
            self._cdata = cdata

    def __iter__(self):
        return self

    def __next__(self):
        stat = lib.dasi_list_next(self._cdata)
        if stat == lib.DASI_ITERATION_COMPLETE:
            raise StopIteration
        self.__read()
        return self

    def __read(self):
        coffset = ffi.new("long *", 0)
        clength = ffi.new("long *", 0)
        ctime = ffi.new("dasi_time_t *", 0)
        curi = ffi.new("const char **", ffi.NULL)
        ckey = ffi.new("dasi_key_t **", ffi.NULL)
        lib.dasi_list_attrs(self._cdata, ckey, ctime, curi, coffset, clength)
        ckey = ffi.gc(ckey[0], lib.dasi_free_key)
        self.__key = Key(ckey)
        self.__timestamp = ctime[0]
        self.__uri = ffi_decode(curi[0])
        self.__offset = coffset[0]
        self.__length = clength[0]

    @property
    def key(self):
        return self.__key

    @property
    def timestamp(self):
        return self.__timestamp

    @property
    def uri(self):
        return self.__uri

    @property
    def offset(self):
        return self.__offset

    @property
    def length(self):
        return self.__length
