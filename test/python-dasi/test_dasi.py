
import pytest

from dasi import DASI, DASIError, DASIObjectNotFound, InlineConfig, Key

TEST_SCHEMA = """\
schema:
 - [key1, key2, key3,
     [key1a, key2a, key3,
        [key1b, key2b, key3b],
        [key1B, key2B, key3B]],

     [key1A, key2A, key3,
        [key1b, key2b, key3b]],
     [Key1A, Key2A, Key3A,
        [key1b, key2b, key3b]]
   ]
 - [Key1, Key2, Key3,
     [key1a, key2a, key3,
        [key1b, key2b, key3b],
        [key1B, key2B, key3B]],

     [key1A, key2A, key3,
        [key1b, key2b, key3b]],
     [Key1A, Key2A, Key3A,
        [key1b, key2b, key3b]]
   ]
"""


@pytest.fixture
def dasi_config(tmp_path):
    cfg = "engine: shelf\n"
    cfg += "root: "
    cfg += str(tmp_path / "shelf")
    cfg += "\n"
    cfg += TEST_SCHEMA
    return InlineConfig(cfg)


def test_simple_put(dasi_config, tmp_path):
    dasi = DASI(dasi_config)

    key = {
        "key1": "value1",
        "key2": "value2",
        "key3": "value3",
        "key1a": "value1a",
        "key2a": "value2a",
        "key1b": "value1b",
        "key2b": "value2b",
        "key3b": "value3b",
    }

    expected_path = (tmp_path / "shelf" /
            "key1=value1,key2=value2,key3=value3" /
            "key1a=value1a,key2a=value2a,key3=value3" /
            "key1b=value1b,key2b=value2b,key3b=value3b")

    test_data = b"TESTING TESTING"
    dasi.put(key, test_data)

    assert expected_path.exists()
    archived_data = expected_path.read_bytes()
    assert len(archived_data) == len(test_data)
    assert archived_data == test_data


@pytest.mark.xfail(reason="Not implemented")
def test_put_invalid_key(dasi_config):
    dasi = DASI(dasi_config)

    valid_key = {
        "key1": "value1",
        "key2": "value2",
        "key3": "value3",
        "key1a": "value1a",
        "key2a": "value2a",
        "key1b": "value1b",
        "key2b": "value2b",
        "key3b": "value3b",
    }

    # key1a not provided

    key1 = valid_key.copy()
    del key1["key1a"]

    # Extra keys provided

    key2 = valid_key.copy()
    key2["invalid"] = "key"

    # Key provided from another part of the schema

    key3 = valid_key.copy()
    key3["key1A"] = "value1A"

    key4 = valid_key.copy()
    del key4["key1a"]
    key4["key1A"] = "value1A"

    test_data = b"TESTING TESTING"
    dasi.put(valid_key, test_data)

    for key in [key1, key2, key3]:
        with pytest.raises(DASIError):
            dasi.put(key, test_data)


def test_simple_get(dasi_config):
    dasi = DASI(dasi_config)

    key = {
        "key1": "value1",
        "key2": "value2",
        "key3": "value3",
        "key1a": "value1a",
        "key2a": "value2a",
        "key1b": "value1b",
        "key2b": "value2b",
        "key3b": "value3b",
    }

    query = {
        "key1": ["value1"],
        "key2": ["value2"],
        "key3": ["value3"],
        "key1a": ["value1a"],
        "key2a": ["value2a"],
        "key1b": ["value1b"],
        "key2b": ["value2b"],
        "key3b": ["value3b"],
    }

    test_data = b"TESTING TESTING"
    dasi.put(key, test_data)

    extra_data = b"NEW DATA"
    key["key3b"] = "new3b"
    dasi.put(key, extra_data)

    result = dasi.get(query)
    with result.handle() as handle:
        res = handle.read(len(test_data))
        assert len(res) == len(test_data)
        assert res == test_data


def test_get_multiple(dasi_config):
    dasi = DASI(dasi_config)

    key = {
        "key1": "value1",
        "key2": "value2",
        "key3": "value3",
        "key1a": "value1a",
        "key2a": "value2a",
        "key1b": "value1b",
        "key2b": "value2b",
        "key3b": "value3b",
    }

    query = {
        "key1": ["value1"],
        "key2": ["value2"],
        "key3": ["value3"],
        "key1a": ["value1a"],
        "key2a": ["value2a"],
        "key1b": ["value1b"],
        "key2b": ["value2b"],
        "key3b": ["value3b", "new3b"],
    }

    test_data = b"TESTING TESTING\0"
    dasi.put(key, test_data)

    extra_data = b"NEW DATA\0"
    key["key3b"] = "new3b"
    dasi.put(key, extra_data)

    result = dasi.get(query)
    with result.handle() as handle:
        res = handle.read(len(test_data))
        assert len(res) == len(test_data)
        assert res == test_data

        res2 = handle.read(len(extra_data))
        assert len(res2) == len(extra_data)
        assert res2 == extra_data


