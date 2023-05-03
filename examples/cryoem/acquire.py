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
from io import BytesIO

from microscope import Microscope

from dasi import Dasi


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
    parser.add_argument("-n", "--num-images", type=int, default=5)
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_cmdline_args()

    metadata = {
        "User": args.user,
        "Laboratory": args.lab,
        "Project": args.project,
        "DateTime": args.timestamp,
        "Processing": args.processing,
        "Type": args.type,
    }
    microscope = Microscope(num_images=args.num_images, metadata=metadata)

    dasi = Dasi("dasi-config.yml")
    key = {
        "User": args.user,
        "Project": args.project,
        "DateTime": args.timestamp,
        "Processing": args.processing,
        "Type": args.type,
    }

    key["Object"] = "metadata"
    metadata = microscope.get_metadata()
    dasi.archive(key, json.dumps(metadata).encode("utf-8"))

    key["Object"] = "images"
    output = BytesIO()
    for image in microscope.acquire_images():
        output.write(image.tobytes())
    dasi.archive(key, output.getbuffer())
