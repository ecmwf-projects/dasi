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

from logging import DEBUG, INFO, NOTSET, WARNING, CRITICAL, getLogger

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
            "formatter": "compact",
            "stream": "ext://sys.stdout",
            "level": INFO,
        },
        "file": {
            "class": "logging.handlers.RotatingFileHandler",
            "formatter": "default",
            "filename": "dasi.log",
            "maxBytes": 1024,
            "backupCount": 0,
            "level": DEBUG,
        },
    },
    root={
        "handlers": ["console"],
        "level": NOTSET,
    },
    loggers={
        "dasi.dasi": {
            "level": DEBUG,
            "propagate": True,
        },
    },
)


def init_logging() -> None:
    from logging.config import dictConfig

    if _check_debug_arg():
        __logging_config__["root"]["handlers"].append("file")  # type: ignore

    dictConfig(__logging_config__)


def _check_debug_arg() -> bool:
    from argparse import ArgumentParser

    parser = ArgumentParser()
    parser.add_argument(
        "--debug",
        action="store_true",
        help="debug outputs to log file.",
    )
    args, _ = parser.parse_known_args()

    return args.debug
