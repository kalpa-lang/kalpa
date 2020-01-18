#!/bin/sh

set -e

CXX=c++

FMT_LDFLAGS="$(pkg-config --libs fmt)"
FMT_CFLAGS="$(pkg-config --cflags fmt)"

$CXX \
    -std=c++17 \
    -Wall -Wextra \
    -O0 -g \
    $FMT_LDFLAGS $FMT_CFLAGS \
    src/*.cc -o kalpa