def test_get_iterator(dasi_config):
    dasi = DASI(dasi_config)

    kbase = {
        "key1": "value1",
        "key2": "value2",
        "key3": "value3",
        "key1a": "value1a",
        "key2a": "value2a",
        "key1b": "value1b",
        "key2b": "value2b",
        "key3b": "value3b",
    }

    num_keys = 5
    vals = []
    expected = {}
    templ = b"TEST "
    data_len = len(templ) + 2
    for i in range(num_keys):
        key = kbase.copy()
        kv = str(i)
        key["key2b"] = kv
        data = b"%b%02d" % (templ, i)
        dasi.put(key, data)
        if i % 2 == 0:
            vals.append(kv)
            expected[kv] = (Key(key), data)

    query = {
        "key1": ["value1"],
        "key2": ["value2"],
        "key3": ["value3"],
        "key1a": ["value1a"],
        "key2a": ["value2a"],
        "key1b": ["value1b"],
        "key2b": vals,
        "key3b": ["value3b"],
    }

    result = dasi.get(query)

    for key, handle in result:
        kv = key["key2b"]
        assert kv in expected
        ekey, edata = expected.pop(kv)

        assert ekey == key

        with handle:
            res = handle.read(data_len)
            assert len(res) == data_len
            assert res == edata

    assert expected == {}


def test_get_no_data(dasi_config):
    dasi = DASI(dasi_config)

    key = {
        "key1": "value1",
        "key2": "value2",
        "key3": "value3",
        "key1a": "value1a",
        "key2a": "value2a",
        "key1b": "value1b",
        "key2b": "value2b",
        "key3b": "value3b",
    }

    query = {
        "key1": {"value1"},
        "key2": {"value2"},
        "key3": {"value3"},
        "key1a": {"value1a"},
        "key2a": {"value2a"},
        "key1b": {"value1b"},
        "key2b": {"value2b"},
        "key3b": {"absent"},
    }

    test_data = b"TESTING TESTING"
    dasi.put(key, test_data)

    with pytest.raises(DASIObjectNotFound):
        dasi.get(query)


def test_get_partial(dasi_config):
    dasi = DASI(dasi_config)

    key = {
        "key1": "value1",
        "key2": "value2",
        "key3": "value3",
        "key1a": "value1a",
        "key2a": "value2a",
        "key1b": "value1b",
        "key2b": "value2b",
        "key3b": "value3b",
    }

    query = {
        "key1": ["value1"],
        "key2": ["value2"],
        "key3": ["value3"],
        "key1a": ["value1a"],
        "key2a": ["value2a"],
        "key1b": ["value1b", "absent"],
        "key2b": ["value2b"],
        "key3b": ["value3b"],
    }

    test_data = b"TESTING TESTING"
    dasi.put(key, test_data)

    with pytest.raises(DASIObjectNotFound):
        dasi.get(query)


def test_list_full(dasi_config):
    dasi = DASI(dasi_config)

    kbase = {
        "key1": "value1",
        "key2": "value2",
        "key3": "value3",
        "key1a": "value1a",
        "key2a": "value2a",
        "key1b": "value1b",
        "key2b": "value2b",
        "key3b": "value3b",
    }

    num_keys = 5
    vals = []
    expected = {}
    templ = b"TEST "
    for i in range(num_keys):
        key = kbase.copy()
        kv = str(i)
        key["key2b"] = kv
        data = b"%b%02d" % (templ, i)
        dasi.put(key, data)
        if i % 2 == 0:
            vals.append(kv)
            expected[kv] = Key(key)

    query = {
        "key1": ["value1"],
        "key2": ["value2"],
        "key3": ["value3"],
        "key1a": ["value1a"],
        "key2a": ["value2a"],
        "key1b": ["value1b"],
        "key2b": vals,
        "key3b": ["value3b"],
    }

    result = dasi.list(query)

    for key in result:
        kv = key["key2b"]
        assert kv in expected
        ekey = expected.pop(kv)
        assert ekey == key

    assert expected == {}