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
import matplotlib.pyplot as plt
import numpy as np

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
    parser.add_argument("-o", "--output", default="sample.png")
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_cmdline_args()

    dasi = Dasi("dasi-config.yml")

    query = {
        "User": [args.user],
        "Project": [args.project],
        "DateTime": [args.timestamp],
        "Processing": [args.processing],
        "Type": [args.type],
        "Object": ["metadata", "images"],
    }

    #
    # Metadata
    retrieved = dasi.retrieve(query)
    if retrieved.count() != 2:
        exit("Query could not return results!\n{}\n".format(query))

    # read data by looping "Object": "metadata" and "images"
    results = {}
    for r in retrieved:
        key, data = r.read()
        results[key["Object"]] = data

    #
    # Metadata
    metadata = literal_eval(results["metadata"].decode("utf-8"))
    num_img = metadata["NumberOfImages"]
    img_size = tuple(metadata["ImageSize"])
    cols = 3
    rows, rest = divmod(num_img, cols)
    if rest > 0:
        rows += 1

    #
    # Images
    shape = (num_img,) + img_size
    images = np.frombuffer(results["images"], dtype=np.float32).reshape(shape)

    fig = plt.figure()
    for i, img in enumerate(images, start=1):
        ax = fig.add_subplot(rows, cols, i)
        ax.imshow(img, vmin=0.0, vmax=1.0)

    fig.suptitle(
        f"{metadata['Type']} images for project {metadata['Project']}, "
        + f"timestamp {metadata['DateTime']}\n"
        + f"processed using {metadata['Processing']}, "
        + f"microscope temperature: {metadata['Temperature']:.4g} K"
    )
    fig.savefig(args.output)
