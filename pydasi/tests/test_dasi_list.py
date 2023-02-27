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

from dasi import Dasi, DASIException, Key

__simple_data_0__ = b"TESTING SIMPLE LIST 00000000000"
__simple_data_1__ = b"TESTING SIMPLE LIST 11111111111"
__simple_data_2__ = b"TESTING SIMPLE LIST 22222222222"

__list_0__ = {
    "key1": "value0",
    "key2": "123",
    "key3": "value1",
    "key1a": "value1",
    "key2a": "value1",
    "key3a": "321",
    "key1b": "value1",
    "key2b": "value1",
    "key3b": "value1",
}

__list_1__ = {
    "key1": "value1",
    "key2": "123",
    "key3": "value1",
    "key1a": "value1",
    "key2a": "value1",
    "key3a": "321",
    "key1b": "value1",
    "key2b": "value1",
    "key3b": "value1",
}

__list_2__ = {
    "key1": "value2",
    "key2": "123",
    "key3": "value1",
    "key1a": "value1",
    "key2a": "value1",
    "key3a": "321",
    "key1b": "value1",
    "key2b": "value1",
    "key3b": "value1",
}

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


def test_simple_list(dasi_config_file):
    """
    Test Dasi list
    """

    dasi = Dasi(dasi_config_file)

    try:
        dasi.archive(__list_0__, __simple_data_0__)
        dasi.archive(__list_1__, __simple_data_1__)
        dasi.archive(__list_2__, __simple_data_2__)
    except DASIException:
        pytest.fail("Cannot archive!")

    dasi.flush()

    query = {
        "key2": ["123"],
        "key3": ["value1"],
        "key1a": ["value1"],
        "key2a": ["value1"],
        "key3a": ["321"],
        "key1b": ["value1"],
        "key2b": ["value1"],
        "key3b": ["value1"],
    }

    keys = []
    for item in dasi.list(query):
        keys.append(item.key)

    assert keys[0] == Key(__list_0__)
    assert keys[1] == Key(__list_1__)
    assert keys[2] == Key(__list_2__)


if __name__ == "__main__":
    retcode = pytest.main()
    print("Return Code: ", retcode)
