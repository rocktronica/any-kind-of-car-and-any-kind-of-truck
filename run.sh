#!/bin/bash

{

# Exit on error
set -o errexit
set -o errtrace

stub="any-kind-of-car-and-any-kind-of-truck"

ardens="/Applications/Ardens/ArdensPlayer.app/Contents/MacOS/ArdensPlayer"

fbqn="arduboy:avr:arduboy"

input_path="arduino/$stub"
build_path="$PWD/build"

function help() {
    echo "\
CLI wrapper around Arduino and Ardens

Usage:
./run.sh -h         Show help and exit

./run.sh dev        Compile and emuluate
                    Looped! Quit emulator to refresh
"
}

if [ "$1" == '-h' ]; then
    help
    exit
fi

function compile() {
    mkdir -pv "$build_path" >/dev/null

    arduino-cli compile \
        --fqbn "$fbqn" \
        --build-path="$build_path" \
        "$input_path"
}

function emulate() {
    $ardens file="$build_path/$stub.ino.hex" >/dev/null
}

if [ "$1" == '-h' ]; then
    help
    exit
fi

if [ "$1" == 'dev' ]; then
    while true; do
        compile
        emulate

        echo
        echo "!! Press CTRL+C to quit !!"
        echo
    done

    exit
fi

help

}