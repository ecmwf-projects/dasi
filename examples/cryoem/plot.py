
import argparse
import datetime
import json

import matplotlib.pyplot as plt
import numpy as np

from dasi import DASI


def parse_cmdline_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("-u", "--user", default="0000-0001-2345-9876")
    parser.add_argument("-l", "--lab", default="CEITEC")
    parser.add_argument("-p", "--project", default="80s")
    parser.add_argument("-t", "--timestamp",
        default=datetime.datetime.now().strftime("%Y%m%d%H%M"))
    parser.add_argument("-P", "--processing",
        choices=["g1", "g2"], default="g1")
    parser.add_argument("-T", "--type", default="spa",
        choices=["spa", "tomo", "edt", "FIB-SEM", "screening"])
    parser.add_argument("-o", "--output", default="sample.png")
    return parser.parse_args()


def read_handle(handle):
    buf = bytearray()
    chunk = 1024
    with handle:
        while True:
            data = handle.read(chunk)
            read_len = len(data)
            buf += data
            if read_len < chunk:
                break
    return buf


if __name__ == "__main__":
    args = parse_cmdline_args()

    dasi = DASI("dasi-config.yml")
    query = {
        "User": [args.user],
        "Project": [args.project],
        "DateTime": [args.timestamp],
        "DataProcessing": [args.processing],
        "Type": [args.type],
        "Object": ["metadata", "images"],
    }

    result = dasi.get(query)
    sample = {key["Object"]: read_handle(handle)
              for key, handle in result}

    metadata = json.loads(sample["metadata"].decode("utf-8"))
    num_images = metadata["NumberOfImages"]
    image_size = tuple(metadata["ImageSize"])
    cols = 3
    rows, rest = divmod(num_images, cols)
    if rest > 0:
        rows += 1

    images = np.frombuffer(sample["images"], dtype=np.float32)
    images = images.reshape((num_images,) + image_size)

    fig = plt.figure()
    for i, img in enumerate(images, start=1):
        ax = fig.add_subplot(rows, cols, i)
        ax.imshow(img, vmin=0., vmax=1.)

    fig.suptitle(
        f"{metadata['Type']} images for project {metadata['Project']}, "
        + f"timestamp {metadata['DateTime']}, processed using {metadata['Processing']},\n"
        + f"microscope temperature: {metadata['Temperature']:.4g} K"
    )
    fig.savefig(args.output)