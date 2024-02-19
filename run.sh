#!/bin/bash

{

# Exit on error
set -o errexit
set -o errtrace

stub="any-kind-of-car-and-any-kind-of-truck"

ardens="/Applications/Ardens/ArdensPlayer.app/Contents/MacOS/ArdensPlayer"

fbqn="arduboy:avr:arduboy"
port="/dev/cu.usbmodem143101"

input_path="arduino/$stub"
build_path="$PWD/build"

function help() {
    echo "\
CLI wrapper around Arduino and Ardens

Usage:
./run.sh -h                 Show help and exit

./run.sh dev                Compile and emuluate
                            Looped! Quit emulator to refresh
./run.sh deploy             Compile and upload
                            Default port: $port
./run.sh deploy -p PORT     ^ with arbitrary port
                            Run 'arduino-cli board list' for list

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

function upload() {
    arduino-cli upload \
        --fqbn "$fbqn" \
        --port "$port" \
        --input-file "$build_path/$stub.ino.hex"
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

if [ "$1" == 'deploy' ]; then
    if [ "$2" == '-p' ]; then
        port="$3"
    fi

    compile
    upload

    exit
fi

help

}