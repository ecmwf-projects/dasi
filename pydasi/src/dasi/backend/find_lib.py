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

import os, errno
from platform import system as p_system

from dasi.utils import log


class FindLib:
    """
    Finds the library given the name and path to search for.
    """

    def __init__(self, name: str, dir: str):
        self._log = log.getLogger(__package__)

        # the env var overwrites built-in
        for dir_ in ("DASI_DIR", "dasi_DIR"):
            env_dir_ = os.environ.get(dir_)
            if env_dir_ and os.path.exists(env_dir_):
                dir = env_dir_
                break

        dir = os.path.join(dir, "lib")

        self._log.info("Searching '%s' in '%s'", name, dir)

        platform = p_system()
        self._log.debug("- platform: %s", platform)

        self.__path = ""
        if platform == "Linux":
            self.__name = "lib" + name + ".so"
            self.__path = os.path.join(dir, "linux", self.__name)
        elif platform == "Darwin":
            self.__name = "lib" + name + ".dylib"
            self.__path = os.path.join(dir, "darwin", self.__name)
        elif platform == "Windows":
            raise NotImplementedError("Windows OS is not supported!")

        self._log.info("- name: %s", self.__name)

        if not os.path.exists(self.__path):
            raise FileNotFoundError(
                errno.ENOENT, os.strerror(errno.ENOENT), self.__path
            )

        self._log.debug("- path: %s", self.__path)

    @property
    def path(self):
        return self.__path
