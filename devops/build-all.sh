#!/bin/bash

cd "$(dirname "$0")/.." || exit

function build_all() {
    local red='\033[0;31m'
    local grey='\033[0;37m'
    local nc='\033[0m'

    for d in lab-*/*/ ; do
        trimmed_dir=$(echo "$d" | sed 's:/*$::')
        WORKDIR="${trimmed_dir}" make build
        if [ $? -ne 0 ]; then
            echo -e "${red}[ERROR]${nc} ${trimmed_dir} building failed."
            exit 1
        fi
        echo -e "${grey}[INFO]${nc} ${trimmed_dir} building completed."
        WORKDIR="${trimmed_dir}" make clean
    done
}

build_all
