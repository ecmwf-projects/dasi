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

import os


class DirectoryData:
    """
    Store the files in a directory.
    """

    def __init__(self, path=None):
        self.__metadata = {}
        self.__filepaths = []
        with os.scandir(path) as it:
            for entry in it:
                if entry.is_file():
                    self.__filepaths.append(entry.path)

        self.__metadata["NumberOfFiles"] = len(self.__filepaths)

    def get_files(self):
        for path in self.__filepaths:
            filename = os.path.basename(path)
            with open(path, "rb") as f:
                yield filename, f.read()

    @property
    def metadata(self):
        return self.__metadata
