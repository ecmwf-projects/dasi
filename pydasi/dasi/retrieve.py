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

from ._dasi_cffi import ffi, lib
from .key import Key
from .utils import DEBUG, getLogger

logger = getLogger(__name__)
logger.setLevel(DEBUG)


class Retrieve:
    """
    RetrieveResult
    """

    def __init__(self, cdata: ffi.CData):
        logger.debug("Initialize Retrieve...")
        if ffi.typeof(cdata) is ffi.typeof("dasi_retrieve_t *"):
            self._cdata = cdata

    def __iter__(self):
        return self

    def __next__(self):
        stat = lib.dasi_retrieve_next(self._cdata)
        if stat == lib.DASI_ITERATION_COMPLETE:
            raise StopIteration
        return self

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

    def count(self) -> int:
        count = ffi.new("long *", 0)
        lib.dasi_retrieve_count(self._cdata, count)
        return count[0]

    def read(self):
        length = ffi.new("long *", 0)
        ckey = ffi.new("dasi_key_t **", ffi.NULL)
        lib.dasi_retrieve_attrs(self._cdata, ckey, ffi.NULL, ffi.NULL, length)
        ckey = ffi.gc(ckey[0], lib.dasi_free_key)
        logger.debug("- data length = %d", length[0])
        data = bytearray(length[0])
        lib.dasi_retrieve_read(self._cdata, ffi.from_buffer(data), length)
        return Key(ckey), data
