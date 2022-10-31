
import os

from . import _dasi
from ._dasi import DASIError, DASIUnexpectedError, DASIObjectNotFound

_ffi = _dasi.ffi


def _encode(data, buffer=False):
    if isinstance(data, bytes):
        return data
    if not isinstance(data, str):
        data = str(data)
    data = data.encode('utf-8', 'surrogateescape')
    if buffer:
        return _ffi.from_buffer(data)
    return data


def _decode(cdata):
    buf = _ffi.string(cdata)
    return str(buf, encoding='utf-8', errors='surrogateescape')


class InlineConfig:
    def __init__(self, config):
        self.value = _encode(config)


class Key:
    def __init__(self, data=None):
        if isinstance(data, Key):
            self._key = _dasi.key_copy(data._key)
            data = None
        elif isinstance(data, _ffi.CData):
            self._key = data
            data = None
        else:
            self._key = _dasi.key_new()

        if data is not None:
            self.update(data)

    def copy(self):
        return Key(self)

    def __getitem__(self, kw):
        return _decode(_dasi.key_get(self._key, _encode(kw)))

    def __delitem__(self, kw):
        _dasi.key_del(self._key, _encode(kw))

    def __setitem__(self, kw, val):
        _dasi.key_set(self._key, _encode(kw), _encode(val))

    def update(self, data):
        for kw, val in data.items():
            self[kw] = val

    def cmp(self, other):
        return _dasi.key_cmp(self._key, other._key)

    def __eq__(self, other):
        return self.cmp(other) == 0

    def __ne__(self, other):
        return self.cmp(other) != 0

    @property
    def _cdata(self):
        return self._key


class Query:
    def __init__(self, data=None):
        if isinstance(data, Query):
            self._query = _dasi.query_copy(data._query)
            data = None
        else:
            self._query = _dasi.query_new()

        if data is not None:
            self.update(data)

    def __setitem__(self, kw, val):
        _dasi.query_set(self._query, _encode(kw), [_encode(item, buffer=True) for item in val], len(val))

    def update(self, data):
        for kw, val in data.items():
            self[kw] = val

    @property
    def _cdata(self):
        return self._query


class ReadHandle:
    def __init__(self, cdata):
        self._handle = cdata
        _dasi.read_handle_open(self._handle)

    def close(self):
        _dasi.read_handle_close(self._handle)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def read(self, length):
        buf = bytearray(length)
        cbuf = _ffi.from_buffer(buf, require_writable=True)
        nread = _dasi.read_handle_read(self._handle, cbuf, length)
        return buf[:nread]


class RetrieveIterator:
    def __init__(self, cdata, which='both'):
        self._iterator = cdata
        self._first = True
        assert which in ['keys', 'handles', 'both']
        self._which = which

    def __iter__(self):
        return self

    def __next__(self):
        if not self._first:
            _dasi.retrieve_iterator_next(self._iterator)
        self._first = False
        key = None
        if self._which in ['keys', 'both']:
            key = Key(_dasi.retrieve_iterator_get_key(self._iterator))
        handle = None
        if self._which in ['handles', 'both']:
            handle = _dasi.retrieve_iterator_get_handle(self._iterator)
        if self._which == 'keys':
            return key
        handle = ReadHandle(handle)  # open only if needed
        if self._which == 'handles':
            return handle
        return key, handle


class RetrieveResult:
    def __init__(self, cdata):
        self._result = cdata

    def __iter__(self):
        return RetrieveIterator(_dasi.retrieve_result_iterate(self._result))

    def keys(self):
        return RetrieveIterator(_dasi.retrieve_result_iterate(self._result), 'keys')

    def handles(self):
        return RetrieveIterator(_dasi.retrieve_result_iterate(self._result), 'handles')

    def handle(self):
        return ReadHandle(_dasi.retrieve_result_get_handle(self._result))


class DASI:
    def __init__(self, config):
        if isinstance(config, InlineConfig):
            self._session = _dasi.open_str(config.value)
        else:
            self._session = _dasi.open(os.fsencode(config))

    def put(self, key, data):
        if not isinstance(key, Key):
            key = Key(key)
        _dasi.put(self._session, key._cdata, _ffi.from_buffer(data), len(data))

    def get(self, query):
        if not isinstance(query, Query):
            query = Query(query)
        return RetrieveResult(_dasi.get(self._session, query._cdata))
