
import cffi
import findlibs
import functools
from importlib import resources


ffi = cffi.FFI()
libfile = findlibs.find("dasi")
ffi.cdef(resources.read_text("dasi", "dasi_c.h"))
lib = ffi.dlopen(libfile)


class DASIError(RuntimeError):
    pass


class DASIUnexpectedError(RuntimeError):
    pass


class DASIObjectNotFound(DASIError):
    pass


def error_str(err):
    errmsg = ffi.string(lib.dasi_error_str(err))
    return str(errmsg, encoding='utf-8', errors='surrogateescape')


def wrap(fun, out=None, gc=None):
    @functools.wraps(fun)
    def wrapped(*args):
        outval = None
        if out is not None:
            outval = ffi.new(out)
            args = args + (outval,)
        err = fun(*args)
        if err == lib.DASI_SUCCESS:
            if outval is not None:
                if gc is not None:
                    return ffi.gc(outval[0], wrap(gc))
                return outval[0]
            return
        errmsg = error_str(err)
        if err == lib.DASI_ITERATOR_END:
            raise StopIteration
        if err == lib.DASI_NOT_FOUND:
            raise DASIObjectNotFound(errmsg)
        if err == lib.DASI_ERROR:
            raise DASIError(errmsg)
        if err == lib.DASI_UNEXPECTED:
            raise DASIUnexpectedError(errmsg)
        assert False, f"Unknown error code {err}"
    return wrapped


open = wrap(lib.dasi_open, out="dasi_t **", gc=lib.dasi_close)
open_str = wrap(lib.dasi_open_str, out="dasi_t **", gc=lib.dasi_close)
close = wrap(lib.dasi_close)
put = wrap(lib.dasi_put)
get = wrap(lib.dasi_get, out="dasi_retrieve_result_t **", gc=lib.dasi_retrieve_result_destroy)

key_new = wrap(lib.dasi_key_new, out="dasi_key_t **", gc=lib.dasi_key_destroy)
key_copy = wrap(lib.dasi_key_copy, out="dasi_key_t **", gc=lib.dasi_key_destroy)
key_set = wrap(lib.dasi_key_set)
key_del = wrap(lib.dasi_key_del)
key_get = wrap(lib.dasi_key_get, out="const char **")
key_iterate = wrap(lib.dasi_key_iterate,
   out="dasi_key_iterator_t **", gc=lib.dasi_key_iterator_destroy)
key_cmp = wrap(lib.dasi_key_cmp, out="int *")

key_iterator_next = wrap(lib.dasi_key_iterator_next)
key_iterator_get_keyword = wrap(lib.dasi_key_iterator_get_keyword, out="const char **")
key_iterator_get_value = wrap(lib.dasi_key_iterator_get_value, out="const char **")

query_new = wrap(lib.dasi_query_new, out="dasi_query_t **", gc=lib.dasi_query_destroy)
query_copy = wrap(lib.dasi_query_copy, out="dasi_query_t **", gc=lib.dasi_query_destroy)
query_set = wrap(lib.dasi_query_set)

retrieve_result_iterate = wrap(lib.dasi_retrieve_result_iterate,
   out="dasi_retrieve_iterator_t **", gc=lib.dasi_retrieve_iterator_destroy)
retrieve_result_get_handle = wrap(lib.dasi_retrieve_result_get_handle,
   out="dasi_read_handle_t **", gc=lib.dasi_read_handle_destroy)

retrieve_iterator_next = wrap(lib.dasi_retrieve_iterator_next)
retrieve_iterator_get_key = wrap(lib.dasi_retrieve_iterator_get_key,
   out="dasi_key_t **", gc=lib.dasi_key_destroy)
retrieve_iterator_get_handle = wrap(lib.dasi_retrieve_iterator_get_handle,
   out="dasi_read_handle_t **", gc=lib.dasi_read_handle_destroy)

read_handle_open = wrap(lib.dasi_read_handle_open)
read_handle_close = wrap(lib.dasi_read_handle_close)
read_handle_read = wrap(lib.dasi_read_handle_read, out="size_t *")
