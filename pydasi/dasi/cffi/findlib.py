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
import platform

from dasi.utils import get_logger

logger = get_logger(name=__package__)

__file_dir__ = os.path.dirname(__file__)
"""current file path directory"""


class FindLib:
    def __init__(self, lib_name: str):
        logger.info("Searching for the library: '%s'", lib_name)

        platform_system = platform.system()
        logger.debug("- platform: %s", platform_system)

        lib_path = ""
        if platform_system == "Linux":
            lib_name += ".so"
            lib_path = os.path.join(__file_dir__, "linux")
        elif platform_system == "Darwin":
            lib_name += ".dylib"
            lib_path = os.path.join(__file_dir__, "darwin")
        elif platform_system == "Windows":
            raise NotImplementedError("Windows OS is not yet supported!")

        logger.debug("- library name: %s", lib_name)

        # path_ = osenv.get("LD_LIBRARY_PATH")
        # if path_:
        #     lib_path = os.path.join(path_, "lib")

        # the env var overwrites built-in
        for dir_ in ("DASI_DIR", "dasi_DIR"):
            path_ = os.environ.get(dir_)
            if path_:
                lib_path = os.path.join(path_, "lib")

        library = os.path.join(lib_path, lib_name)
        if os.path.exists(library):
            self.library = library
            logger.debug("- library: %s", self.library)
        else:
            raise FileNotFoundError(
                errno.ENOENT, os.strerror(errno.ENOENT), library
            )
