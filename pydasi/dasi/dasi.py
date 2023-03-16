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
from .list import List
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
    The Python interface to DASI.

    .. code-block:: python

        # Import dasi
        from dasi import Dasi

        # Create new DASI session
        dasi = Dasi("config.yml")
    """

    # TODO: Be able to configure this by providing in a python dictionary
    def __init__(self, config_path: str):
        """
        Creates new DASI session.

        :param str config_path: Path to the YAML configuration file.
        """
        if config_path:
            logger.info("Config file: %s", config_path)
            self._cdata = _new_session(config_path)
        else:
            err_msg = "Could not find the config file:\n{}".format(config_path)
            raise DASIException(err_msg)

    def flush(self):
        """
        Flushes all buffers and ensures internal state is safe (wrt failure).
        """
        lib.dasi_flush(self._cdata)

    def archive(self, key, data):
        """
        Write data to be stored according to Dasi configuration.

        :param key: The metadata description of the data to store and index
        :param data: A pointer to a (read-only) copy of the data
        """
        if not isinstance(key, Key):
            key = Key(key)

        lib.dasi_archive(
            self._cdata, key._cdata, ffi.from_buffer(data), len(data)
        )

    def _new_retrieve(self, query: Query):
        # allocate an instance
        cret = ffi.new("dasi_retrieve_t **")
        lib.dasi_retrieve(self._cdata, query._cdata, cret)
        # set the free function
        cret = ffi.gc(cret[0], lib.dasi_free_retrieve)
        return cret

    def retrieve(self, query) -> Retrieve:
        """Retrieve data objects from the archive

        :param query: A description of the span of data to retrieve
        :return: A generic data handle, that will retrieve the data
        :rtype: Retrieve
        """
        if not isinstance(query, Query):
            query = Query(query)

        retriev = Retrieve(self._new_retrieve(query))
        logger.debug("- retrieve count: %d", len(retriev))

        return retriev

    def _new_list(self, query: Query):
        # allocate an instance
        clist = ffi.new("dasi_list_t **")
        lib.dasi_list(self._cdata, query._cdata, clist)
        # set the free function
        clist = ffi.gc(clist[0], lib.dasi_free_list)
        return clist

    def list(self, query) -> List:
        """List data present and retrievable from the archive

        :param query: A description of the span of metadata to list within
        :return: An iterable details of the objects describing data.
        :rtype: List
        """
        if not isinstance(query, Query):
            query = Query(query)

        list = List(self._new_list(query))

        return list
