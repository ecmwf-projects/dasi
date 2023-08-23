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

from dasi.backend import FFI, ffi, lib, new_retrieve

from dasi.key import Key


class Retrieve:
    def __init__(self, dasi: FFI.CData, query: FFI.CData):
        from dasi.utils import log

        self._log = log.getLogger(__name__)

        self._log.debug("Initialize Retrieve...")

        self.__key = Key()
        self.__data: bytearray
        self.__timestamp: int = 0
        self.__offset: int = 0
        self.__length: int = 0

        self._cdata = new_retrieve(dasi, query)

        self._log.debug("- retrieve count: %d", len(self))

    def __iter__(self):
        return self

    def __next__(self):
        stat = lib.dasi_retrieve_next(self._cdata)
        if stat == lib.DASI_ITERATION_COMPLETE:
            raise StopIteration
        self.__read()
        return self

    def __len__(self) -> int:
        count = ffi.new("long *", 0)
        lib.dasi_retrieve_count(self._cdata, count)
        return count[0]

    def __read(self):
        coffset = ffi.new("long *", 0)
        clength = ffi.new("long *", 0)
        ctime = ffi.new("dasi_time_t *", 0)
        ckey = ffi.new("dasi_key_t **", ffi.NULL)
        lib.dasi_retrieve_attrs(self._cdata, ckey, ctime, coffset, clength)
        ckey = ffi.gc(ckey[0], lib.dasi_free_key)
        data = bytearray(clength[0])
        lib.dasi_retrieve_read(self._cdata, ffi.from_buffer(data), clength)

        self.__key = Key(ckey)
        self.__data = data
        self.__timestamp = ctime[0]
        self.__offset = coffset[0]
        self.__length = clength[0]

    @property
    def key(self):
        return self.__key

    @property
    def data(self):
        return self.__data

    @property
    def timestamp(self):
        return self.__timestamp

    @property
    def offset(self):
        return self.__offset

    @property
    def length(self):
        return self.__length
