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
    plt.close()


if __name__ == "__main__":
    args = parse_cmdline_args()

    dasi = Dasi("dasi-config.yml")

    query = {
        "User": [args.user],
        "Project": [args.project],
        "DateTime": [args.timestamp],
        "Processing": [args.processing],
        "Type": [args.type],
    }

    # Get the list of objects in the store (metadata and filenames)
    objects = []
    for item in dasi.list(query):
        objects.append(item.key["Object"])
    query["Object"] = objects

    # Using the query, retrieve the results
    retrieved = dasi.retrieve(query)
    if len(retrieved) < 2:
        exit("Query could not return any results!\n{}\n".format(query))

    # Read the data into results
    results = {}
    for r in retrieved:
        object = r.key["Object"]
        results[object] = r.data

    # Below here is an example for working with the retrieved data

    # Option 1: access data individually (e.g., result["blabla"])
    metadata = literal_eval(results["metadata"].decode("utf-8"))
    lab_name = metadata["Laboratory"]
    num_files = metadata["NumberOfFiles"]
    print("-----------------------------")
    print("[metadata] - Laboratory: {}".format(lab_name))
    print("[metadata] - # of files: {}".format(num_files))

    # Option 2: Loop over the objects and work as needed
    # here, we work with files only
    for object in objects:
        print("-----------------------------")
        print("Object: ", object)
        data = results[object]
        if object.endswith("tif"):  # this object is an "image" file
            # plot the image
            plot_histogram(data, object)
        elif object.endswith("mdoc"):  # this object is a "mdoc" file
            file = data.decode()
            print(file)
