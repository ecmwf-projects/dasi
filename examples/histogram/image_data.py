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


class ImageData:
    def __init__(self, directory=None):
        self.__metadata = {}
        self.__files = []
        with os.scandir(directory) as it:
            for entry in it:
                if entry.name.endswith(".tif") and entry.is_file():
                    self.__files.append(entry.path)

        self.__metadata["NumberOfImages"] = len(self.__files)

    def get_images(self):
        for idx, file in enumerate(self.__files):
            with open(file, "rb") as f:
                yield idx, f.read()

    @property
    def metadata(self):
        return self.__metadata
