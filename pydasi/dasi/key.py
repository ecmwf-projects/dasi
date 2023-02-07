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


class Key:
    """
    This is the Dasi::Key.
    """

    def __init__(self, pair: str = ""):
        # Allocate an instance
        key = ffi.new("dasi_key_t**")
        if pair:
            lib.dasi_new_key_from_string(key, pair.encode("utf-8"))
        else:
            lib.dasi_new_key(key)
        # Set free function
        key = ffi.gc(key[0], lib.dasi_free_key)

        self._key = key

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

    def has(self, name):
        raise NotImplementedError


key = Key()
