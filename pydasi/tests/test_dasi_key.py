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

from dasi import Key


def test_key_typename():
    """
    Construct a key and assert its type name.
    """

    from cffi import FFI

    key = Key()

    assert FFI().typeof(key._cdata).cname == "dasi_key_t *"


def test_key_clear():
    """
    Assert clear() and erase() keys.
    """

    key = Key()

    count = key.count()
    assert count == 0

    # add keyword:value pair
    key["key1"] = "value1"

    count = key.count()
    assert count == 1

    # insert pairs from dict
    pair = {"key2": "value2", "key3": "value3"}
    key.insert(pair)

    stat = key.has("key2")
    assert stat is True

    count = key.count()
    assert count == 3

    del key["key1"]

    stat = key.has("key1")
    assert stat is False

    count = key.count()
    assert count == 2

    key.clear()

    count = key.count()
    assert count == 0


def test_key_empty():
    """
    Construct empty keys and assert keys are empty by count() and has().
    """

    key = Key()

    stat = key.has("key1")
    assert stat is False

    stat = key.has("key2")
    assert stat is False

    count = key.count()
    assert count == 0

    # add keyword:value pair
    key["key1"] = "value1"

    # get the value of keyword 'key1'
    value1 = key["key1"]
    assert value1 == "value1"

    stat = key.has("key1")
    assert stat is True

    count = key.count()
    assert count == 1


def test_key_dictionary():
    """
    Construct a key from dictionary and assert values.
    """

    # dict version
    pair = {"key1": "value1"}
    key = Key(pair)

    stat = key.has("key1")
    assert stat is True

    # get the value of keyword 'key1'
    value1 = key["key1"]
    assert value1 == "value1"

    count = key.count()
    assert count == 1

    # add keyword:value pair
    key["key2"] = "value2"

    stat = key.has("key2")
    assert stat is True

    # get the value of keyword 'key2'
    value2 = key["key2"]
    assert value2 == "value2"

    count = key.count()
    assert count == 2

    # insert pairs from dict
    pair = {"key3": "value3", "key4": "value4"}
    key.insert(pair)

    stat = key.has("key3")
    assert stat is True
    stat = key.has("key4")
    assert stat is True

    # get the value of keyword 'key3'
    value3 = key["key3"]
    assert value3 == "value3"
    # get the value of keyword 'key4'
    value4 = key["key4"]
    assert value4 == "value4"

    count = key.count()
    assert count == 4


if __name__ == "__main__":
    test_key_typename()
    test_key_clear()
    test_key_empty()
    test_key_dictionary()
