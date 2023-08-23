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

from dasi.backend import FFI, ffi, lib, ffi_decode, new_list

from dasi.key import Key


class List:
    def __init__(self, dasi: FFI.CData, query: FFI.CData):
        from dasi.utils import log

        self._log = log.getLogger(__name__)

        self._log.debug("Initialize List...")

        self.__key = Key()
        self.__uri = ffi.new("const char **", ffi.NULL)
        self.__time = ffi.new("dasi_time_t *", 0)
        self.__offset = ffi.new("long *", 0)
        self.__length = ffi.new("long *", 0)
        self._cdata = new_list(dasi, query)

    def __iter__(self):
        return self

    def __next__(self):
        if lib.dasi_list_next(self._cdata) == lib.DASI_ITERATION_COMPLETE:
            raise StopIteration
        self.__read()
        return self

    def __len__(self) -> int:
        self._log.debug("not implemented in Dasi C lib!")
        return 0

    def __read(self):
        ckey = ffi.new("dasi_key_t **", ffi.NULL)
        lib.dasi_list_attrs(
            self._cdata,
            ckey,
            self.__time,
            self.__uri,
            self.__offset,
            self.__length,
        )
        ckey: FFI.CData = ffi.gc(ckey[0], lib.dasi_free_key)
        self.__key = Key(ckey)

    @property
    def key(self) -> Key:
        return self.__key

    @property
    def uri(self) -> str:
        return ffi_decode(self.__uri[0])

    @property
    def timestamp(self) -> int:
        return self.__time[0]

    @property
    def offset(self) -> int:
        return self.__offset[0]

    @property
    def length(self) -> int:
        return self.__length[0]