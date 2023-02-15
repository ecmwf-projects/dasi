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


import pytest

from dasi import Dasi, DASIException

__simple_data_1__ = b"TESTING SIMPLE ARCHIVE 11111111111"
__simple_data_2__ = b"TESTING SIMPLE ARCHIVE 22222222222"
__simple_data_3__ = b"TESTING SIMPLE ARCHIVE 33333333333"

__dasi_schema__ = """
key2:  Integer;
key3a: Integer;

[ key1, key2, key3
    [ key1a, key2a, key3a
        [ key1b, key2b, key3b ]]]

"""

__dasi_config__ = """---
schema: {schema_file}
catalogue: toc
store: file
spaces:
  - roots:
    - path: {root_path}

"""


def dasi_schema_file(dasi_config_dir):
    file_ = dasi_config_dir / "schema"
    file_.write_text(__dasi_schema__)
    return file_


@pytest.fixture(scope="session")
def dasi_config_dir(tmp_path_factory):
    return tmp_path_factory.mktemp("config")


@pytest.fixture(scope="session")
def dasi_config_file(tmp_path_factory, dasi_config_dir):
    schema = dasi_schema_file(dasi_config_dir)
    dasi_config = __dasi_config__.format(
        schema_file=schema, root_path=tmp_path_factory.getbasetemp()
    )
    file_ = dasi_config_dir / "dasi_config.yml"
    file_.write_text(dasi_config)
    return file_


def test_create_schema_file(dasi_config_dir):
    """
    Test create a simple schema file
    """

    assert dasi_schema_file(dasi_config_dir).read_text() == __dasi_schema__


def test_simple_archive(dasi_config_file):
    """
    Test Dasi archiving
    """

    dasi = Dasi(dasi_config_file)

    key = {
        "key2": "123",
        "key3": "value1",
        "key1a": "value1",
        "key2a": "value1",
        "key3a": "321",
        "key1b": "value1",
        "key2b": "value1",
        "key3b": "value1",
    }

    try:
        key["key1"] = "value1"
        dasi.archive(key, __simple_data_1__)
        key["key1"] = "value2"
        dasi.archive(key, __simple_data_2__)
        key["key1"] = "value3"
        dasi.archive(key, __simple_data_3__)
    except DASIException:
        pytest.fail("Cannot archive!")


def test_simple_retrieve(dasi_config_file):
    """
    Test Dasi retrieve
    """

    dasi = Dasi(dasi_config_file)

    query = {
        "key1": ["value1", "value2", "value3"],
        "key2": ["123"],
        "key3": ["value1"],
        "key1a": ["value1"],
        "key2a": ["value1"],
        "key3a": ["321"],
        "key1b": ["value1"],
        "key2b": ["value1"],
        "key3b": ["value1"],
    }

    retrieved = dasi.retrieve(query)

    results = {}
    for r in retrieved:
        key, data = r.read()
        results[key["key1"]] = data

    assert results["value1"] == __simple_data_1__
    assert results["value2"] == __simple_data_2__
    assert results["value3"] == __simple_data_3__


if __name__ == "__main__":
    retcode = pytest.main()
    print("Return Code: ", retcode)
