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
def cmake_cache_value(cache, name):
    """Read one typed value from a configured CMake cache."""
    prefix = name + ":"
    for line in cache.read_text(encoding="utf-8").splitlines():
        if line.startswith(prefix):
            _, value = line.split("=", 1)
            return value
    raise ValueError("{} has no {} entry".format(cache, name))


def symbol_address(readelf, elf, name):
    """Read one defined symbol address from an ELF image."""
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


def load_image(readelf, elf, binary):
    """Load a flat image and derive its half-open linked address range."""
    start = symbol_address(readelf, elf, "__region_code_start")
    data = binary.read_bytes()
    return start, start + len(data), data


def combine_images(images):
    """Place non-overlapping images in one erased-byte-filled address span."""
    images.sort(key=lambda image: image[0])
    if images[0][1] > images[1][0]:
        raise ValueError("bootloader load images overlap")

    base = images[0][0]
    result = bytearray([ERASED_IMAGE_BYTE]) * (images[-1][1] - base)
    for start, _, data in images:
        offset = start - base
        result[offset:offset + len(data)] = data
    return base, result


# Command-line entry ---------------------------------------------------------
def main():
    """Parse two image pairs and publish their combined binary."""
    parser = argparse.ArgumentParser(
        description="Combine two flat images using their linked code addresses."
    )
    parser.add_argument("--readelf")
    parser.add_argument("--cmake-cache", type=Path)
    parser.add_argument("--first-elf", type=Path, required=True)
    parser.add_argument("--first-bin", type=Path, required=True)
    parser.add_argument("--second-elf", type=Path, required=True)
    parser.add_argument("--second-bin", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if bool(args.readelf) == bool(args.cmake_cache):
        parser.error("select exactly one of --readelf or --cmake-cache")
    readelf = args.readelf
    if args.cmake_cache:
        readelf = cmake_cache_value(args.cmake_cache, "CMAKE_READELF")

    images = [
        load_image(readelf, args.first_elf, args.first_bin),
        load_image(readelf, args.second_elf, args.second_bin),
    ]
    base, result = combine_images(images)
    args.output.write_bytes(result)
    print("Generated {} at base {:#x}.".format(args.output, base))


if __name__ == "__main__":
    main()
