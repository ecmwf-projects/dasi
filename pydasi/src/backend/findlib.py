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

import errno
import os
from platform import system as p_system

from utils import get_logger

logger = get_logger(name=__package__)

__file_dir__ = os.path.dirname(__file__)
"""current file path directory"""


class FindLib:
    """
    Finds the library given the name.
    """

    def __init__(self, name: str):
        logger.info("Searching for the library: '%s'", name)

        platform = p_system()
        logger.debug("- platform: %s", platform)

        path = ""
        if platform == "Linux":
            name += ".so"
            path = os.path.join(__file_dir__, "linux")
        elif platform == "Darwin":
            name += ".dylib"
            path = os.path.join(__file_dir__, "darwin")
        elif platform == "Windows":
            raise NotImplementedError("Windows OS is not yet supported!")

        logger.debug("- library name: %s", name)

        # the env var overwrites built-in
        for dir_ in ("DASI_DIR", "dasi_DIR"):
            env_dir_ = os.environ.get(dir_)
            if env_dir_:
                path = os.path.join(env_dir_, "lib")

        path = os.path.join(path, name)
        if os.path.exists(path):
            self.path = path
            logger.debug("- library: %s", self.path)
        else:
            raise FileNotFoundError(
                errno.ENOENT, os.strerror(errno.ENOENT), path
            )
