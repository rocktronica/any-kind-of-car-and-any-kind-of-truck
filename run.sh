#!/bin/bash

{

# Exit on error
set -o errexit
set -o errtrace

stub="any-kind-of-car-and-any-kind-of-truck"

ardens="/Applications/Ardens/ArdensPlayer.app/Contents/MacOS/ArdensPlayer"

input_path="arduino/$stub"
build_path="$PWD/build"

mkdir -pv "$build_path" >/dev/null

arduino-cli compile \
    --fqbn arduboy:avr:arduboy \
    --build-path="$build_path" \
    "$input_path"

$ardens file="$build_path/$stub.ino.hex"

}