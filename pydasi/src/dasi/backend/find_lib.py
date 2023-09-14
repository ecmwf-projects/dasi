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

        self._log.info("Searching '%s' ...", name)

        platform = p_system()
        self._log.debug("- platform: %s", platform)

        if platform == "Linux":
            self.__name = "lib" + name + ".so"
        elif platform == "Darwin":
            self.__name = "lib" + name + ".dylib"
        elif platform == "Windows":
            raise NotImplementedError("Windows OS is not supported!")

        self._log.info("- name: %s", self.__name)

        self.__path = ""
        if platform == "Linux":
            self.__path = os.path.join(dir, "lib", "linux", self.__name)
        elif platform == "Darwin":
            self.__path = os.path.join(dir, "lib", "darwin", self.__name)

        # env vars take priority over dir
        for env_var_ in ("DASI_DIR", "dasi_DIR", "LD_LIBRARY_PATH"):
            env_path_ = os.getenv(env_var_)
            if env_path_:
                for edir_ in env_path_.split(":"):
                    epath_ = os.path.join(edir_, self.__name)
                    if os.path.isfile(epath_):
                        self.__path = epath_
                        break

        if not os.path.exists(self.__path):
            raise FileNotFoundError(
                errno.ENOENT, os.strerror(errno.ENOENT), self.__path
            )

        self._log.debug("- path: %s", self.__path)

    @property
    def path(self):
        return self.__path
