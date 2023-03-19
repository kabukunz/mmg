#!/bin/bash

# clean
rm -rf client/build
rm -rf client/bin

# build mmg
client/build_mmg2d.sh
client/compile_mmg2d.sh
client/install_mmg2d.sh

# build client
cmake -B client/build -G "Ninja" -DCMAKE_BUILD_TYPE=Release
cmake --build client/build

# test client
client/bin/mmg2dclient \
    libexamples/mmg2d/adaptation_example0/example0_a/init.mesh \
    client/init_out.mesh
