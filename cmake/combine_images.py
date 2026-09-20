# Copyright (c) 2026 simple-boot contributors
# SPDX-License-Identifier: MIT
#
# File: cmake/combine_images.py
# Brief: Combine linked bootloader binaries into one address-preserving image.

# Imports --------------------------------------------------------------------
from __future__ import print_function

import argparse
from pathlib import Path
import subprocess


# Image constants ------------------------------------------------------------
ERASED_IMAGE_BYTE = 0xff


# ELF inspection -------------------------------------------------------------
def symbol_address(readelf, elf, name):
    output = subprocess.check_output(
        [readelf, "-W", "-s", str(elf)],
        universal_newlines=True,
        encoding="utf-8",
    )
    for line in output.splitlines():
        fields = line.split()
        if len(fields) >= 8 and fields[7] == name and fields[6] != "UND":
            return int(fields[1], 16)
    raise ValueError("{} has no {} symbol".format(elf, name))


# Image composition ----------------------------------------------------------
def main():
    parser = argparse.ArgumentParser(
        description="Combine two flat images using their linked code addresses."
    )
    parser.add_argument("--readelf", required=True)
    parser.add_argument("--first-elf", type=Path, required=True)
    parser.add_argument("--first-bin", type=Path, required=True)
    parser.add_argument("--second-elf", type=Path, required=True)
    parser.add_argument("--second-bin", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    images = []
    for elf, binary in ((args.first_elf, args.first_bin),
                        (args.second_elf, args.second_bin)):
        start = symbol_address(args.readelf, elf, "__region_code_start")
        data = binary.read_bytes()
        images.append((start, start + len(data), data))
    images.sort(key=lambda image: image[0])
    if images[0][1] > images[1][0]:
        raise ValueError("bootloader load images overlap")

    base = images[0][0]
    result = bytearray([ERASED_IMAGE_BYTE]) * (images[-1][1] - base)
    for start, _, data in images:
        offset = start - base
        result[offset:offset + len(data)] = data
    args.output.write_bytes(result)
    print("Generated {} at base {:#x}.".format(args.output, base))


if __name__ == "__main__":
    main()
