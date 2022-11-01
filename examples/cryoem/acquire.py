
import argparse
import datetime
from io import BytesIO
import json

from dasi import DASI

from microscope import Microscope

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

    dasi = DASI("dasi-config.yml")
    key = {
        "User": args.user,
        "Project": args.project,
        "DateTime": args.timestamp,
        "DataProcessing": args.processing,
        "Type": args.type,
    }

    metadata = microscope.get_metadata()
    output = BytesIO()
    for image in microscope.acquire_images():
        output.write(image.tobytes())

    key["Object"] = "metadata"
    dasi.put(key, json.dumps(metadata).encode("utf-8"))

    key["Object"] = "images"
    dasi.put(key, output.getbuffer())