#!/bin/bash

# clean
rm -rf client/build
rm -rf client/bin

# build mmg
client/build_mmg2d.sh
client/compile_mmg2d.sh
client/install_mmg2d.sh

export MMG_SRC=$(pwd)
export MMG_INST="${MMG_SRC}/install"

cd client

# build client
cmake -B build -G "Ninja" \
-DCMAKE_BUILD_TYPE=Release \
-DMMG_SRC=$MMG_SRC \
-DMMG_INST=$MMG_INST

cmake --build build

# test client
bin/mmg2dclient \
    ../libexamples/mmg2d/adaptation_example0/example0_a/init.mesh \
    init_out.mesh

cd ..

rm -rf client/init*.*
