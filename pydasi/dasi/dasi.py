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

from .cffi import DASIException, ffi, lib
from .key import Key
from .query import Query
from .retrieve import Retrieve
from .utils import get_logger

logger = get_logger(name=__name__)


def _new_session(config):
    # allocate an instance
    cobj = ffi.new("dasi_t **")
    lib.dasi_open(cobj, fsencode(config))
    # set the free function
    cobj = ffi.gc(cobj[0], lib.dasi_close)
    return cobj


class Dasi:
    """
    TODO documentation
    """

    # TODO: Be able to configure this by providing in a python dictionary
    def __init__(self, config_path: str):
        if config_path:
            logger.info("Config file: %s", config_path)
            self._cdata = _new_session(config_path)
        else:
            err_msg = "Could not find the config file:\n{}".format(config_path)
            raise DASIException(err_msg)

    def archive(self, key, data):
        if not isinstance(key, Key):
            key = Key(key)

        lib.dasi_archive(
            self._cdata, key._cdata, ffi.from_buffer(data), len(data)
        )

    def _new_retrieve(self, query: Query) -> ffi.CData:
        # allocate an instance
        cret = ffi.new("dasi_retrieve_t **")
        lib.dasi_retrieve(self._cdata, query._cdata, cret)
        # set the free function
        cret = ffi.gc(cret[0], lib.dasi_free_retrieve)
        return cret

    def retrieve(self, query) -> Retrieve:
        if not isinstance(query, Query):
            query = Query(query)

        retriev = Retrieve(self._new_retrieve(query))
        logger.debug("- retrieve count: %d", len(retriev))

        return retriev
