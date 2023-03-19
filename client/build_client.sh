#!/bin/bash

# clean
rm -rf client/build
rm -rf client/bin

# build mmg
client/build_mmg2d.sh

export MMG_SRC=$(pwd)
export MMG_INSTALL="${MMG_SRC}/install"

cd client

# build client
cmake -B build -G "Ninja" \
-DCMAKE_BUILD_TYPE=Release \
-DMMG_SRC=$MMG_SRC \
-DMMG_INSTALL=$MMG_INSTALL

cmake --build build

# test client
bin/mmg2dclient \
    ../libexamples/mmg2d/adaptation_example0/example0_a/init.mesh \
    bin/init_out.mesh

cd ..

