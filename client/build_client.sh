#!/bin/bash

# clean
rm -rf client/build
rm -rf client/bin
rm -rf client/init*.*

# build mmg
client/build_mmg2d.sh
client/compile_mmg2d.sh
client/install_mmg2d.sh

cd client

# build client
cmake -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release
cmake --build build

# test client
bin/mmg2dclient \
    ../libexamples/mmg2d/adaptation_example0/example0_a/init.mesh \
    init_out.mesh

cd ..
