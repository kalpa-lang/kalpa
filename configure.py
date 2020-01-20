#!/usr/bin/env python3

import argparse
import glob as glob_lib
import os
from pathlib import Path
import subprocess
import sys


DEFAULT_CXX="c++"
DEFAULT_CXXFLAGS_COMMON = "-Wall -Wextra -std=c++17 -fPIC"
DEFAULT_CXXFLAGS_DEBUG = "-O0 -g"
DEFAULT_CXXFLAGS_RELEASE = "-O3 -flto"
DEFAULT_DEPFLAGS = "-MMD -MF $out.d"

NINJA_TEMPLATE = \
"""rule cxx
    depfile = $out.d
    command = {cxx} {cxxflags} -c $in -o $out

rule link
    command = {ld} $in ${ldflags} -o $out

{body}
default kalpa
"""

NINJA_OBJECT_TEMPLATE = \
"""build {dst}: cxx {src}
"""

NINJA_EXEC_TEMPLATE = \
"""build {dst}: link {src}
"""


def main():
    args = parse_args()

    root = Path(args.root or os.path.dirname(__file__))

    if args.cxxflags is None:
        cxxflags = DEFAULT_CXXFLAGS_COMMON

        if args.release:
            cxxflags += " " + DEFAULT_CXXFLAGS_RELEASE
        else:
            cxxflags += " " + DEFAULT_CXXFLAGS_DEBUG
    else:
        cxxflags = args.cxxflags

    cxxflags += " " + pkg_config("fmt", "cflags")

    ldflags = args.ldflags or cxxflags
    ldflags += " " + pkg_config("fmt", "libs")

    objects = []
    object_dsts = []

    for src_path in glob(root / "src" / "*.cc"):
        obj, dst = make_object(root, src_path)
        objects.append(obj)
        object_dsts.append(dst)

    kalpa = make_exec(root, "kalpa", object_dsts)

    ninja = NINJA_TEMPLATE.format(
        cxx=args.cxx, cxxflags=cxxflags + " " + args.depflags,
        ld=args.ld, ldflags=ldflags,
        body="".join(objects + ["\n", kalpa]),
    )

    with open("build.ninja", "w") as ninja_file:
        ninja_file.write(ninja)


def parse_args():
    parser = argparse.ArgumentParser(
        description="generate kalpa build scripts",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )

    parser.add_argument(
        "--root",
        help="project root directory, equals dirname of configure.py by default",
    )

    parser.add_argument(
        "-r", "--release",
        action="store_true",
        help="enable release mode",
    )

    parser.add_argument(
        "-c", "--cxx",
        default=DEFAULT_CXX,
        help="C++ compiler",
    )

    parser.add_argument(
        "-C", "--cxxflags",
        help="C++ compiler flags",
    )

    parser.add_argument(
        "-D", "--depflags",
        default=DEFAULT_DEPFLAGS,
        help="depfile generation flags",
    )

    parser.add_argument(
        "-l", "--ld",
        default=DEFAULT_CXX,
        help="linker",
    )

    parser.add_argument(
        "-L", "--ldflags",
        help="linker flags, equals cxxflags by default",
    )

    return parser.parse_args()


def pkg_config(pkg, mode):
    return subprocess.check_output(["pkg-config", f"--{mode}", pkg]).decode()[:-1]


def glob(path):
    if isinstance(path, Path):
        path = str(path)

    return [*map(Path, glob_lib.glob(path))]


def make_object(root, path):
    dst_path = path.relative_to(root).with_suffix(".o")
    return NINJA_OBJECT_TEMPLATE.format(dst=dst_path, src=path), dst_path


def make_exec(root, dst, paths):
    src = " ".join(str(path).replace(" ", "$ ") for path in paths)
    return NINJA_EXEC_TEMPLATE.format(dst=dst, src=src)


if __name__ == "__main__":
    sys.exit(main() or 0)
