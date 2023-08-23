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


from helper import cmdline_args, plot_histogram

from dasi import Dasi


if __name__ == "__main__":
    args = cmdline_args()

    query = {
        "UserID": [args.UserID],
        "Institute": [args.Institute],
        "Project": [args.Project],
        "Type": [args.Type],
        "Directory": [args.Directory],
        "Date": ["01-01-2023"],
    }

    session = Dasi("./dasi.yml")

    # 1- Query the file names
    names = []
    for item in session.list(query):
        names.append(item.key["Name"])

    query["Name"] = names

    # 2- Retrieve the data
    retrieved = session.retrieve(query)
    if len(retrieved) < 2:
        exit("Query could not return any results!\n{}\n".format(query))

    # 3- Map the file name and data
    files: dict[str, bytearray] = {}
    for r in retrieved:
        name = r.key["Name"]
        files[name] = r.data

    # 4- Work with the files
    for name, data in files.items():
        print("--- [%s] ---" % name)
        if name.endswith("mdoc"):  # mdoc file
            print("Content: %s" % data.decode())
        elif name.endswith("tif"):  # image file
            plot_histogram(data, name)

    print("Finished!")
