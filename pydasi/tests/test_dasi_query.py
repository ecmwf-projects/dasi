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

from dasi import Query


def test_query_typename():
    """
    Construct a query and assert its type name.
    """

    from cffi import FFI

    query = Query()

    assert FFI().typeof(query._cdata).cname == "dasi_query_t *"


def test_query_empty():
    """
    Construct empty keys and assert keys are empty by count() and has().
    """

    query = Query()

    assert query.has("key1") is False
    assert query.has("key2") is False
    assert query.count_keyword() == 0

    query["key1"] = ["value1"]
    assert query.has("key1") is True
    assert query.count_keyword() == 1
    assert query.count_value("key1") == 1

    assert query.has("key2") is False
    query["key2"] = ["value2a", "value2b"]
    assert query.has("key2") is True
    assert query.count_keyword() == 2
    assert query.count_value("key2") == 2


def test_query_clear():
    """
    Assert clear() and erase().
    """

    tmp = {
        "key1a": ["value1a"],
        "key2a": ["value2a"],
        "key3a": ["value3a"],
        "key1b": ["value1b"],
        "key2b": ["value2b"],
        "key3b": ["value3ba", "value3bb"],
    }
    query = Query(tmp)

    assert query.has("key1a") is True
    assert query.has("key2a") is True
    assert query.has("key3a") is True
    assert query.has("key1b") is True
    assert query.has("key2b") is True
    assert query.has("key3b") is True
    assert query.has("key4") is False

    assert query.count_keyword() == 6

    assert query.count_value("key1a") == 1
    assert query.count_value("key2a") == 1
    assert query.count_value("key3a") == 1
    assert query.count_value("key1b") == 1
    assert query.count_value("key2b") == 1
    assert query.count_value("key3b") == 2

    # delete pairs
    del query["key1a"]
    assert query.count_keyword() == 5
    del query["key2a"]
    assert query.count_keyword() == 4
    # add more pairs
    query["key4"] = ["value4a", "value4b"]
    assert query.has("key4") is True
    assert query.count_keyword() == 5
    # delete all pairs
    query.clear()
    assert query.count_keyword() == 0
    assert query.count_value("key1a") == 0
    assert query.count_value("key2a") == 0
    assert query.count_value("key3a") == 0
    assert query.count_value("key1b") == 0
    assert query.count_value("key2b") == 0
    assert query.count_value("key3b") == 0
    assert query.count_value("key4") == 0


if __name__ == "__main__":
    test_query_typename()
    test_query_empty()
    test_query_clear()
