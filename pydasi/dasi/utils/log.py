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

from logging import DEBUG, INFO, WARNING, getLogger
from logging.config import dictConfig

__logging_config__ = dict(
    version=1,
    formatters={
        "default": {
            "datefmt": "%d-%m-%Y [%H:%M:%S]",
            "format": "%(asctime)s %(name)-12s | %(levelname)-6s | %(message)s",
        },
        "compact": {
            "format": "%(name)-12s | %(message)s",
        },
    },
    handlers={
        "console": {
            "class": "logging.StreamHandler",
            "formatter": "default",
            "stream": "ext://sys.stdout",
            # "level": INFO,
        },
        "console_compact": {
            "class": "logging.StreamHandler",
            "formatter": "compact",
            # "level": INFO,
        },
        # "file": {
        #     "class": "logging.handlers.RotatingFileHandler",
        #     "formatter": "default",
        #     "filename": __package__ + ".log",
        #     "maxBytes": 1024,
        #     "backupCount": 3,
        #     "level": INFO,
        # },
    },
    root={
        "handlers": ["console_compact"],
        "level": WARNING,
    },
    loggers={
        "dasi": {
            "handlers": ["console"],
            "level": DEBUG,
            "propagate": False,
        },
        # "__main__": {  # if __name__ == '__main__'
        #     "handlers": ["console"],
        #     "level": DEBUG,
        #     "propagate": False,
        # },
    },
)

dictConfig(__logging_config__)


def get_logger(level=INFO, name=None):
    logger = getLogger(name)
    logger.setLevel(level)
    return logger
