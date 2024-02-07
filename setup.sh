#!/usr/bin/env bash
set -euo pipefail

CC=${CC:-clang}

# make sure we start in right dir
script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd -P)
cd ${script_dir}

# prep
cmake -B build -DCMAKE_C_COMPILER=${CC} -DCMAKE_BUILD_TYPE=Release

# build
cd build
make -j4

# pack
cpack -G DEB
