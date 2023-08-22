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

import yaml

# from pathlib import Path, PosixPath

__default__ = """---
schema: ./schema
catalogue: toc
store: file
spaces:
- handler: Default
  roots:
  - path: ./root1
"""

__default_path__ = "./dasi.yml"

# def dasi_schema_file(dasi_config_dir):
#     file_ = dasi_config_dir / "schema"
#     file_.write_text(__dasi_schema__)
#     return file_


class Config:
    def __init__(self, path: str = __default_path__) -> None:
        from os import path as ospath
        from dasi.utils import log

        self._log = log.getLogger(__name__)

        self._log.debug("init config: %s", path)

        self._path = path

        if ospath.exists(self._path):
            with open(self._path, "r") as file:
                self._cfg = yaml.safe_load(file)

        if self._cfg is None:
            self._log.warning("Using default config!")
            self._cfg = yaml.safe_load(__default__)
            self.dump()

        self._log.debug("- config: %s", yaml.dump(self._cfg))

    def __enter__(self):
        return self._cfg

    def __exit__(self, *args):
        self.dump()

    @property
    def path(self):
        return self._path

    def dump(self):
        self._log.debug("dump config")
        with open(self._path, "w+") as f:
            yaml.dump(self._cfg, f)

    def schema(self, path: str):
        self._cfg["schema"] = path

    def root(self, path: str):
        self._cfg["spaces"]["handler"]["roots"].add("path", path)
