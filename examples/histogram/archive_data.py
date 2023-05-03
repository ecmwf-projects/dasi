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

import argparse
import json

from dasi import Dasi

from directory_data import DirectoryData


def parse_cmdline_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("-u", "--user", default="0000-0001-2345-9876")
    parser.add_argument("-l", "--lab", default="CEITEC")
    parser.add_argument("-p", "--project", default="80s")
    parser.add_argument("-t", "--timestamp", default="202302102043")
    parser.add_argument("-P", "--processing", choices=["g1", "g2"], default="g1")
    parser.add_argument(
        "-T",
        "--type",
        default="spa",
        choices=["spa", "tomo", "edt", "FIB-SEM", "screening"],
    )
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_cmdline_args()

    store = DirectoryData(path="data")

    dasi = Dasi("dasi-config.yml")
    key = {
        "User": args.user,
        "Project": args.project,
        "DateTime": args.timestamp,
        "Processing": args.processing,
        "Type": args.type,
    }

    key["Object"] = "metadata"
    store.metadata.update(key)
    store.metadata.update({"Laboratory": args.lab})
    dasi.archive(key, json.dumps(store.metadata).encode("utf-8"))

    for filename, file in store.get_files():
        print("Archiving '{}' ...".format(filename))
        key["Object"] = filename
        dasi.archive(key, file)

    print("Archiving finished!")
