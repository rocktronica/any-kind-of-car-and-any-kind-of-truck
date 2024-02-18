#!/bin/bash

{

# Exit on error
set -o errexit
set -o errtrace

stub="any-kind-of-car-and-any-kind-of-truck"

ardens="/Applications/Ardens/ArdensPlayer.app/Contents/MacOS/ArdensPlayer"

input_path="arduino/$stub"
build_path="$PWD/build"

function help() {
    echo "\
Compile and emuluate


Usage:
$0
$0 -h   Show help and exit
"
}

if [ "$1" == '-h' ]; then
    help
    exit
fi

function run() {
    mkdir -pv "$build_path" >/dev/null

    while true; do
        arduino-cli compile \
            --fqbn arduboy:avr:arduboy \
            --build-path="$build_path" \
            "$input_path"

        $ardens file="$build_path/$stub.ino.hex" >/dev/null

        echo
        echo "!! Press CTRL+C to quit !!"

        echo
    done
}

if [ "$1" == '-h' ]; then
    help
    exit
fi

run "${@:1}"

}