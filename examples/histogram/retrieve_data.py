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
from ast import literal_eval
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
    return parser.parse_args()


def plot_histogram(image, name: str):
    import matplotlib.pyplot as plt

    print("[plot] - Name: {}".format(name))

    plt.hist(image, density=True, bins=40)
    plt.xlabel("Data")
    plt.ylabel("Value")
    plt.title("Histogram of {}".format(name))
    plt.savefig("./{}.png".format(name))


if __name__ == "__main__":
    args = parse_cmdline_args()

    dasi = Dasi("dasi-config.yml")

    query = {
        "User": [args.user],
        "Project": [args.project],
        "DateTime": [args.timestamp],
        "Processing": [args.processing],
        "Type": [args.type],
        "Object": ["metadata", "image0", "image1", "image2", "image3", "image4"],
    }

    # Retrieve results from query
    retrieved = dasi.retrieve(query)
    print(len(retrieved))
    if len(retrieved) < 2:
        exit("Query could not return any results!\n{}\n".format(query))

    # Read data by looping over "Object": "metadata" and "images"
    results = {}
    for r in retrieved:
        key, data = r.read()
        results[key["Object"]] = data

    # Metadata
    metadata = literal_eval(results["metadata"].decode("utf-8"))
    lab = metadata["Laboratory"]
    num_img = metadata["NumberOfImages"]
    print("[metadata] - Laboratory: {}".format(lab))
    print("[metadata] - # of images: {}".format(num_img))

    # Images
    image0 = results["image0"]

    # Work with the retrieved image
    plot_histogram(image0, "histogram_0")
